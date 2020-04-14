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
#include "exceptions.h"

#include "equality-subseq-transform.h"
#include "match-subseq-transform.h"
#include "rle-subseq-transform.h"
#include "merge-subseq-transform.h"

namespace genie {
namespace entropy {
namespace gabac {

static inline
void doInverseSubsequenceTransform(const paramcabac::Subsequence &subseqCfg,
                                   std::vector<util::DataBlock> *const transformedSubseqs) {
    // GABACIFY_LOG_TRACE << "Encoding sequence of length: " <<
    // (*transformedSequences)[0].size();

    // GABACIFY_LOG_DEBUG << "Performing sequence transformation " <<
    // gabac::transformationInformation[id].name;

    const paramcabac::TransformedParameters& trnsfSubseqParams = subseqCfg.getTransformParameters();
    const uint16_t param = trnsfSubseqParams.getParam(); // get first param

    switch(trnsfSubseqParams.getTransformIdSubseq()) {
        case paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM:
            transformedSubseqs->resize(1);
        break;
        case paramcabac::TransformedParameters::TransformIdSubseq::EQUALITY_CODING:
            inverseTransformEqualityCoding(&(*transformedSubseqs)[0], &(*transformedSubseqs)[1]);
            transformedSubseqs->resize(1);
        break;
        case paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING:
            inverseTransformMatchCoding(&(*transformedSubseqs)[0], &(*transformedSubseqs)[1],
                                               &(*transformedSubseqs)[2]);
            transformedSubseqs->resize(1);
        break;
        case paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING:
            inverseTransformRleCoding(param, &(*transformedSubseqs)[0], &(*transformedSubseqs)[1]);
            transformedSubseqs->resize(1);
        break;
        case paramcabac::TransformedParameters::TransformIdSubseq::MERGE_CODING:
            inverseTransformMergeCoding(subseqCfg, transformedSubseqs);
        break;
        default:
            GABAC_DIE("Invalid subseq transforamtion");
        break;
    }

    // GABACIFY_LOG_TRACE << "Got " << transformedSequences->size() << "
    // sequences";
    for (unsigned i = 0; i < transformedSubseqs->size(); ++i) {
        // GABACIFY_LOG_TRACE << i << ": " << (*transformedSequences)[i].size()
        // << " bytes";
    }
}

static inline
void decodeSingleSequence(const paramcabac::TransformedSubSeq &transformedSubseqCfg,
                          std::istream *inStream,
                          util::DataBlock *const decodedTransformedSubseq,
                          util::DataBlock *const dependencySubseq = nullptr) {
    size_t numEncodedSymbols = 0;
    size_t streamSize = StreamHandler::readStream(*inStream, decodedTransformedSubseq, numEncodedSymbols);
    if (streamSize <= 0 || numEncodedSymbols <= 0) return;

    // Decoding
    gabac::decode_cabac(transformedSubseqCfg, numEncodedSymbols, decodedTransformedSubseq, dependencySubseq);
}

void decode(const IOConfiguration &ioConf, const EncodingConfiguration &enConf) {
    util::DataBlock dependency(0, 4);
    size_t size = 0;
    if(ioConf.dependencyStream != nullptr) {
        if (!ioConf.blocksize) {
            if(size == gabac::StreamHandler::readFull(*ioConf.dependencyStream, &dependency)) {
                GABAC_DIE("Dependency file is empty");
            }
        } else {
            if(size == gabac::StreamHandler::readBlock(*ioConf.dependencyStream, ioConf.blocksize, &dependency)) {
                GABAC_DIE("Dependency file is empty");
            }
        } // FIXME the support for blockSize might need to be removed. TBD. For the moment read whole file.
    }

    while (ioConf.inputStream->peek() != EOF) {
        // Set up for the inverse sequence transformation
        size_t numTransformedSubSeqCfgs = enConf.subseq.getNumTransformSubseqCfgs();

        // Loop through the transformed sequences
        std::vector<util::DataBlock> transformedSubseqs;
        for (size_t i = 0; i < numTransformedSubSeqCfgs; i++) {
            // GABACIFY_LOG_TRACE << "Processing transformed sequence: " << i;
            auto transformedSubseqCfg = enConf.subseq.getTransformSubseqCfg(i);

            util::DataBlock decodedTransformedSubseq;

            decodeSingleSequence(transformedSubseqCfg,
                                 ioConf.inputStream,
                                 &decodedTransformedSubseq,
                                 (dependency.size()) ? &dependency : nullptr);

            transformedSubseqs.emplace_back();
            transformedSubseqs.back().swap(&(decodedTransformedSubseq));
        }

        doInverseSubsequenceTransform(enConf.getSubseqConfig(), &transformedSubseqs);
        // GABACIFY_LOG_TRACE << "Decoded sequence of length: " << transformedSubseqs[0].size();

        gabac::StreamHandler::writeBytes(*ioConf.outputStream, &transformedSubseqs[0]);
    }
}

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
