/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/entropy/gabac/mismatch-decoder.h"
#include <memory>
#include <utility>
#include <vector>
#include "genie/entropy/gabac/stream-handler.h"
#include "genie/util/make_unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

MismatchDecoder::MismatchDecoder(util::DataBlock &&d, const EncodingConfiguration &c)
    : numSubseqSymbolsTotal(0), numSubseqSymbolsDecoded(0), numTrnsfSubseqs(0) {
    const paramcabac::Subsequence &subseqCfg = c.getSubseqConfig();
    uint64_t subseqPayloadSizeUsed = 0;

    util::DataBlock data = std::move(d);
    gabac::IBufferStream inputStream(&data, 0);
    const uint64_t subseqPayloadSize = gabac::StreamHandler::readStreamSize(inputStream);
    if (subseqPayloadSize <= 0) return;  // Simple return as subseqPayloadSize can be zero.

    // Read number of symbols in descriptor subsequence
    if (subseqCfg.getTokentypeFlag()) {
        subseqPayloadSizeUsed += gabac::StreamHandler::readU7(inputStream, numSubseqSymbolsTotal);
    } else {
        subseqPayloadSizeUsed += gabac::StreamHandler::readUInt(inputStream, numSubseqSymbolsTotal, 4);
    }

    if (numSubseqSymbolsTotal > 0) {
        if (inputStream.peek() != EOF) {
            // Set up for the inverse sequence transformation
            numTrnsfSubseqs = subseqCfg.getNumTransformSubseqCfgs();
            if (numTrnsfSubseqs > 1) return;  // Note: Mismatch decoder is only allowed with 1 transformed subseq.

            // Loop through the transformed sequences
            trnsfSubseqData.resize(numTrnsfSubseqs);
            for (size_t i = 0; i < numTrnsfSubseqs; i++) {
                uint64_t currNumtrnsfSymbols = 0;
                uint64_t trnsfSubseqPayloadSizeRemain = 0;

                if (i < (numTrnsfSubseqs - 1)) {
                    subseqPayloadSizeUsed +=
                        gabac::StreamHandler::readUInt(inputStream, trnsfSubseqPayloadSizeRemain, 4);
                } else {
                    trnsfSubseqPayloadSizeRemain = subseqPayloadSize - subseqPayloadSizeUsed;
                }

                if (trnsfSubseqPayloadSizeRemain > 0) {
                    if (numTrnsfSubseqs > 1) {
                        subseqPayloadSizeUsed += gabac::StreamHandler::readUInt(inputStream, currNumtrnsfSymbols, 4);
                        trnsfSubseqPayloadSizeRemain -= 4;
                    } else {
                        currNumtrnsfSymbols = numSubseqSymbolsTotal;
                    }

                    if (currNumtrnsfSymbols > 0) {
                        subseqPayloadSizeUsed += gabac::StreamHandler::readBytes(
                            inputStream, trnsfSubseqPayloadSizeRemain, &trnsfSubseqData[i]);
                    }

                    trnsfSymbolsDecoder.emplace_back(&trnsfSubseqData[i], subseqCfg.getTransformSubseqCfg((uint8_t)i),
                                                     (unsigned int)currNumtrnsfSymbols);
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t MismatchDecoder::decodeMismatch(uint64_t ref) {
    std::vector<uint64_t> decodedTrnsfSymbols(numTrnsfSubseqs, 0);

    for (size_t i = 0; i < numTrnsfSubseqs; i++) {
        decodedTrnsfSymbols[i] =
            (trnsfSymbolsDecoder[i].symbolsAvail() > 0) ? trnsfSymbolsDecoder[i].decodeNextSymbol(&ref) : 0;
    }

    numSubseqSymbolsDecoded++;
    // Note: Mismatch decoder is only allowed with 1 transform subseq.
    return decodedTrnsfSymbols[0];
}

// ---------------------------------------------------------------------------------------------------------------------

bool MismatchDecoder::dataLeft() const { return numSubseqSymbolsDecoded < numSubseqSymbolsTotal; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t MismatchDecoder::getSubseqSymbolsTotal() const { return numSubseqSymbolsTotal; }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<core::MismatchDecoder> MismatchDecoder::copy() const {
    return util::make_unique<MismatchDecoder>(*this);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
