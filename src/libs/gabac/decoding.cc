/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#include "decoding.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include "configuration.h"
#include "constants.h"
#include "data-block.h"
#include "decode-cabac.h"
#include "reader.h"
#include "stream-handler.h"

namespace gabac {

static void decodeInverseLUT(unsigned bits0, unsigned order, std::istream *inStream, gabac::DataBlock *const inverseLut,
                             gabac::DataBlock *const inverseLut1) {
    size_t streamSize = StreamHandler::readStream(*inStream, inverseLut);
    if (streamSize <= 0) return;

    size_t lutWordSize = 0;
    if (bits0 <= 8) {
        lutWordSize = 1;
    } else if (bits0 <= 16) {
        lutWordSize = 2;
    } else if (bits0 <= 32) {
        lutWordSize = 4;
    } else if (bits0 <= 64) {
        lutWordSize = 8;
    }

    gabac::decode_cabac(gabac::BinarizationId::BI, {bits0}, gabac::ContextSelectionId::bypass,
                        static_cast<uint8_t>(lutWordSize), inverseLut);

    if (order > 0) {
        streamSize = StreamHandler::readStream(*inStream, inverseLut1);
        if (streamSize <= 0) return;

        auto bits1 = unsigned(inverseLut->size());

        bits1 = unsigned(std::ceil(std::log2(bits1)));

        size_t lut1WordSize = 0;
        if (bits1 <= 8) {
            lut1WordSize = 1;
        } else if (bits1 <= 16) {
            lut1WordSize = 2;
        } else if (bits1 <= 32) {
            lut1WordSize = 4;
        } else if (bits1 <= 64) {
            lut1WordSize = 8;
        }

        gabac::decode_cabac(gabac::BinarizationId::BI, {bits1}, gabac::ContextSelectionId::bypass,
                            static_cast<uint8_t>(lut1WordSize), inverseLut1);
    }
}

static void doDiffCoding(bool enabled, gabac::DataBlock *const lutTransformedSequence) {
    // Diff libs
    if (enabled) {
        // GABACIFY_LOG_TRACE << "Diff libs *en*abled";
        std::vector<gabac::DataBlock> vec(1);
        vec[0].swap(lutTransformedSequence);
        gabac::getTransformation(gabac::SequenceTransformationId::diff_coding).inverseTransform({}, &vec);
        vec[0].swap(lutTransformedSequence);
        return;
    }

    // GABACIFY_LOG_TRACE << "Diff libs *dis*abled";
}

static void doLUTCoding(bool enabled, unsigned order, std::vector<gabac::DataBlock> *const lutSequences) {
    if (enabled) {
        // GABACIFY_LOG_TRACE << "LUT transform *en*abled";

        // Do the inverse LUT transform
        gabac::getTransformation(gabac::SequenceTransformationId::lut_transform)
            .inverseTransform({order}, lutSequences);
        return;
    }

    // GABACIFY_LOG_TRACE << "LUT transform *dis*abled";
}

static void doEntropyCoding(const gabac::TransformedSequenceConfiguration &transformedSequenceConfiguration,
                            uint8_t wordsize, std::istream *inStream,
                            gabac::DataBlock *const diffAndLutTransformedSequence) {
    size_t streamSize = StreamHandler::readStream(*inStream, diffAndLutTransformedSequence);
    if (streamSize <= 0) return;
    // GABACIFY_LOG_TRACE << "Bitstream size: " <<
    // diffAndLutTransformedSequence->size();

    // Decoding
    gabac::decode_cabac(transformedSequenceConfiguration.binarizationId,
                        transformedSequenceConfiguration.binarizationParameters,
                        transformedSequenceConfiguration.contextSelectionId, wordsize, diffAndLutTransformedSequence);
}

void decode(const IOConfiguration &ioConf, const EncodingConfiguration &enConf) {
    while (ioConf.inputStream->peek() != EOF) {
        // Set up for the inverse sequence transformation
        size_t numTransformedSequences = gabac::getTransformation(enConf.sequenceTransformationId).wordsizes.size();

        // Loop through the transformed sequences
        std::vector<gabac::DataBlock> transformedSequences;
        for (size_t i = 0; i < numTransformedSequences; i++) {
            // GABACIFY_LOG_TRACE << "Processing transformed sequence: " << i;
            auto transformedSequenceConfiguration = enConf.transformedSequenceConfigurations.at(i);

            std::vector<gabac::DataBlock> lutTransformedSequences(3);
            if (transformedSequenceConfiguration.lutTransformationEnabled) {
                decodeInverseLUT(enConf.transformedSequenceConfigurations[i].lutBits,
                                 enConf.transformedSequenceConfigurations[i].lutOrder, ioConf.inputStream,
                                 &lutTransformedSequences[1], &lutTransformedSequences[2]);
            }

            uint8_t wsize = getTransformation(enConf.sequenceTransformationId).wordsizes[i];
            if (wsize == 0) {
                wsize = static_cast<uint8_t>(enConf.wordSize);
            }

            doEntropyCoding(enConf.transformedSequenceConfigurations[i], wsize, ioConf.inputStream,
                            &lutTransformedSequences[0]);

            doDiffCoding(enConf.transformedSequenceConfigurations[i].diffCodingEnabled, &lutTransformedSequences[0]);

            doLUTCoding(enConf.transformedSequenceConfigurations[i].lutTransformationEnabled,
                        enConf.transformedSequenceConfigurations[i].lutOrder, &lutTransformedSequences);

            transformedSequences.emplace_back();
            transformedSequences.back().swap(&(lutTransformedSequences[0]));
        }

        getTransformation(enConf.sequenceTransformationId)
            .inverseTransform({enConf.sequenceTransformationParameter}, &transformedSequences);
        // GABACIFY_LOG_TRACE << "Decoded sequence of length: " <<
        // transformedSequences[0].size();

        gabac::StreamHandler::writeBytes(*ioConf.outputStream, &transformedSequences[0]);
    }
}

}  // namespace gabac
