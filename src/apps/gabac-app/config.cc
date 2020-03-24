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
    GenSubIndex geniesubseqID = (GenSubIndex) std::pair<GenDesc, uint8_t>(genieDescID, subseqID);

    // default values
    AlphabetID alphaID = AlphabetID::ACGTN;
    uint64_t numAlphaSpecial = StateVars::getNumAlphaSpecial(genieDescID,
                                                             geniesubseqID,
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


    if(!found) {
        GABAC_DIE("Unsupported subseqID "+std::to_string(subseqID)+" for descID value "+std::to_string(subseqID));
    }

    Subsequence subseqCfg(
            TransformedParameters(trnsfSubseqID, 0),
            subseqID,
            tokenTypeFlag);
    TransformedSeq trnsfSubseqCfg(
            trnsfSubsymID,
            SupportValues(outputSymbolSize, codingSubSymSize, codingOrder, shareSubsymPrvFlag, shareSubsymLutFlag),
            Binarization(binID,
                         bypassFlag,
                         BinarizationParameters(binID, binParams),
                         Context(adaptiveModeFlag, outputSymbolSize, codingSubSymSize, shareSubsymCtxFlag))
            );

    subseqCfg.setTransformSubseqCfg(0, std::move(trnsfSubseqCfg));

    return EncodingConfiguration(std::move(subseqCfg));
}

//------------------------------------------------------------------------------

}  // namespace gabacify

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
