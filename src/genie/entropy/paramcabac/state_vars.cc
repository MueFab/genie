/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include "state_vars.h"
#include <genie/core/constants.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

// ---------------------------------------------------------------------------------------------------------------------

StateVars::StateVars()
    : numAlphaSubsym(0),
      numSubsyms(0),
      numCtxSubsym(0),
      codingOrderCtxOffset{0,0,0},
      codingSizeCtxOffset(0),
      numCtxLuts(0),
      numCtxTotal(0){}

// ---------------------------------------------------------------------------------------------------------------------
void StateVars::populate(const SupportValues::TransformIdSubsym transform_ID_subsym,
                         const SupportValues support_values,
                         const Binarization cabac_binarization) {

    const uint8_t codingSubsymSize = support_values.getCodingSubsymSize();
    const uint8_t outputSymbolSize = support_values.getOutputSymbolSize();
    const uint8_t codingOrder = support_values.getCodingOrder();
    core::GenDesc descriptor_ID = core::GenDesc::POS;                   // need descriptor_ID TODO
    core::GenSubIndex subsequence_ID = core::GenSub::POS_MAPPING_FIRST; // need subsequence_ID TODO
    core::AlphabetID alphabet_ID = core::AlphabetID::ACGTN;             // need alphabet_ID TODO

    // numSubSymbols
    if(codingSubsymSize > 0) { // TODO: should do assert or return error?
        numSubsyms = outputSymbolSize / codingSubsymSize;
    } else {
        // TODO deal with error.
    }

    // numAlphabetSymbols
    // Check for special cases for numAlphabetSymbols
    if(descriptor_ID == core::GenDesc::MMTYPE || descriptor_ID == core::GenDesc::RFTT) {
        if(subsequence_ID == core::GenSub::MMTYPE_TYPE || subsequence_ID == core::GenSub::RFTT_TYPE) { // subseq 0
            numAlphaSubsym = 3;
        } else if((subsequence_ID == core::GenSub::MMTYPE_SUBSTITUTION || subsequence_ID == core::GenSub::RFTT_SUBSTITUTION) // subseq 1
                ||(subsequence_ID == core::GenSub::MMTYPE_INSERTION || subsequence_ID == core::GenSub::RFTT_INSERTION) // subseq 2
                  ) {
            numAlphaSubsym = (alphabet_ID == core::AlphabetID::ACGTN) ? 5 : 16;
        }
    } else if(descriptor_ID == core::GenDesc::CLIPS) {
        if(subsequence_ID == core::GenSub::CLIPS_TYPE) { // subseq 1
            numAlphaSubsym = 9;
        } else if(subsequence_ID == core::GenSub::CLIPS_SOFT_STRING) { // subseq 2
            numAlphaSubsym = ((alphabet_ID == core::AlphabetID::ACGTN) ? 5 : 16) + 1;
        }
    } else if(descriptor_ID == core::GenDesc::UREADS && subsequence_ID == core::GenSub::UREADS) { // subseq 0
        numAlphaSubsym = (alphabet_ID == core::AlphabetID::ACGTN) ? 5 : 16;
    } else if(descriptor_ID == core::GenDesc::RTYPE && subsequence_ID ==  core::GenSub::RTYPE) {
        numAlphaSubsym = 6;
    }

    if(numAlphaSubsym == 0) { // 0 == not special
        numAlphaSubsym = (1<<codingSubsymSize);
    }
}

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
