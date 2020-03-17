/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "decoding.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include <genie/util/data-block.h>
#include "configuration.h"
#include "constants.h"
#include "decode-cabac.h"
#include "reader.h"
#include "stream-handler.h"

namespace genie {
namespace entropy {
namespace gabac {

/* RESTRUCT_DISABLE
static void decodeInverseLUT(unsigned bits0, unsigned order, std::istream *inStream, util::DataBlock *const inverseLut,
                             util::DataBlock *const inverseLut1) {
    // RESTRUCT_TODO
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

static void doDiffCoding(bool enabled, util::DataBlock *const lutTransformedSubseq) {
    // Diff genie
    if (enabled) {
        // GABACIFY_LOG_TRACE << "Diff genie *en*abled";
        std::vector<util::DataBlock> vec(1);
        vec[0].swap(lutTransformedSubseq);
        gabac::getTransformation(gabac::SequenceTransformationId::diff_coding).inverseTransform({}, &vec);
        vec[0].swap(lutTransformedSubseq);
        return;
    }

    // GABACIFY_LOG_TRACE << "Diff genie *dis*abled";
}

static void doLUTCoding(bool enabled, unsigned order, std::vector<util::DataBlock> *const lutSequences) {
    if (enabled) {
        // GABACIFY_LOG_TRACE << "LUT transform *en*abled";

        // Do the inverse LUT transform
        gabac::getTransformation(gabac::SequenceTransformationId::lut_transform)
            .inverseTransform({order}, lutSequences);
        return;
    }

    // GABACIFY_LOG_TRACE << "LUT transform *dis*abled";
} */

static void doEntropyCoding(const paramcabac::TransformedSeq &transformedSubseqCfg,
                            std::istream *inStream,
                            util::DataBlock *const diffAndLutTransformedSubseq) {
    size_t streamSize = StreamHandler::readStream(*inStream, diffAndLutTransformedSubseq);
    if (streamSize <= 0) return;
    // GABACIFY_LOG_TRACE << "Bitstream size: " <<
    // diffAndLutTransformedSequence->size();

    // Decoding
    gabac::decode_cabac(transformedSubseqCfg, diffAndLutTransformedSubseq);
}

void decode(const IOConfiguration &ioConf, const EncodingConfiguration &enConf) {
    while (ioConf.inputStream->peek() != EOF) {
        // Set up for the inverse sequence transformation
        size_t numTransformedSubSeqCfgs = enConf.subseq.getNumTransformSubseqCfgs();

        // Loop through the transformed sequences
        std::vector<util::DataBlock> transformedSubseqs;
        for (size_t i = 0; i < numTransformedSubSeqCfgs; i++) {
            // GABACIFY_LOG_TRACE << "Processing transformed sequence: " << i;
            auto transformedSubseqCfg = enConf.subseq.getTransformSubseqCfg(i);

            std::vector<util::DataBlock> lutTransformedSubseq(3);
            const paramcabac::SupportValues::TransformIdSubsym subSymTransform = transformedSubseqCfg.getTransformID();
            if (subSymTransform == paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM) {
                /* RESTRUCT_DISABLE
                decodeInverseLUT(enConf.transformedSequenceConfigurations[i].lutBits,
                                 enConf.transformedSequenceConfigurations[i].lutOrder, ioConf.inputStream,
                                 &lutTransformedSequences[1], &lutTransformedSequences[2]);
                */
            }

            doEntropyCoding(transformedSubseqCfg, ioConf.inputStream,
                            &lutTransformedSubseq[0]);

            /* RESTRUCT_DISABLE
            if (subSymTransform == paramcabac::SupportValues::TransformIdSubsym::DIFF_CODING) {
                doDiffCoding(subSymTransform == paramcabac::SupportValues::TransformIdSubsym::DIFF_CODING, &lutTransformedSubseq[0]);
            } else if (subSymTransform == paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM) {
                doLUTCoding(subSymTransform == paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM,
                            transformedSubseqCfg.getSupportValues().getCodingOrder(),
                            &lutTransformedSubseq);
            } */

            transformedSubseqs.emplace_back();
            transformedSubseqs.back().swap(&(lutTransformedSubseq[0]));
        }

        /* RESTRUCT_DISABLE
        getTransformation(enConf.subseq.getTransformParameters().getTransformIdSubseq())
            .inverseTransform({enConf.subseq.getTransformParameters()}, &transformedSubseqs);*/
        // GABACIFY_LOG_TRACE << "Decoded sequence of length: " <<
        // transformedSequences[0].size();

        gabac::StreamHandler::writeBytes(*ioConf.outputStream, &transformedSubseqs[0]);
    }
}

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
