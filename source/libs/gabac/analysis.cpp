/**
 * @file
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "analysis.h"

#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stack>
#include <string>
#include <vector>

#include "block_stepper.h"
#include "configuration.h"
#include "constants.h"
#include "data_block.h"
#include "encode_cabac.h"
#include "stream_handler.h"


namespace gabac {

struct Snapshot
{
    std::vector<DataBlock> streams;
};

struct TraversalInfo
{
    const IOConfiguration *ioconf{};
    EncodingConfiguration currConfig;
    EncodingConfiguration bestConfig;

    TransformedSequenceConfiguration bestSeqConfig;

    size_t currStreamIndex{};

    size_t currSequenceSize{};
    size_t bestSequenceSize{};

    size_t currTotalSize{};
    size_t bestTotalSize{};

    int numConfigs{};

    std::stack<Snapshot> stack;
};

const AnalysisConfiguration& getCandidateConfig(){
    static const AnalysisConfiguration config = {
            {  // Wordsizes
                    1,
                       2,
                          4,
                             8
            },
            {  // Sequence Transformations
                    gabac::SequenceTransformationId::no_transform,
                       gabac::SequenceTransformationId::equality_coding,
                          gabac::SequenceTransformationId::match_coding,
                             gabac::SequenceTransformationId::rle_coding
            },
            {  // Match libs window sizes
                    32,
                       256
            },
            {  // RLE Guard
                    255
            },
            {  // LUT transform
                    false,
                       true
            },
            {  // Diff transform
                    false,
                       true
            },
            {  // Binarizations (unsigned)
                    gabac::BinarizationId::BI,
                       gabac::BinarizationId::TU,
                          gabac::BinarizationId::EG,
                             gabac::BinarizationId::TEG
            },
            {  // Binarizations (signed)
                    gabac::BinarizationId::SEG,
                       gabac::BinarizationId::STEG
            },
            {  // Binarization parameters (TEG and STEG only)
                    1, 2, 3, 5, 7, 9,
                    15, 30, 255
            },
            {  // Context modes
                    gabac::ContextSelectionId::bypass,
                       gabac::ContextSelectionId::adaptive_coding_order_0,
                          gabac::ContextSelectionId::adaptive_coding_order_1,
                             gabac::ContextSelectionId::adaptive_coding_order_2
            },
            {  // LUT order
                    0,
                       1
            },
            0,
            0
    };
    return config;
}

//------------------------------------------------------------------------------

void getOptimumOfBinarizationParameter(const AnalysisConfiguration& aconf,
                                       TraversalInfo *info
){
    for (const auto& transID : aconf.candidateContextSelectionIds) {
        info->currConfig.transformedSequenceConfigurations[info->currStreamIndex].contextSelectionId = transID;
        info->stack.push(info->stack.top());
        size_t maxSize =
                std::min(info->bestSequenceSize - info->currSequenceSize, info->bestTotalSize - info->currTotalSize) -
                sizeof(uint32_t);

        info->numConfigs += 1;
        gabac::encode_cabac(
                info->currConfig.transformedSequenceConfigurations[info->currStreamIndex].binarizationId,
                info->currConfig.transformedSequenceConfigurations[info->currStreamIndex].binarizationParameters,
                info->currConfig.transformedSequenceConfigurations[info->currStreamIndex].contextSelectionId,
                &(info->stack.top().streams.front()), maxSize
        );
        info->currSequenceSize += sizeof(uint32_t) + info->stack.top().streams.front().size();
        if (info->bestSequenceSize > info->currSequenceSize) {
            info->bestSequenceSize = info->currSequenceSize;
            info->bestSeqConfig = info->currConfig.transformedSequenceConfigurations[info->currStreamIndex];
        }

        info->currSequenceSize -= sizeof(uint32_t) + info->stack.top().streams.front().size();

        info->stack.pop();
    }
}

//------------------------------------------------------------------------------

static void getMinMax(const gabac::DataBlock& b, uint64_t *umin, uint64_t *umax, int64_t *smin, int64_t *smax){
    gabac::BlockStepper r = b.getReader();
    *umin = std::numeric_limits<uint64_t>::max();
    *umax = std::numeric_limits<uint64_t>::min();
    *smin = std::numeric_limits<int64_t>::max();
    *smax = std::numeric_limits<int64_t>::min();
    while (r.isValid()) {
        uint64_t val = r.get();
        *umax = std::max(*umax, val);
        *umin = std::min(*umin, val);
        auto sval = int64_t(val);
        *smax = std::max(*smax, sval);
        *smin = std::min(*smin, sval);
        r.inc();
    }
}

//------------------------------------------------------------------------------

static uint64_t getMaxOfStream(SequenceTransformationId trans, uint8_t stream, uint64_t max) {
    if((trans == SequenceTransformationId::rle_coding || trans == SequenceTransformationId::match_coding) && stream > 0)
        return std::numeric_limits<uint32_t>::max();
    if(trans == SequenceTransformationId::equality_coding && stream == 1)
        return 1;
    return max;
}

//------------------------------------------------------------------------------

void getOptimumOfBinarization(const AnalysisConfiguration& aconf,
                              TraversalInfo *info
){
    uint64_t min, max;
    int64_t smin, smax;
    if(aconf.maxValue == 0) {
        getMinMax(info->stack.top().streams.front(), &min, &max, &smin, &smax);
    } else {
        max = getMaxOfStream(info->currConfig.sequenceTransformationId, info->currStreamIndex, aconf.maxValue);
        min = 0;
        smax = max;
        smin = -smax;
    }

    const unsigned BIPARAM = (max > 0) ? unsigned(std::floor(std::log2(max)) + 1) : 1;
    const unsigned TUPARAM = (max > 0) ? unsigned(std::min(max, uint64_t(256))) : 1;
    const std::vector<std::vector<unsigned>> candidates = {{std::min(BIPARAM, 32u)},
                                                           {std::min(TUPARAM, 32u)},
                                                           {0},
                                                           {0},
                                                           aconf.candidateBinarizationParameters,
                                                           aconf.candidateBinarizationParameters};
    auto id = info->currConfig.transformedSequenceConfigurations[info->currStreamIndex].binarizationId;

    for (const auto& transID : candidates[unsigned(id)]) {
        if (!getBinarization(id).isSigned) {
            if (!getBinarization(id).sbCheck(min, max, transID)) {
                continue;
            }
        } else {
            if (!getBinarization(id).sbCheck(uint64_t(smin), uint64_t(smax), transID)) {
                continue;
            }
        }


        info->currConfig.transformedSequenceConfigurations[info->currStreamIndex].binarizationParameters = {transID};

        getOptimumOfBinarizationParameter(
                aconf,
                info
        );
    }
}

//------------------------------------------------------------------------------

void getOptimumOfDiffTransformedStream(const AnalysisConfiguration& aconf,
                                       TraversalInfo *info
){
    std::vector<gabac::BinarizationId>
            candidates = (!info->currConfig.transformedSequenceConfigurations[info->currStreamIndex].diffCodingEnabled)
                         ? aconf.candidateUnsignedBinarizationIds
                         : aconf.candidateSignedBinarizationIds;

    for (const auto& transID : candidates) {
        info->currConfig.transformedSequenceConfigurations[info->currStreamIndex].binarizationId = transID;
        getOptimumOfBinarization(
                aconf,
                info
        );
    }
}

//------------------------------------------------------------------------------

void getOptimumOfLutTransformedStream(const AnalysisConfiguration& aconf,
                                      TraversalInfo *info
){
    for (const auto& transID : aconf.candidateDiffParameters) {
        info->currConfig.transformedSequenceConfigurations[info->currStreamIndex].diffCodingEnabled = transID;
        if (transID) {
            info->stack.push(info->stack.top());
            gabac::getTransformation(gabac::SequenceTransformationId::diff_coding).transform(
                    {0},
                    &info->stack.top().streams
            );
        }
        getOptimumOfDiffTransformedStream(aconf, info);

        if (transID) {
            info->stack.pop();
        }
    }
}

//------------------------------------------------------------------------------

void getOptimumOfLutEnabled(const AnalysisConfiguration& aconf,
                            TraversalInfo *info
){
    for (const auto& transID : aconf.candidateLutOrder) {
        info->currConfig.transformedSequenceConfigurations[info->currStreamIndex].lutOrder = transID;
        info->stack.push(info->stack.top());

        try {
            getTransformation(SequenceTransformationId::lut_transform).transform(
                    {info->currConfig.transformedSequenceConfigurations[info->currStreamIndex].lutOrder},
                    &info->stack.top().streams
            );
        }
        catch (...) {
            continue;
        }

        unsigned bits0 = 0;
        uint64_t min, max;
        int64_t smin, smax;
        if(aconf.maxValue == 0) {
            getMinMax(info->stack.top().streams.front(), &min, &max, &smin, &smax);
        } else {
            max = getMaxOfStream(info->currConfig.sequenceTransformationId, info->currStreamIndex, aconf.maxValue);
            min = 0;
            smax = max;
            smin = -smax;
        }
        bits0 = unsigned(std::ceil(std::log2(max + 1)));
        if (max <= 1) {
            bits0 = 1;
        }
        auto bits1 = unsigned(info->stack.top().streams[1].size());

        info->currConfig.transformedSequenceConfigurations[info->currStreamIndex].lutBits = bits0;

        info->numConfigs += 1;
        gabac::encode_cabac(
                gabac::BinarizationId::BI,
                {bits0},
                gabac::ContextSelectionId::bypass,
                &info->stack.top().streams[1]
        );


        info->currSequenceSize += sizeof(uint32_t) + info->stack.top().streams[1].size();

        if (info->currConfig.transformedSequenceConfigurations[info->currStreamIndex].lutOrder > 0) {
            bits1 = unsigned(std::ceil(std::log2(bits1)));

            info->numConfigs += 1;
            gabac::encode_cabac(
                    gabac::BinarizationId::BI,
                    {bits1},
                    gabac::ContextSelectionId::bypass,
                    &info->stack.top().streams[2]
            );

            info->currSequenceSize += sizeof(uint32_t) + info->stack.top().streams[2].size();
        }

        getOptimumOfLutTransformedStream(aconf, info);

        info->stack.pop();
    }
}

//------------------------------------------------------------------------------

void getOptimumOfTransformedStream(const AnalysisConfiguration& aconf,
                                   TraversalInfo *info
){
    for (const auto& transID : aconf.candidateLUTCodingParameters) {
        info->currConfig.transformedSequenceConfigurations[info->currStreamIndex].lutTransformationEnabled = transID;
        info->currSequenceSize = 0;
        if (transID) {
            getOptimumOfLutEnabled(aconf, info);
        } else {
            getOptimumOfLutTransformedStream(aconf, info);
        }
    }
}

//------------------------------------------------------------------------------

void getOptimumOfSequenceTransform(const AnalysisConfiguration& aconf,
                                   TraversalInfo *info
){
    const std::vector<uint32_t> candidateDefaultParameters = {0};
    const std::vector<uint32_t> *params[] = {&candidateDefaultParameters,
                                             &candidateDefaultParameters,
                                             &aconf.candidateMatchCodingParameters,
                                             &aconf.candidateRLECodingParameters};
    for (auto const& p : *(params[unsigned(info->currConfig.sequenceTransformationId)])) {
        info->stack.push(info->stack.top());

        info->currConfig.sequenceTransformationParameter = p;

        gabac::getTransformation(info->currConfig.sequenceTransformationId).transform(
                {p},
                &info->stack.top().streams
        );

        info->currTotalSize = 0;

        for (unsigned i = 0; i < info->stack.top().streams.size(); ++i) {
            info->stack.push(info->stack.top());
            info->stack.top().streams[i].swap(&info->stack.top().streams[0]);
            info->stack.top().streams.resize(1);

            info->ioconf->log(gabac::IOConfiguration::LogLevel::INFO) << "Stream " << i << "..." << std::endl;
            info->bestSequenceSize = std::numeric_limits<size_t>::max();
            info->currStreamIndex = i;
            getOptimumOfTransformedStream(aconf, info);
            info->currConfig.transformedSequenceConfigurations[i] = info->bestSeqConfig;
            if (info->bestSequenceSize == std::numeric_limits<size_t>::max()) {
                info->ioconf->log(gabac::IOConfiguration::LogLevel::DEBUG)
                        << "Found no valid configuration for stream "
                        << info->currStreamIndex
                        << " of transformation"
                        << unsigned(info->currConfig.sequenceTransformationId)
                        << " in word size "
                        << info->currConfig.wordSize
                        << " Skipping!"
                        << std::endl;
                info->currTotalSize = info->bestSequenceSize;
                info->stack.pop();
                break;
            }

            info->currTotalSize += info->bestSequenceSize;
            if (info->currTotalSize >= info->bestTotalSize) {
                info->ioconf->log(gabac::IOConfiguration::LogLevel::TRACE)
                        << "Skipping. Bitstream already larger than permitted." << std::endl;
                info->stack.pop();
                break;
            }
            info->stack.pop();
        }

        if (info->currTotalSize < info->bestTotalSize) {
            info->ioconf->log(gabac::IOConfiguration::LogLevel::DEBUG)
                    << "Found configuration compressing to "
                    << info->currTotalSize
                    << " bytes."
                    << std::endl;
            info->bestTotalSize = info->currTotalSize;
            info->bestConfig = info->currConfig;
        }

        info->stack.pop();
    }
}

//------------------------------------------------------------------------------

void getOptimumOfSymbolSequence(const AnalysisConfiguration& aconf,
                                TraversalInfo *info
){
    for (const auto& transID : aconf.candidateSequenceTransformationIds) {

        info->ioconf->log(gabac::IOConfiguration::LogLevel::INFO)
                << "Transformation "
                << unsigned(transID)
                << "..."
                << std::endl;
        info->currConfig.sequenceTransformationId = transID;
        info->currConfig
                .transformedSequenceConfigurations
                .resize(gabac::getTransformation(transID).wordsizes.size());
        // Core of analysis
        getOptimumOfSequenceTransform(
                aconf,
                info
        );
    }
}


void analyze(const IOConfiguration& ioconf, const AnalysisConfiguration& aconf){
    ioconf.validate();
    TraversalInfo info;
    info.ioconf = &ioconf;
    info.stack.emplace();
    info.stack.top().streams.emplace_back(ioconf.blocksize, 1);
    info.bestTotalSize = std::numeric_limits<size_t>::max();
    info.numConfigs = 0;

    if (!ioconf.blocksize) {
        gabac::StreamHandler::readFull(*ioconf.inputStream, &info.stack.top().streams.front());
    } else {
        gabac::StreamHandler::readBytes(*ioconf.inputStream, ioconf.blocksize, &info.stack.top().streams.front());
    }

    auto wordsize_cand = aconf.candidateWordsizes;
    if(aconf.wordSize != 0) {
        wordsize_cand.clear();
        wordsize_cand.push_back(aconf.wordSize);
    }

    for (const auto& w : wordsize_cand) {
        ioconf.log(gabac::IOConfiguration::LogLevel::INFO) << "Wordsize " << w << "..." << std::endl;
        if (info.stack.top().streams.front().getRawSize() % w != 0) {
            ioconf.log(gabac::IOConfiguration::LogLevel::WARNING) << "Input stream size "
                                                                  << info.stack.top().streams.front().getRawSize()
                                                                  << " is not a multiple of word size "
                                                                  << w
                                                                  << "! Skipping word size." << std::endl;
            continue;
        }

        if(w == 8  && info.stack.top().streams.front().getMaximum() > std::numeric_limits<uint32_t>::max()) {
            ioconf.log(gabac::IOConfiguration::LogLevel::WARNING) << "Too large values for wordsize 8" << std::endl;
            continue;
        }

        info.stack.top().streams.front().setWordSize((uint8_t) w);
        info.currConfig.wordSize = w;

        getOptimumOfSymbolSequence(aconf, &info);
    }

    std::string confString = info.bestConfig.toJsonString();
    DataBlock confBlock(&confString);
    gabac::StreamHandler::writeBytes(*ioconf.outputStream, &confBlock);

    // ioconf.log(gabac::IOConfiguration::LogLevel::INFO)
    //         << "Success! Best configuration will compress down to "
    //         << info.bestTotalSize
    //         << " bytes."
    //         << std::endl;

    ioconf.log(gabac::IOConfiguration::LogLevel::INFO) << "RESULT" << std::endl;
    ioconf.log(gabac::IOConfiguration::LogLevel::INFO) << "filesize:" << info.bestTotalSize << ",num_configs:" << info.numConfigs;
}

//------------------------------------------------------------------------------

}  // namespace gabac

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
