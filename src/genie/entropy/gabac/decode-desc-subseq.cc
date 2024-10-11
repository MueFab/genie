/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/decode-desc-subseq.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include "genie/entropy/gabac/configuration.h"
#include "genie/entropy/gabac/decode-transformed-subseq.h"
#include "genie/entropy/gabac/equality-subseq-transform.h"
#include "genie/entropy/gabac/match-subseq-transform.h"
#include "genie/entropy/gabac/merge-subseq-transform.h"
#include "genie/entropy/gabac/reader.h"
#include "genie/entropy/gabac/rle-subseq-transform.h"
#include "genie/entropy/gabac/stream-handler.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

void doInverseSubsequenceTransform(const paramcabac::Subsequence &subseqCfg,
                                   std::vector<util::DataBlock> *const transformedSubseqs) {
    switch (subseqCfg.getTransformParameters().getTransformIdSubseq()) {
        case paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM:
            transformedSubseqs->resize(1);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::EQUALITY_CODING:
            inverseTransformEqualityCoding(transformedSubseqs);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING:
            inverseTransformMatchCoding(transformedSubseqs);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING:
            inverseTransformRleCoding(subseqCfg, transformedSubseqs);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::MERGE_CODING:
            inverseTransformMergeCoding(subseqCfg, transformedSubseqs);
            break;
        default:
            UTILS_DIE("Invalid subseq transforamtion");
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t decodeDescSubsequence(const IOConfiguration &ioConf, const EncodingConfiguration &enConf) {
    const paramcabac::Subsequence &subseqCfg = enConf.getSubseqConfig();
    util::DataBlock dependency(0, 4);

    uint64_t subseqPayloadSizeUsed = 0;
    uint64_t numDescSubseqSymbols = 0;
    const uint64_t subseqPayloadSize = gabac::StreamHandler::readStreamSize(*ioConf.inputStream);

    if (subseqPayloadSize <= 0) return 0;

    // read number of symbols in descriptor subsequence
    if (subseqCfg.getTokentypeFlag()) {
        subseqPayloadSizeUsed += gabac::StreamHandler::readU7(*ioConf.inputStream, numDescSubseqSymbols);
    } else {
        subseqPayloadSizeUsed += gabac::StreamHandler::readUInt(*ioConf.inputStream, numDescSubseqSymbols, 4);
    }

    if (numDescSubseqSymbols > 0) {
        if (ioConf.dependencyStream != nullptr) {
            if (numDescSubseqSymbols != gabac::StreamHandler::readFull(*ioConf.dependencyStream, &dependency)) {
                UTILS_DIE("Size mismatch between dependency and descriptor subsequence");
            }
        }

        if (ioConf.inputStream->peek() != EOF) {
            // Set up for the inverse sequence transformation
            size_t numTrnsfSubseqsCfgs = subseqCfg.getNumTransformSubseqCfgs();

            // Loop through the transformed sequences
            std::vector<util::DataBlock> transformedSubseqs(numTrnsfSubseqsCfgs);
            for (size_t i = 0; i < numTrnsfSubseqsCfgs; i++) {
                uint8_t wordsize = i == numTrnsfSubseqsCfgs - 1 ? ioConf.outputWordsize : 1;
                transformedSubseqs[i].setWordSize(wordsize);
            }
            for (size_t i = 0; i < numTrnsfSubseqsCfgs; i++) {
                util::DataBlock decodedTransformedSubseq;
                uint64_t numtrnsfSymbols = 0;
                uint64_t trnsfSubseqPayloadSizeRemain = 0;

                if (i < (numTrnsfSubseqsCfgs - 1)) {
                    subseqPayloadSizeUsed +=
                        gabac::StreamHandler::readUInt(*ioConf.inputStream, trnsfSubseqPayloadSizeRemain, 4);
                } else {
                    trnsfSubseqPayloadSizeRemain = subseqPayloadSize - subseqPayloadSizeUsed;
                }

                if (trnsfSubseqPayloadSizeRemain > 0) {
                    if (numTrnsfSubseqsCfgs > 1) {
                        subseqPayloadSizeUsed +=
                            gabac::StreamHandler::readUInt(*ioConf.inputStream, numtrnsfSymbols, 4);
                        trnsfSubseqPayloadSizeRemain -= 4;
                    } else {
                        numtrnsfSymbols = numDescSubseqSymbols;
                    }

                    if (numtrnsfSymbols <= 0) continue;

                    gabac::StreamHandler::readBytes(*ioConf.inputStream, trnsfSubseqPayloadSizeRemain,
                                                    &decodedTransformedSubseq);

                    uint8_t wordsize = i == numTrnsfSubseqsCfgs - 1 ? ioConf.outputWordsize : 1;
                    // Decoding
                    subseqPayloadSizeUsed += (uint64_t)gabac::decodeTransformSubseq(
                        subseqCfg.getTransformSubseqCfg(static_cast<uint8_t>(i)),
                        static_cast<unsigned int>(numtrnsfSymbols), &decodedTransformedSubseq, wordsize,
                        (!dependency.empty()) ? &dependency : nullptr);
                    transformedSubseqs[i].swap(&(decodedTransformedSubseq));
                }
            }

            doInverseSubsequenceTransform(subseqCfg, &transformedSubseqs);

            gabac::StreamHandler::writeBytes(*ioConf.outputStream, &transformedSubseqs[0]);
        }
    }

    return (uint64_t)subseqPayloadSizeUsed;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
