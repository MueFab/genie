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
#include "gabac.h"
#include <genie/entropy/paramcabac/transformed-seq.h>

namespace genie {
namespace entropy {
namespace gabac {


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


static void encodeSingleSequence(const paramcabac::TransformedSeq &cfg,
                                 util::DataBlock *const seq,
                                 std::ostream *out,
                                 util::DataBlock *const dep = nullptr) {
    size_t numSymbols = seq->size();
    if(numSymbols <= 0) return;

    // Encoding
    gabac::encode_cabac(cfg, seq, dep);

    printf("compressed size: %lu\n", seq->getRawSize());
    gabac::StreamHandler::writeStream(*out, seq);
}*/

void encode(const IOConfiguration &conf, const EncodingConfiguration &enConf) {
    conf.validate();
    util::DataBlock sequence(0, 4);
    util::DataBlock dependency(0, 4);
    size_t size = 0;
    if (!conf.blocksize) {
        size = gabac::StreamHandler::readFull(*conf.inputStream, &sequence);
        if(conf.dependencyStream != nullptr) {
            if(size != gabac::StreamHandler::readFull(*conf.dependencyStream, &dependency)) {
                GABAC_DIE("Size mismatch between dependency and descriptor subsequence");
            }
        }
    } else {
        size = gabac::StreamHandler::readBlock(*conf.inputStream, conf.blocksize, &sequence);
        if(conf.dependencyStream != nullptr) {
            if(size != gabac::StreamHandler::readBlock(*conf.dependencyStream, conf.blocksize, &dependency)) {
                GABAC_DIE("Size mismatch between dependency and descriptor subsequence");
            }
        }
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
            size_t numSymbols = transformedSubseqs[i].size();
            if(numSymbols <= 0) return;

            // Encoding
            gabac::encode_cabac(enConf.subseq.getTransformSubseqCfg(i),
                                &(transformedSubseqs[i]),
                                (dependency.size()) ? &dependency : nullptr);

            printf("compressed size: %lu\n", transformedSubseqs[i].getRawSize());
            gabac::StreamHandler::writeStream(*conf.outputStream, &transformedSubseqs[i], numSymbols);
        }
        if (conf.blocksize) {
            // RESTRUCT_DISABLE sequence.setWordSize(static_cast<uint8_t>(enConf.wordSize));
            size = gabac::StreamHandler::readBlock(*conf.inputStream, conf.blocksize, &sequence);
            if(conf.dependencyStream != nullptr) {
                if(size != gabac::StreamHandler::readBlock(*conf.dependencyStream, conf.blocksize, &dependency)) {
                    GABAC_DIE("Size mismatch between dependency and descriptor subsequence");
                }
            }
        } else {
            size = 0;
        }
    }
}
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
