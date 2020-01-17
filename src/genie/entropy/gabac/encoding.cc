/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "encoding.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include <genie/util/block-stepper.h>
#include <genie/util/data-block.h>
#include "configuration.h"
#include "constants.h"
#include "encode-cabac.h"
#include "stream-handler.h"
#include "writer.h"

namespace genie {
namespace entropy {
namespace gabac {

static uint64_t getMax(const util::DataBlock &b) {
    uint64_t max = 0;
    util::BlockStepper r = b.getReader();
    while (r.isValid()) {
        max = std::max(max, r.get());
    }
    return max;
}

void doSequenceTransform(const gabac::SequenceTransformationId &transID, uint64_t param,
                         std::vector<util::DataBlock> *const transformedSequences) {
    // GABACIFY_LOG_TRACE << "Encoding sequence of length: " <<
    // (*transformedSequences)[0].size();

    // GABACIFY_LOG_DEBUG << "Performing sequence transformation " <<
    // gabac::transformationInformation[id].name;

    getTransformation(transID).transform({param}, transformedSequences);

    // GABACIFY_LOG_TRACE << "Got " << transformedSequences->size() << "
    // sequences";
    for (unsigned i = 0; i < transformedSequences->size(); ++i) {
        // GABACIFY_LOG_TRACE << i << ": " << (*transformedSequences)[i].size()
        // << " bytes";
    }
}

static void encodeStream(const gabac::TransformedSequenceConfiguration &conf,
                         util::DataBlock *const diffAndLutTransformedSequence, std::ostream *out) {
    // Encoding
    gabac::encode_cabac(conf.binarizationId, conf.binarizationParameters, conf.contextSelectionId,
                        diffAndLutTransformedSequence);

    gabac::StreamHandler::writeStream(*out, diffAndLutTransformedSequence);
}

void doLutTransform(unsigned int order, std::vector<util::DataBlock> *const lutSequences, unsigned *bits0,
                    std::ostream *out) {
    // GABACIFY_LOG_TRACE << "LUT transform *en*abled";

    // Put raw sequence in, get transformed sequence and lut tables
    getTransformation(SequenceTransformationId::lut_transform).transform({order}, lutSequences);

    // GABACIFY_LOG_DEBUG << "Got uncompressed stream after LUT: " <<
    // (*lutSequences)[0].size() << " bytes"; GABACIFY_LOG_DEBUG << "Got table0
    // after LUT: " << (*lutSequences)[1].size() << " bytes"; GABACIFY_LOG_DEBUG
    // << "Got table1 after LUT: " << (*lutSequences)[2].size() << " bytes";

    // Calculate bit size for order 0 table
    if (*bits0 == 0) {
        uint64_t max = getMax(lutSequences->at(1));
        *bits0 = unsigned(std::ceil(std::log2(max + 1)));
        if (max <= 1) {
            *bits0 = 1;
        }
    }

    auto bits1 = unsigned((*lutSequences)[1].size());
    encodeStream({false, 0, 0, false, gabac::BinarizationId::BI, {*bits0}, gabac::ContextSelectionId::bypass},
                 &(*lutSequences)[1], out);

    if (order > 0) {
        bits1 = unsigned(std::ceil(std::log2(bits1)));
        encodeStream({false, 0, 0, false, gabac::BinarizationId::BI, {bits1}, gabac::ContextSelectionId::bypass},
                     &(*lutSequences)[2], out);
    }
}

void doDiffTransform(std::vector<util::DataBlock> *const sequence) {
    // GABACIFY_LOG_TRACE << "LUT transform *en*abled";

    // Put raw sequence in, get transformed sequence and lut tables
    getTransformation(SequenceTransformationId::diff_coding).transform({0}, sequence);

    // GABACIFY_LOG_TRACE << "Diff genie *dis*abled";
    // GABACIFY_LOG_DEBUG << "Got uncompressed stream after diff: "
    // << diffAndLutTransformedSequence->size() << " bytes";
}

static void encodeSingleSequence(const gabac::TransformedSequenceConfiguration &configuration,
                                 util::DataBlock *const seq, std::ostream *out) {
    std::vector<util::DataBlock> lutTransformedSequences;

    // Symbol/transformed symbols, lut0 bytestream, lut1 bytestream
    lutTransformedSequences.resize(1);
    lutTransformedSequences[0].swap(seq);

    // Put sequence in, get lut sequence and lut bytestreams
    if (configuration.lutTransformationEnabled) {
        unsigned bits = configuration.lutBits;
        doLutTransform(configuration.lutOrder, &lutTransformedSequences, &bits, out);
    }

    // Put lut transformed in, get difftransformed out
    if (configuration.diffCodingEnabled) {
        doDiffTransform(&lutTransformedSequences);
    }

    encodeStream(configuration, &lutTransformedSequences[0], out);
}

void encode(const IOConfiguration &conf, const EncodingConfiguration &enConf) {
    conf.validate();
    util::DataBlock sequence(0, 1);
    sequence.setWordSize(static_cast<uint8_t>(enConf.wordSize));
    size_t size = 0;
    if (!conf.blocksize) {
        size = gabac::StreamHandler::readFull(*conf.inputStream, &sequence);
    } else {
        size = gabac::StreamHandler::readBlock(*conf.inputStream, conf.blocksize, &sequence);
    }
    while (size) {
        // Insert sequence into vector
        std::vector<util::DataBlock> transformedSequences;
        transformedSequences.resize(1);
        transformedSequences[0].swap(&sequence);

        // Put symbol stream in, get transformed streams out
        doSequenceTransform(enConf.sequenceTransformationId, enConf.sequenceTransformationParameter,
                            &transformedSequences);

        // Loop through the transformed sequences
        for (size_t i = 0; i < transformedSequences.size(); i++) {
            // Put transformed sequence in, get partial bytestream back
            encodeSingleSequence(enConf.transformedSequenceConfigurations[i], &(transformedSequences[i]),
                                 conf.outputStream);
        }
        if (conf.blocksize) {
            sequence.setWordSize(static_cast<uint8_t>(enConf.wordSize));
            size = gabac::StreamHandler::readBlock(*conf.inputStream, conf.blocksize, &sequence);
        } else {
            size = 0;
        }
    }
}
}  // namespace gabac
}  // namespace entropy
}  // namespace genie