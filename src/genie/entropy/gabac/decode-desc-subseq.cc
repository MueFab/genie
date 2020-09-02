/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "decode-desc-subseq.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include <genie/util/data-block.h>
#include "configuration.h"
#include "decode-transformed-subseq.h"
#include "reader.h"
#include "stream-handler.h"

#include "equality-subseq-transform.h"
#include "match-subseq-transform.h"
#include "merge-subseq-transform.h"
#include "rle-subseq-transform.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

static inline void doInverseSubsequenceTransform(const paramcabac::Subsequence &subseqCfg,
                                                 std::vector<util::DataBlock> *const transformedSubseqs) {
    // GABACIFY_LOG_TRACE << "Encoding sequence of length: " <<
    // (*transformedSequences)[0].size();

    // GABACIFY_LOG_DEBUG << "Performing sequence transformation " <<
    // gabac::transformationInformation[id].name;

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

    // GABACIFY_LOG_TRACE << "Got " << transformedSequences->size() << "
    // sequences";
    // for (unsigned i = 0; i < transformedSubseqs->size(); ++i) {
    // GABACIFY_LOG_TRACE << i << ": " << (*transformedSequences)[i].size()
    // << " bytes";
    // }
}

// ---------------------------------------------------------------------------------------------------------------------

unsigned long decodeDescSubsequence(const IOConfiguration &ioConf, const EncodingConfiguration &enConf) {
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
                // GABACIFY_LOG_TRACE << "Processing transformed sequence: " << i;

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

                    // Decoding
                    subseqPayloadSizeUsed += gabac::decodeTransformSubseq(subseqCfg.getTransformSubseqCfg(i),
                                                                          numtrnsfSymbols, &decodedTransformedSubseq,
                                                                          (dependency.size()) ? &dependency : nullptr);
                    transformedSubseqs[i].swap(&(decodedTransformedSubseq));
                }
            }

            doInverseSubsequenceTransform(subseqCfg, &transformedSubseqs);
            // GABACIFY_LOG_TRACE << "Decoded sequence of length: " << transformedSubseqs[0].size();

            gabac::StreamHandler::writeBytes(*ioConf.outputStream, &transformedSubseqs[0]);
        }
    }

    return subseqPayloadSizeUsed;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------