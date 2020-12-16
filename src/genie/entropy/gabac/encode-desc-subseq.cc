/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "encode-desc-subseq.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include <genie/entropy/paramcabac/transformed-subseq.h>
#include <genie/util/block-stepper.h>
#include <genie/util/data-block.h>
#include "configuration.h"
#include "encode-transformed-subseq.h"
#include "stream-handler.h"
#include "writer.h"

#include "equality-subseq-transform.h"
#include "match-subseq-transform.h"
#include "merge-subseq-transform.h"
#include "rle-subseq-transform.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

static inline void doSubsequenceTransform(const paramcabac::Subsequence &subseqCfg,
                                          std::vector<util::DataBlock> *const transformedSubseqs) {
    switch (subseqCfg.getTransformParameters().getTransformIdSubseq()) {
        case paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM:
            transformedSubseqs->resize(1);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::EQUALITY_CODING:
            transformEqualityCoding(transformedSubseqs);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING:
            transformMatchCoding(subseqCfg, transformedSubseqs);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING:
            transformRleCoding(subseqCfg, transformedSubseqs);
            break;
        case paramcabac::TransformedParameters::TransformIdSubseq::MERGE_CODING:
            transformMergeCoding(subseqCfg, transformedSubseqs);
            break;
        default:
            UTILS_DIE("Invalid subseq transforamtion");
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

unsigned long encodeDescSubsequence(const IOConfiguration &conf, const EncodingConfiguration &enConf) {
    conf.validate();
    const paramcabac::Subsequence &subseqCfg = enConf.getSubseqConfig();
    util::DataBlock subsequence(0, 4);
    util::DataBlock dependency(0, 4);

    size_t numDescSubseqSymbols = 0;
    size_t subseqPayloadSize = 0;

    numDescSubseqSymbols = gabac::StreamHandler::readFull(*conf.inputStream, &subsequence);
    if (conf.dependencyStream != nullptr) {
        if (numDescSubseqSymbols != gabac::StreamHandler::readFull(*conf.dependencyStream, &dependency)) {
            UTILS_DIE("Size mismatch between dependency and descriptor subsequence");
        }
    }

    if (numDescSubseqSymbols > 0) {
        // write number of symbols in descriptor subsequence
        if (subseqCfg.getTokentypeFlag()) {
            subseqPayloadSize += gabac::StreamHandler::writeU7(*conf.outputStream, numDescSubseqSymbols);
        } else {
            subseqPayloadSize += gabac::StreamHandler::writeUInt(*conf.outputStream, numDescSubseqSymbols, 4);
        }

        // Insert subsequence into vector
        std::vector<util::DataBlock> transformedSubseqs;
        transformedSubseqs.resize(1);
        transformedSubseqs[0].swap(&subsequence);

        // Put descriptor subsequence, get transformed subsequences out
        doSubsequenceTransform(subseqCfg, &transformedSubseqs);
        const size_t numTrnsfSubseqs = transformedSubseqs.size();

        // Loop through the transformed sequences
        for (size_t i = 0; i < numTrnsfSubseqs; i++) {
            uint64_t numtrnsfSymbols = transformedSubseqs[i].size();
            uint64_t trnsfSubseqPayloadSize = 0;
            if (numtrnsfSymbols > 0) {
                // Encoding
                trnsfSubseqPayloadSize =
                    gabac::encodeTransformSubseq(subseqCfg.getTransformSubseqCfg(i), &(transformedSubseqs[i]),
                                                 (dependency.size()) ? &dependency : nullptr);
            }

            if (i < (numTrnsfSubseqs - 1)) {
                subseqPayloadSize += gabac::StreamHandler::writeUInt(*conf.outputStream, trnsfSubseqPayloadSize + 4, 4);
            }

            if (numTrnsfSubseqs > 1) {
                subseqPayloadSize += gabac::StreamHandler::writeUInt(*conf.outputStream, numtrnsfSymbols, 4);
            }

            if (trnsfSubseqPayloadSize > 0) {
                subseqPayloadSize += gabac::StreamHandler::writeBytes(*conf.outputStream, &transformedSubseqs[i]);
            }
        }
    }

    return subseqPayloadSize;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------