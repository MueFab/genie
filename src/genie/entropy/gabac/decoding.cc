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
#include "gabac.h"

namespace genie {
namespace entropy {
namespace gabac {

static void decodeSingleSequence(const paramcabac::TransformedSeq &transformedSubseqCfg,
                                 std::istream *inStream,
                                 util::DataBlock *const decodedTransformedSubseq,
                                 util::DataBlock *const dependencySubseq = nullptr) {
    size_t streamSize = StreamHandler::readStream(*inStream, decodedTransformedSubseq);
    if (streamSize <= 0) return;

    // Decoding
    gabac::decode_cabac(transformedSubseqCfg, decodedTransformedSubseq, dependencySubseq);
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
