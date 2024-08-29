/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/module/manager.h"
#include "genie/core/global-cfg.h"
#include "genie/core/parameter/quality-values.h"
#include "genie/entropy/paramcabac/decoder.h"
#include "genie/quality/paramqv1/qv_coding_config_1.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::module {

// ---------------------------------------------------------------------------------------------------------------------

void detect() {
    auto& indPark = core::GlobalCfg::getSingleton().getIndustrialPark();
    indPark.registerConstructor<core::parameter::desc_pres::DecoderRegular>(
        entropy::paramcabac::DecoderRegular::MODE_CABAC, &entropy::paramcabac::DecoderRegular::create);
    indPark.registerConstructor<core::parameter::desc_pres::DecoderTokentype>(
        entropy::paramcabac::DecoderRegular::MODE_CABAC, &entropy::paramcabac::DecoderTokenType::create);
    indPark.registerConstructor<core::parameter::QualityValues>(quality::paramqv1::QualityValues1::MODE_QV1,
                                                                &quality::paramqv1::QualityValues1::create);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::module

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
