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
#include <genie/entropy/paramcabac/transformed-seq.h>

namespace genie {
namespace entropy {
namespace gabac {

/* RESTRUCT_DISABLE
static uint64_t getMax(const util::DataBlock &b) {
    uint64_t max = 0;
    util::BlockStepper r = b.getReader();
    while (r.isValid()) {
        max = std::max(max, r.get());
    }
    return max;
}

void doLutTransform(unsigned int order, std::vector<util::DataBlock> *const lutSequences, unsigned *bits0,
                    std::ostream *out) {
    // RESTRUCT_TODO
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
*/

/* RESTRUCT_DISABLE
void doSequenceTransform(const gabac::SequenceTransformationId &transID, uint64_t param,
                         std::vector<util::DataBlock> *const transformedSubSeqs) {
    // GABACIFY_LOG_TRACE << "Encoding sequence of length: " <<
    // (*transformedSequences)[0].size();

    // GABACIFY_LOG_DEBUG << "Performing sequence transformation " <<
    // gabac::transformationInformation[id].name;

    getTransformation(transID).transform({param}, transformedSubSeqs);

    // GABACIFY_LOG_TRACE << "Got " << transformedSequences->size() << "
    // sequences";
    for (unsigned i = 0; i < transformedSubSeqs->size(); ++i) {
        // GABACIFY_LOG_TRACE << i << ": " << (*transformedSequences)[i].size()
        // << " bytes";
    }
}
*/

static void encodeStream(const paramcabac::TransformedSeq &conf,
                         util::DataBlock *const diffAndLutTransformedSequence, std::ostream *out) {
    // Encoding
    gabac::encode_cabac(conf, diffAndLutTransformedSequence);

    gabac::StreamHandler::writeStream(*out, diffAndLutTransformedSequence);
}

static void encodeSingleSequence(const paramcabac::TransformedSeq &cfg,
                                 util::DataBlock *const seq,
                                 std::ostream *out) {
    std::vector<util::DataBlock> lutTransformedSubseqs;

    // Symbol/transformed symbols, lut0 bytestream, lut1 bytestream
    lutTransformedSubseqs.resize(1);
    lutTransformedSubseqs[0].swap(seq);

    // Put sequence in, get lut sequence and lut bytestreams
    /* RESTRUCT-DISABLE
    const paramcabac::SupportValues::TransformIdSubsym subSymTransform = cfg.getTransformID();
    if (subSymTransform == paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM) {
        //unsigned bits = cfg.lutBits; // TODO
        doLutTransform(cfg.getSupportValues().getCodingOrder(),
                       &lutTransformedSubseqs, &bits, out);
    } else if (subSymTransform == paramcabac::SupportValues::TransformIdSubsym::DIFF_CODING) {
        doDiffTransform(&lutTransformedSubseqs);
    }*/

    encodeStream(cfg, &lutTransformedSubseqs[0], out);
}

void encode(const IOConfiguration &conf, const EncodingConfiguration &enConf) {
    conf.validate();
    util::DataBlock sequence(0, 1);
    size_t size = 0;
    if (!conf.blocksize) {
        size = gabac::StreamHandler::readFull(*conf.inputStream, &sequence);
    } else {
        size = gabac::StreamHandler::readBlock(*conf.inputStream, conf.blocksize, &sequence);
    }
    while (size) {
        // Insert sequence into vector
        std::vector<util::DataBlock> transformedSubseqs;
        transformedSubseqs.resize(1);
        transformedSubseqs[0].swap(&sequence);

        /* RESTRUCT_DISABLE
        // Put symbol stream in, get transformed streams out
        doSequenceTransform(enConf.subseq.getTransformParameters().getTransformIdSubseq(),
                            enConf.subseq.getTransformParameters().getParam(),
                            &transformedSubseqs);*/

        // Loop through the transformed sequences
        for (size_t i = 0; i < transformedSubseqs.size(); i++) {
            // Put transformed sequence in, get partial bytestream back
            encodeSingleSequence(enConf.subseq.getTransformSubseqCfg(i),
                                 &(transformedSubseqs[i]),
                                 conf.outputStream);
        }
        if (conf.blocksize) {
            // RESTRUCT_DISABLE sequence.setWordSize(static_cast<uint8_t>(enConf.wordSize));
            size = gabac::StreamHandler::readBlock(*conf.inputStream, conf.blocksize, &sequence);
        } else {
            size = 0;
        }
    }
}
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
