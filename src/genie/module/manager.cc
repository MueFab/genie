/**
 * Copyright 2018-2024 The Genie Authors.
 * @file manager.cc
 *
 * @brief Implements the module manager responsible for registering decoders and
 * quality value handlers.
 *
 * This file is part of the Genie project, which is designed for advanced
 * genomic data handling such as compression and reconstruction. The
 * `manager.cpp` file focuses on organizing and registering components like
 * entropy decoders and quality value handlers in the global configuration.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/module/manager.h"

#include "genie/core/global_cfg.h"
#include "genie/core/parameter/quality_values.h"
//#include "genie/entropy/bsc/param_decoder.h"
//#include "genie/entropy/lzma/param_decoder.h"
#include "genie/entropy/paramcabac/decoder.h"
#include "genie/entropy/zstd/param_decoder.h"
#include "genie/quality/paramqv1/qv_coding_config_1.h"

// -----------------------------------------------------------------------------

namespace genie::module {

// -----------------------------------------------------------------------------

void detect() {
    auto& indPark = core::GlobalCfg::GetSingleton().GetIndustrialPark();
    indPark.RegisterConstructor<core::parameter::desc_pres::DecoderRegular>(
        entropy::paramcabac::kModeCabac, &entropy::paramcabac::DecoderRegular::create);
    indPark.RegisterConstructor<core::parameter::desc_pres::DecoderTokenType>(
        entropy::paramcabac::kModeCabac, &entropy::paramcabac::DecoderTokenType::create);
//    indPark.RegisterConstructor<core::parameter::QualityValues>(quality::paramqv1::QualityValues1::MODE_QV1,
//                                                                &quality::paramqv1::QualityValues1::create);
}

// -----------------------------------------------------------------------------

}  // namespace genie::module

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------