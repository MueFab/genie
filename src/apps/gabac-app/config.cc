#include "config.h"

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

#include <genie/entropy/paramcabac/subsequence.h>

namespace gabacify {

using namespace genie::entropy::paramcabac;

// ---------------------------------------------------------------------------------------------------------------------
unsigned long getCmaxTU(uint64_t const        numAlphaSpecial,
                        uint8_t const         codingSubsymSize) {
    if(numAlphaSpecial > 0) { /* special case */
        return numAlphaSpecial-1;
    } else {
        return StateVars::get2PowN(codingSubsymSize)-1;
    }
}

EncodingConfiguration getEncoderConfig(
//      uint8_t classID,
        uint8_t descID,
        uint8_t subseqID) {

    uint8_t symSize;
    GenDesc genieDescID = (GenDesc) descID;
    GenSubIndex genieSubseqID = (GenSubIndex) std::pair<GenDesc, uint8_t>(genieDescID, subseqID);

    // default values
    AlphabetID alphaID = AlphabetID::ACGTN;
    uint64_t numAlphaSpecial = StateVars::getNumAlphaSpecial(genieSubseqID,
                                                             alphaID);

    TransformedParameters::TransformIdSubseq trnsfSubseqID =
            TransformedParameters::TransformIdSubseq::NO_TRANSFORM;

    uint8_t codingSubSymSize = 32;
    uint8_t outputSymbolSize = 32;
    uint8_t codingOrder = 0;
    SupportValues::TransformIdSubsym trnsfSubsymID = SupportValues::TransformIdSubsym::NO_TRANSFORM;
    BinarizationParameters::BinarizationId binID =
            BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY;
    std::vector<uint8_t> binParams (2, 0);
    bool tokenTypeFlag = false;
    bool bypassFlag = false;
    bool adaptiveModeFlag = true;
    bool shareSubsymPrvFlag = true;
    bool shareSubsymLutFlag = true;
    bool shareSubsymCtxFlag = false;

    bool found = true;

    // populate config below
    switch(genieDescID) {
        case GenDesc::POS:
            switch(subseqID) {
                case 0: // POS for first alignment
                    outputSymbolSize = 32;
                    codingSubSymSize = 32;

                    binID = BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY;
                    binParams[0] = 4; // splitUnitSize;
                break;
                case 1: // POS for other alignments
                    outputSymbolSize = 32;
                    codingSubSymSize = 32;

                    binID = BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY;
                    binParams[0] = 4; // splitUnitSize;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::RCOMP:
            switch(subseqID) {
                case 0:
                    outputSymbolSize = 2;
                    codingSubSymSize = 2;
                    codingOrder = 1;

                    binID = BinarizationParameters::BinarizationId::TRUNCATED_UNARY;
                    binParams[0] = getCmaxTU(numAlphaSpecial, codingSubSymSize); // cmax;

                    trnsfSubsymID = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::FLAGS:
            switch(subseqID) {
                case 0:
                case 1:
                case 2:
                    outputSymbolSize = 1;
                    codingSubSymSize = 1;
                    codingOrder = 2;

                    binID = BinarizationParameters::BinarizationId::BINARY_CODING;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::MMPOS:
            switch(subseqID) {
                case 0: // mmposFlag
                    outputSymbolSize = 1;
                    codingSubSymSize = 1;
                    codingOrder = 2;

                    binID = BinarizationParameters::BinarizationId::BINARY_CODING;
                break;
                case 1: // mmposPos
                    outputSymbolSize = 16;
                    codingSubSymSize = 4;
                    codingOrder = 1;

                    binID = BinarizationParameters::BinarizationId::TRUNCATED_UNARY;
                    binParams[0] = getCmaxTU(numAlphaSpecial, codingSubSymSize); // cmax;

                    trnsfSubsymID = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::MMTYPE:
            switch(subseqID) {
                case 0: // mmtype
                    outputSymbolSize = 2;
                    codingSubSymSize = 2;
                    codingOrder = 1;

                    binID = BinarizationParameters::BinarizationId::TRUNCATED_UNARY;
                    binParams[0] = getCmaxTU(numAlphaSpecial, codingSubSymSize); // cmax;

                    trnsfSubsymID = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                break;
                case 1: // mmtypeSbs
                case 2: // mmtypeIndels
                    symSize = (alphaID == AlphabetID::ACGTN) ? 3 : 5;
                    outputSymbolSize = symSize;
                    codingSubSymSize = symSize;
                    codingOrder = subseqID;

                    binID = BinarizationParameters::BinarizationId::TRUNCATED_UNARY;
                    binParams[0] = getCmaxTU(numAlphaSpecial, codingSubSymSize); // cmax;

                    trnsfSubsymID = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::CLIPS:
            switch(subseqID) {
                case 0: // clipsRecordID
                case 3: // clipsHardClipsLength
                    outputSymbolSize = 32;
                    codingSubSymSize = 32;

                    binID = BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY;
                    binParams[0] = 4; // splitUnitSize;
                break;
                case 1: // clipsType
                    outputSymbolSize = 4;
                    codingSubSymSize = 4;
                    codingOrder = 1;

                    binID = BinarizationParameters::BinarizationId::TRUNCATED_UNARY;
                    binParams[0] = getCmaxTU(numAlphaSpecial, codingSubSymSize); // cmax;

                    trnsfSubsymID = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                break;
                case 2: // clipsSoftClips
                    symSize = (alphaID == AlphabetID::ACGTN) ? 3 : 5;
                    outputSymbolSize = symSize;
                    codingSubSymSize = symSize;
                    codingOrder = 2;

                    binID = BinarizationParameters::BinarizationId::TRUNCATED_UNARY;
                    binParams[0] = getCmaxTU(numAlphaSpecial, codingSubSymSize); // cmax;

                    trnsfSubsymID = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::UREADS:
            switch(subseqID) {
                case 0:
                    symSize = (alphaID == AlphabetID::ACGTN) ? 3 : 5;
                    outputSymbolSize = symSize;
                    codingSubSymSize = symSize;
                    codingOrder = 2;

                    binID = BinarizationParameters::BinarizationId::TRUNCATED_UNARY;
                    binParams[0] = getCmaxTU(numAlphaSpecial, codingSubSymSize); // cmax;

                    trnsfSubsymID = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::RLEN:
            switch(subseqID) {
                case 0:
                    outputSymbolSize = 32;
                    codingSubSymSize = 32;

                    binID = BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY;
                    binParams[0] = 4; // splitUnitSize;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::PAIR:
            switch(subseqID) {
                case 0: // pairType
                    outputSymbolSize = 3;
                    codingSubSymSize = 3;
                    codingOrder = 1;

                    binID = BinarizationParameters::BinarizationId::TRUNCATED_UNARY;
                    binParams[0] = getCmaxTU(numAlphaSpecial, codingSubSymSize); // cmax;

                    trnsfSubsymID = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                break;
                case 1: // pairReadDistance
                    outputSymbolSize = 16;
                    codingSubSymSize = 4;
                    codingOrder = 1;

                    binID = BinarizationParameters::BinarizationId::TRUNCATED_UNARY;
                    binParams[0] = getCmaxTU(numAlphaSpecial, codingSubSymSize); // cmax;

                    trnsfSubsymID = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                break;
                case 2: case 3: case 6: case 7: // pairAbsPos
                    outputSymbolSize = 32;
                    codingSubSymSize = 32;

                    binID = BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY;
                    binParams[0] = 4; // splitUnitSize;
                break;
                case 4: case 5: // pairSeqID
                    outputSymbolSize = 16;
                    codingSubSymSize = 16;

                    binID = BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY;
                    binParams[0] = 4; // splitUnitSize;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::MSCORE:
            switch(subseqID) {
                case 0:
                    outputSymbolSize = 32;
                    codingSubSymSize = 32;

                    binID = BinarizationParameters::BinarizationId::SIGNED_SPLIT_UNITWISE_TRUNCATED_UNARY;
                    binParams[0] = 4; // splitUnitSize;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::MMAP:
            switch(subseqID) {
                case 0: // number of alignments
                case 1: // index of right alignments
                    outputSymbolSize = 16;
                    codingSubSymSize = 4;
                    codingOrder = 2;
                    shareSubsymLutFlag = false;
                    shareSubsymPrvFlag = false;

                    binID = BinarizationParameters::BinarizationId::TRUNCATED_UNARY;
                    binParams[0] = getCmaxTU(numAlphaSpecial, codingSubSymSize); // cmax;

                    trnsfSubsymID = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                break;
                case 2: // more alignments flag
                    outputSymbolSize = 1;
                    codingSubSymSize = 1;
                    codingOrder = 2;

                    binID = BinarizationParameters::BinarizationId::BINARY_CODING;
                break;
                case 3: // next alignment SeqID
                    outputSymbolSize = 16;
                    codingSubSymSize = 16;

                    binID = BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY;
                    binParams[0] = 4; // splitUnitSize;
                break;
                case 4: // next alignment abs mapping pos
                    outputSymbolSize = 32;
                    codingSubSymSize = 32;

                    binID = BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY;
                    binParams[0] = 4; // splitUnitSize;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::MSAR:
        case GenDesc::RNAME:
            tokenTypeFlag = true;
            switch(subseqID) {
                case 0: // CABAC_METHOD_0
                    outputSymbolSize = 8;
                    codingSubSymSize = 8;

                    binID = BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY;
                    binParams[0] = 4; // splitUnitSize;
                break;
                case 1: // CABAC_METHOD_1
                    outputSymbolSize = 8;
                    codingSubSymSize = 4;
                    codingOrder = 1;
                    shareSubsymLutFlag = false;

                    binID = BinarizationParameters::BinarizationId::TRUNCATED_UNARY;
                    binParams[0] = getCmaxTU(numAlphaSpecial, codingSubSymSize); // cmax;

                    trnsfSubsymID = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::RTYPE:
            switch(subseqID) {
                case 0:
                    outputSymbolSize = 3;
                    codingSubSymSize = 3;
                    codingOrder = 2;

                    binID = BinarizationParameters::BinarizationId::TRUNCATED_UNARY;
                    binParams[0] = getCmaxTU(numAlphaSpecial, codingSubSymSize); // cmax;

                    trnsfSubsymID = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::RGROUP:
            switch(subseqID) {
                case 0:
                    outputSymbolSize = 16;
                    codingSubSymSize = 16;

                    binID = BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY;
                    binParams[0] = 4; // splitUnitSize;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::QV:
            switch(subseqID) {
                case 0: // qvPresenceFlag
                    outputSymbolSize = 1;
                    codingSubSymSize = 1;
                    codingOrder = 2;


                    binID = BinarizationParameters::BinarizationId::TRUNCATED_UNARY;
                    binParams[0] = getCmaxTU(numAlphaSpecial, codingSubSymSize); // cmax;

                    trnsfSubsymID = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                break;
                default:/* subSequenceID >= 2 */
                    found = false;
            }
        break;
        case GenDesc::RFTP:
            switch(subseqID) {
                case 0: // POS for first alignment
                    outputSymbolSize = 32;
                    codingSubSymSize = 32;

                    binID = BinarizationParameters::BinarizationId::SPLIT_UNITWISE_TRUNCATED_UNARY;
                    binParams[0] = 4; // splitUnitSize;
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::RFTT:
            switch(subseqID) {
                case 0:
                    symSize = (alphaID == AlphabetID::ACGTN) ? 3 : 5;
                    outputSymbolSize = symSize;
                    codingSubSymSize = symSize;
                    codingOrder = 1;

                    binID = BinarizationParameters::BinarizationId::TRUNCATED_UNARY;
                    binParams[0] = getCmaxTU(numAlphaSpecial, codingSubSymSize); // cmax;

                    trnsfSubsymID = SupportValues::TransformIdSubsym::LUT_TRANSFORM;
                break;
                default:
                    found = false;
            }
        break;
    }

    if(!found) {
        GABAC_DIE("Unsupported subseqID "+std::to_string(subseqID)+" for descID value "+std::to_string(subseqID));
    }

    TransformedSubSeq trnsfSubseqCfg(
            trnsfSubsymID,
            SupportValues(outputSymbolSize, codingSubSymSize, codingOrder, shareSubsymLutFlag, shareSubsymPrvFlag),
            Binarization(binID,
                         bypassFlag,
                         BinarizationParameters(binID, binParams),
                         Context(adaptiveModeFlag, outputSymbolSize, codingSubSymSize, shareSubsymCtxFlag)),
            genieSubseqID,
            alphaID
            );
    Subsequence subseqCfg(
            TransformedParameters(trnsfSubseqID, 0),
            subseqID,
            tokenTypeFlag,
            std::vector<TransformedSubSeq>({trnsfSubseqCfg}));

    return EncodingConfiguration(std::move(subseqCfg));
}

//------------------------------------------------------------------------------

}  // namespace gabacify

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
