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
#include <genie/entropy/paramcabac/transformed-subseq.h>
#include "configuration.h"
#include "constants.h"
#include "encode-cabac.h"
#include "stream-handler.h"
#include "writer.h"
#include "exceptions.h"

#include "equality-subseq-transform.h"
#include "match-subseq-transform.h"
#include "rle-subseq-transform.h"
#include "merge-subseq-transform.h"

namespace genie {
namespace entropy {
namespace gabac {

static inline
void doSubsequenceTransform(const paramcabac::Subsequence &subseqCfg,
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
            transformedSubseqs->resize(2);
            (*transformedSubseqs)[1] = util::DataBlock(0, 1); // FIXME shouldn't this wsize=1 be based on outputSymbolSize?
            transformEqualityCoding(&(*transformedSubseqs)[0], &(*transformedSubseqs)[1]);
        break;
        case paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING:
            transformedSubseqs->resize(3);
            assert(param <= std::numeric_limits<uint32_t>::max());
            (*transformedSubseqs)[1] = util::DataBlock(0, 4); // FIXME shouldn't this wsize=4 be based on outputSymbolSize?
            (*transformedSubseqs)[2] = util::DataBlock(0, 4); // FIXME shouldn't this wsize=4 be based on outputSymbolSize?
            transformMatchCoding(static_cast<uint32_t>(param), &(*transformedSubseqs)[0],
                                 &(*transformedSubseqs)[1], &(*transformedSubseqs)[2]);
        break;
        case paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING:
            transformedSubseqs->resize(2);
            (*transformedSubseqs)[1] = util::DataBlock(0, 1); // FIXME shouldn't this wsize=1 be based on outputSymbolSize?
            transformRleCoding(param, &(*transformedSubseqs)[0], &(*transformedSubseqs)[1]);
        break;
        case paramcabac::TransformedParameters::TransformIdSubseq::MERGE_CODING:
            transformMergeCoding(subseqCfg, transformedSubseqs);
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

/*
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

        // Put symbol stream in, get transformed streams out
        doSubsequenceTransform(enConf.getSubseqConfig(), &transformedSubseqs);

        // Loop through the transformed sequences
        for (size_t i = 0; i < transformedSubseqs.size(); i++) {
            size_t numSymbols = transformedSubseqs[i].size();
            if(numSymbols <= 0) return;

            // Encoding
            gabac::encode_cabac(enConf.subseq.getTransformSubseqCfg(i),
                                &(transformedSubseqs[i]),
                                (dependency.size()) ? &dependency : nullptr);

            printf("compressed size: %lu\n", transformedSubseqs[i].getRawSize()+4);
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
