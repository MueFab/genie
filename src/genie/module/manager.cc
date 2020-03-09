/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "manager.h"
#include <genie/core/format-importer.h>
#include <genie/core/global-cfg.h>
#include <genie/core/name-decoder.h>
#include <genie/core/parameter/descriptor_present/decoder.h>
#include <genie/core/parameter/quality-values.h>
#include <genie/core/qv-decoder.h>
#include <genie/entropy/paramcabac/decoder.h>
#include <genie/name/tokenizer/decoder.h>
#include <genie/quality/paramqv1/qv_coding_config_1.h>
#include <genie/quality/qvwriteout/decoder.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace module {

// ---------------------------------------------------------------------------------------------------------------------

void detect() {
    auto& indPark = core::GlobalCfg::getSingleton().getIndustrialPark();
    indPark.registerConstructor<core::parameter::desc_pres::DecoderRegular>(
        entropy::paramcabac::DecoderRegular::MODE_CABAC, &entropy::paramcabac::DecoderRegular::create);
    indPark.registerConstructor<core::parameter::desc_pres::DecoderTokentype>(
        entropy::paramcabac::DecoderRegular::MODE_CABAC, &entropy::paramcabac::DecoderTokenType::create);
    indPark.registerConstructor<core::parameter::QualityValues>(quality::paramqv1::QualityValues1::MODE_QV1,
                                                                &quality::paramqv1::QualityValues1::create);
    indPark.registerConstructor<core::NameDecoder>(0,
                                                                &name::tokenizer::Decoder::create);

    indPark.registerConstructor<core::QVDecoder>(quality::paramqv1::QualityValues1::MODE_QV1,
                                                 &quality::qvwriteout::Decoder::create);

    //  indPark.registerConstructor<core::FormatImporter>()
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace module
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------