/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/entropy/gabac/config-manual.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>
#include "genie/entropy/paramcabac/subsequence.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

#define MAX_NUM_TRANSPORT_SEQS 4  // put max 4 for now.

// ---------------------------------------------------------------------------------------------------------------------

uint8_t getCmaxTU(uint64_t const numAlphaSpecial, uint8_t const codingSubsymSize) {
    if (numAlphaSpecial > 0) { /* special case */
        return (uint8_t)(numAlphaSpecial - 1);
    } else {
        return (uint8_t)(paramcabac::StateVars::get2PowN(codingSubsymSize) - 1);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

paramcabac::Subsequence getEncoderConfigManual(const core::GenSubIndex sub) {
    const core::GenDesc genieDescID = (core::GenDesc)sub.first;
    const core::GenSubIndex genieSubseqID = sub;
    const uint8_t subseqID = (uint8_t)sub.second;

    // default values
    const core::AlphabetID alphaID = core::AlphabetID::ACGTN;
    const uint64_t numAlphaSpecial = paramcabac::StateVars::getNumAlphaSpecial(genieSubseqID, alphaID);
    const uint8_t symSize = (alphaID == core::AlphabetID::ACGTN) ? 3 : 5;

    paramcabac::TransformedParameters::TransformIdSubseq trnsfSubseqID =
        paramcabac::TransformedParameters::TransformIdSubseq::NO_TRANSFORM;
    uint16_t trnsfSubseqParam = 0;
    std::vector<uint8_t> mergeCodingShiftSizes(MAX_NUM_TRANSPORT_SEQS, 0);
    bool tokenTypeFlag = false;

    uint8_t codingSubSymSize[MAX_NUM_TRANSPORT_SEQS] = {8};
    uint8_t outputSymbolSize[MAX_NUM_TRANSPORT_SEQS] = {8};
    uint8_t codingOrder[MAX_NUM_TRANSPORT_SEQS] = {0};
    paramcabac::SupportValues::TransformIdSubsym trnsfSubsymID[MAX_NUM_TRANSPORT_SEQS] = {
        paramcabac::SupportValues::TransformIdSubsym::NO_TRANSFORM};
    paramcabac::BinarizationParameters::BinarizationId binID[MAX_NUM_TRANSPORT_SEQS] = {
        paramcabac::BinarizationParameters::BinarizationId::BI};
    std::vector<std::vector<uint8_t>> binParams(MAX_NUM_TRANSPORT_SEQS, std::vector<uint8_t>(2, 0));
    bool bypassFlag[MAX_NUM_TRANSPORT_SEQS] = {false};
    bool adaptiveModeFlag[MAX_NUM_TRANSPORT_SEQS] = {true};
    bool shareSubsymPrvFlag[MAX_NUM_TRANSPORT_SEQS] = {true};
    bool shareSubsymLutFlag[MAX_NUM_TRANSPORT_SEQS] = {true};
    bool shareSubsymCtxFlag[MAX_NUM_TRANSPORT_SEQS] = {false};

    bool found = true;

    // populate config below
    switch (genieDescID) {
        case core::GenDesc::POS:
            switch (subseqID) {
                case 0:  // POS for first alignment
                    outputSymbolSize[0] = 32;
                    codingSubSymSize[0] = 32;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::SUTU;
                    binParams[0][0] = 4;  // splitUnitSize;
                    break;
                case 1:  // POS for other alignments
                    outputSymbolSize[0] = 32;
                    codingSubSymSize[0] = 32;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::SSUTU;
                    binParams[0][0] = 4;  // splitUnitSize;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::RCOMP:
            switch (subseqID) {
                case 0:
                    outputSymbolSize[0] = 2;
                    codingSubSymSize[0] = 2;
                    codingOrder[0] = 1;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
                    binParams[0][0] = getCmaxTU(numAlphaSpecial, codingSubSymSize[0]);  // cmax;

                    trnsfSubsymID[0] = paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::FLAGS:
            switch (subseqID) {
                case 0:
                case 1:
                case 2:
                    outputSymbolSize[0] = 1;
                    codingSubSymSize[0] = 1;
                    codingOrder[0] = 2;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::BI;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::MMPOS:
            switch (subseqID) {
                case 0:  // mmposFlag
                    outputSymbolSize[0] = 1;
                    codingSubSymSize[0] = 1;
                    codingOrder[0] = 2;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::BI;
                    break;
                case 1:  // mmposPos
                    outputSymbolSize[0] = 16;
                    codingSubSymSize[0] = 4;
                    codingOrder[0] = 1;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
                    binParams[0][0] = getCmaxTU(numAlphaSpecial, codingSubSymSize[0]);  // cmax;

                    trnsfSubsymID[0] = paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::MMTYPE:
            switch (subseqID) {
                case 0:  // mmtype
                    outputSymbolSize[0] = 2;
                    codingSubSymSize[0] = 2;
                    codingOrder[0] = 1;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
                    binParams[0][0] = getCmaxTU(numAlphaSpecial, codingSubSymSize[0]);  // cmax;

                    trnsfSubsymID[0] = paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                    break;
                case 1:  // mmtypeSbs
                case 2:  // mmtypeIndels
                    outputSymbolSize[0] = symSize;
                    codingSubSymSize[0] = symSize;
                    codingOrder[0] = subseqID;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
                    binParams[0][0] = getCmaxTU(numAlphaSpecial, codingSubSymSize[0]);  // cmax;

                    trnsfSubsymID[0] = paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::CLIPS:
            switch (subseqID) {
                case 0:  // clipsRecordID
                case 3:  // clipsHardClipsLength
                    outputSymbolSize[0] = 32;
                    codingSubSymSize[0] = 32;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::SUTU;
                    binParams[0][0] = 4;  // splitUnitSize;
                    break;
                case 1:  // clipsType
                    outputSymbolSize[0] = 4;
                    codingSubSymSize[0] = 4;
                    codingOrder[0] = 1;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
                    binParams[0][0] = getCmaxTU(numAlphaSpecial, codingSubSymSize[0]);  // cmax;

                    trnsfSubsymID[0] = paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                    break;
                case 2:  // clipsSoftClips
                    outputSymbolSize[0] = symSize;
                    codingSubSymSize[0] = symSize;
                    codingOrder[0] = 2;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
                    binParams[0][0] = getCmaxTU(numAlphaSpecial, codingSubSymSize[0]);  // cmax;

                    trnsfSubsymID[0] = paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::UREADS:
            switch (subseqID) {
                case 0:
                    outputSymbolSize[0] = symSize;
                    codingSubSymSize[0] = symSize;
                    codingOrder[0] = 2;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
                    binParams[0][0] = getCmaxTU(numAlphaSpecial, codingSubSymSize[0]);  // cmax;

                    trnsfSubsymID[0] = paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::RLEN:
            switch (subseqID) {
                case 0:
                    outputSymbolSize[0] = 32;
                    codingSubSymSize[0] = 32;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::SUTU;
                    binParams[0][0] = 4;  // splitUnitSize;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::PAIR:
            switch (subseqID) {
                case 0:  // pairType
                    outputSymbolSize[0] = 3;
                    codingSubSymSize[0] = 3;
                    codingOrder[0] = 1;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
                    binParams[0][0] = getCmaxTU(numAlphaSpecial, codingSubSymSize[0]);  // cmax;

                    trnsfSubsymID[0] = paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                    break;
                case 1:  // pairReadDistance
                    outputSymbolSize[0] = 16;
                    codingSubSymSize[0] = 4;
                    codingOrder[0] = 1;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
                    binParams[0][0] = getCmaxTU(numAlphaSpecial, codingSubSymSize[0]);  // cmax;

                    trnsfSubsymID[0] = paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                    break;
                case 2:
                case 3:
                case 6:
                case 7:  // pairAbsPos
                    outputSymbolSize[0] = 32;
                    codingSubSymSize[0] = 32;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::SUTU;
                    binParams[0][0] = 4;  // splitUnitSize;
                    break;
                case 4:
                case 5:  // pairSeqID
                    outputSymbolSize[0] = 16;
                    codingSubSymSize[0] = 16;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::SUTU;
                    binParams[0][0] = 4;  // splitUnitSize;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::MSCORE:
            switch (subseqID) {
                case 0:
                    outputSymbolSize[0] = 32;
                    codingSubSymSize[0] = 32;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::SSUTU;
                    binParams[0][0] = 4;  // splitUnitSize;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::MMAP:
            switch (subseqID) {
                case 0:  // number of alignments
                case 1:  // index of right alignments
                    outputSymbolSize[0] = 16;
                    codingSubSymSize[0] = 4;
                    codingOrder[0] = 2;
                    shareSubsymLutFlag[0] = false;
                    shareSubsymPrvFlag[0] = false;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
                    binParams[0][0] = getCmaxTU(numAlphaSpecial, codingSubSymSize[0]);  // cmax;

                    trnsfSubsymID[0] = paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                    break;
                case 2:  // more alignments flag
                    outputSymbolSize[0] = 1;
                    codingSubSymSize[0] = 1;
                    codingOrder[0] = 2;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::BI;
                    break;
                case 3:  // next alignment SeqID
                    outputSymbolSize[0] = 16;
                    codingSubSymSize[0] = 16;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::SUTU;
                    binParams[0][0] = 4;  // splitUnitSize;
                    break;
                case 4:  // next alignment abs mapping pos
                    outputSymbolSize[0] = 32;
                    codingSubSymSize[0] = 32;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::SUTU;
                    binParams[0][0] = 4;  // splitUnitSize;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::MSAR:
        case core::GenDesc::RNAME:
            tokenTypeFlag = true;
            switch (subseqID) {
                case 0:  // CABAC_METHOD_0
                    outputSymbolSize[0] = 8;
                    codingSubSymSize[0] = 8;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::SUTU;
                    binParams[0][0] = 4;  // splitUnitSize;
                    break;
                case 1:  // CABAC_METHOD_1
                    outputSymbolSize[0] = 8;
                    codingSubSymSize[0] = 4;
                    codingOrder[0] = 1;
                    shareSubsymLutFlag[0] = false;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
                    binParams[0][0] = getCmaxTU(numAlphaSpecial, codingSubSymSize[0]);  // cmax;

                    trnsfSubsymID[0] = paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::RTYPE:
            switch (subseqID) {
                case 0:
                    outputSymbolSize[0] = 3;
                    codingSubSymSize[0] = 3;
                    codingOrder[0] = 2;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
                    binParams[0][0] = getCmaxTU(numAlphaSpecial, codingSubSymSize[0]);  // cmax;

                    trnsfSubsymID[0] = paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::RGROUP:
            switch (subseqID) {
                case 0:
                    outputSymbolSize[0] = 16;
                    codingSubSymSize[0] = 16;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::SUTU;
                    binParams[0][0] = 4;  // splitUnitSize;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::QV:
            switch (subseqID) {
                case 0:  // qvPresenceFlag
                    outputSymbolSize[0] = 1;
                    codingSubSymSize[0] = 1;
                    codingOrder[0] = 2;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
                    binParams[0][0] = getCmaxTU(numAlphaSpecial, codingSubSymSize[0]);  // cmax;

                    trnsfSubsymID[0] = paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                    break;
                default:  // subSequenceID >= 2
                    // TODO(Jan)
                    found = false;
            }
            break;
        case core::GenDesc::RFTP:
            switch (subseqID) {
                case 0:  // POS for first alignment
                    outputSymbolSize[0] = 32;
                    codingSubSymSize[0] = 32;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::SUTU;
                    binParams[0][0] = 4;  // splitUnitSize;
                    break;
                default:
                    found = false;
            }
            break;
        case core::GenDesc::RFTT:
            switch (subseqID) {
                case 0:
                    outputSymbolSize[0] = symSize;
                    codingSubSymSize[0] = symSize;
                    codingOrder[0] = 1;

                    binID[0] = paramcabac::BinarizationParameters::BinarizationId::TU;
                    binParams[0][0] = getCmaxTU(numAlphaSpecial, codingSubSymSize[0]);  // cmax;

                    trnsfSubsymID[0] = paramcabac::SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                    break;
                default:
                    found = false;
            }
            break;
    }

    if (!found) {
        // UTILS_DIE("Unsupported subseqID "+std::to_string(subseqID)+" for descID value "+std::to_string(descID));
    }

    paramcabac::TransformedParameters trnsfSubseqParams(trnsfSubseqID, trnsfSubseqParam);
    size_t numTrnsfSubseqs = trnsfSubseqParams.getNumStreams();

    if (trnsfSubseqID == paramcabac::TransformedParameters::TransformIdSubseq::MERGE_CODING) {
        trnsfSubseqParams.setMergeCodingShiftSizes(mergeCodingShiftSizes);
        numTrnsfSubseqs = trnsfSubseqParam;
    }

    std::vector<paramcabac::TransformedSubSeq> trnsfSubseqCfgs;

    for (size_t t = 0; t < numTrnsfSubseqs; t++) {
        trnsfSubseqCfgs.push_back(paramcabac::TransformedSubSeq(
            trnsfSubsymID[t],
            paramcabac::SupportValues(outputSymbolSize[t], codingSubSymSize[t], codingOrder[t], shareSubsymLutFlag[t],
                                      shareSubsymPrvFlag[t]),
            paramcabac::Binarization(binID[t], bypassFlag[t],
                                     paramcabac::BinarizationParameters(binID[t], binParams[t]),
                                     paramcabac::Context(adaptiveModeFlag[t], outputSymbolSize[t], codingSubSymSize[t],
                                                         shareSubsymCtxFlag[t])),
            genieSubseqID, alphaID));
    }

    paramcabac::Subsequence subseqCfg(std::move(trnsfSubseqParams), subseqID, tokenTypeFlag,
                                      std::move(trnsfSubseqCfgs));

    return subseqCfg;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
