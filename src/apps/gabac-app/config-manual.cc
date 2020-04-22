#include "config-manual.h"

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

#define MAX_NUM_TRANSPORT_SEQS 4 // put max 4 for now.

// ---------------------------------------------------------------------------------------------------------------------
unsigned long getCmaxTU(uint64_t const        numAlphaSpecial,
                        uint8_t const         codingSubsymSize) {
    if(numAlphaSpecial > 0) { /* special case */
        return numAlphaSpecial-1;
    } else {
        return StateVars::get2PowN(codingSubsymSize)-1;
    }
}

EncodingConfiguration getEncoderConfigManual(
        uint8_t descID,
        uint8_t subseqID) {

    GenDesc genieDescID = (GenDesc) descID;
    GenSubIndex genieSubseqID = (GenSubIndex) std::pair<GenDesc, uint8_t>(genieDescID, subseqID);

    // default values
    AlphabetID alphaID = AlphabetID::ACGTN;
    //uint64_t numAlphaSpecial = StateVars::getNumAlphaSpecial(genieSubseqID, alphaID);
    //uint8_t symSize;

    TransformedParameters::TransformIdSubseq trnsfSubseqID =
            TransformedParameters::TransformIdSubseq::NO_TRANSFORM;
    uint16_t trnsfSubseqParam = 0;
    std::vector<uint8_t> mergeCodingShiftSizes(MAX_NUM_TRANSPORT_SEQS, 0);
    bool tokenTypeFlag = false;

    uint8_t codingSubSymSize[MAX_NUM_TRANSPORT_SEQS] = {8};
    uint8_t outputSymbolSize[MAX_NUM_TRANSPORT_SEQS] = {8};
    uint8_t codingOrder[MAX_NUM_TRANSPORT_SEQS] = {0};
    SupportValues::TransformIdSubsym trnsfSubsymID[MAX_NUM_TRANSPORT_SEQS] = {SupportValues::TransformIdSubsym::NO_TRANSFORM};
    BinarizationParameters::BinarizationId binID[MAX_NUM_TRANSPORT_SEQS] =
            {BinarizationParameters::BinarizationId::BINARY_CODING};
    std::vector<std::vector<uint8_t>> binParams(MAX_NUM_TRANSPORT_SEQS, std::vector<uint8_t>(2, 0));
    bool bypassFlag[MAX_NUM_TRANSPORT_SEQS] = {false};
    bool adaptiveModeFlag[MAX_NUM_TRANSPORT_SEQS] = {true};
    bool shareSubsymPrvFlag[MAX_NUM_TRANSPORT_SEQS] = {true};
    bool shareSubsymLutFlag[MAX_NUM_TRANSPORT_SEQS] = {true};
    bool shareSubsymCtxFlag[MAX_NUM_TRANSPORT_SEQS] = {false};

    bool found = true;

    // populate config below
    switch(genieDescID) {
        case GenDesc::POS:
            switch(subseqID) {
                case 0: // POS for first alignment
                    // TODO
                break;
                case 1: // POS for other alignments
                    // TODO
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::RCOMP:
            switch(subseqID) {
                case 0:
                    // TODO
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
                    // TODO
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::MMPOS:
            switch(subseqID) {
                case 0: // mmposFlag
                    // TODO
                break;
                case 1: // mmposPos
                    // TODO
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::MMTYPE:
            switch(subseqID) {
                case 0: // mmtype
                    // TODO
                break;
                case 1: // mmtypeSbs
                case 2: // mmtypeIndels
                    // TODO
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::CLIPS:
            switch(subseqID) {
                case 0: // clipsRecordID
                case 3: // clipsHardClipsLength
                    // TODO
                break;
                case 1: // clipsType
                    // TODO
                break;
                case 2: // clipsSoftClips
                    // TODO
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::UREADS:
            switch(subseqID) {
                case 0:
                    // TODO
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::RLEN:
            switch(subseqID) {
                case 0:
                    // TODO
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::PAIR:
            switch(subseqID) {
                case 0: // pairType
                    // TODO
                break;
                case 1: // pairReadDistance
                    // TODO
                break;
                case 2: case 3: case 6: case 7: // pairAbsPos
                    // TODO
                break;
                case 4: case 5: // pairSeqID
                    // TODO
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::MSCORE:
            switch(subseqID) {
                case 0:
                    // TODO
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::MMAP:
            switch(subseqID) {
                case 0: // number of alignments
                case 1: // index of right alignments
                    // TODO
                break;
                case 2: // more alignments flag
                    // TODO
                break;
                case 3: // next alignment SeqID
                    // TODO
                break;
                case 4: // next alignment abs mapping pos
                    // TODO
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
                    // TODO
                break;
                case 1: // CABAC_METHOD_1
                    // TODO
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::RTYPE:
            switch(subseqID) {
                case 0:
                    // TODO
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::RGROUP:
            switch(subseqID) {
                case 0:
                    // TODO
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::QV:
            switch(subseqID) {
                case 0: // qvPresenceFlag
                    // TODO
                break;
                default:/* subSequenceID >= 2 */
                    // TODO
                    found = false;
            }
        break;
        case GenDesc::RFTP:
            switch(subseqID) {
                case 0: // POS for first alignment
                    // TODO
                break;
                default:
                    found = false;
            }
        break;
        case GenDesc::RFTT:
            switch(subseqID) {
                case 0:
                    // TODO
                break;
                default:
                    found = false;
            }
        break;
    }

    if(!found) {
        GABAC_DIE("Unsupported subseqID "+std::to_string(subseqID)+" for descID value "+std::to_string(subseqID));
    }

    TransformedParameters trnsfSubseqParams(trnsfSubseqID, trnsfSubseqParam);
    size_t numTrnsfSubseqs = trnsfSubseqParams.getNumStreams();

    if(trnsfSubseqID == TransformedParameters::TransformIdSubseq::MERGE_CODING) {
        trnsfSubseqParams.setMergeCodingShiftSizes(mergeCodingShiftSizes);
        numTrnsfSubseqs = trnsfSubseqParam;
    }

    std::vector<TransformedSubSeq> trnsfSubseqCfgs;

    for(size_t t=0; t<numTrnsfSubseqs; t++) {
        trnsfSubseqCfgs.push_back(
                TransformedSubSeq(trnsfSubsymID[t],
                                  SupportValues(outputSymbolSize[t], codingSubSymSize[t], codingOrder[t], shareSubsymLutFlag[t], shareSubsymPrvFlag[t]),
                                  Binarization(binID[t],
                                               bypassFlag[t],
                                               BinarizationParameters(binID[t], binParams[t]),
                                               Context(adaptiveModeFlag[t], outputSymbolSize[t], codingSubSymSize[t], shareSubsymCtxFlag[t])),
                                  genieSubseqID,
                                  alphaID)
                                  );
    }

    Subsequence subseqCfg(
            std::move(trnsfSubseqParams),
            subseqID,
            tokenTypeFlag,
            std::move(trnsfSubseqCfgs));

    return EncodingConfiguration(std::move(subseqCfg));
}

//------------------------------------------------------------------------------

}  // namespace gabacify

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
