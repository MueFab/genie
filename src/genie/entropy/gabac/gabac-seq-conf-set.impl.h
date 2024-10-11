/**
 * @file gabac-seq-conf-set.impl.h
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Implementation of template functions for the GabacSeqConfSet class.
 *
 * This file contains the implementation of template functions defined in the `GabacSeqConfSet` class.
 * The `fillDecoder` and `loadDescriptorDecoderCfg` functions are provided here to manage configurations
 * for MPEG-G descriptor decoding and setup the internal configuration of a decoder based on a descriptor's properties.
 *
 * @details The `fillDecoder` function is used to initialize the decoder with the appropriate subsequence
 * configurations. It extracts the configuration for each descriptor subsequence from the internal configuration set.
 * The `loadDescriptorDecoderCfg` function extracts and verifies a `DescriptorPresent` configuration
 * from a `ParameterSet`. This ensures that the appropriate MPEG-G parameter set configuration is used for CABAC
 * decoding.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_GABAC_SEQ_CONF_SET_IMPL_H_
#define SRC_GENIE_ENTROPY_GABAC_GABAC_SEQ_CONF_SET_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/entropy/gabac/gabac-seq-conf-set.h"  //!< Include the primary header for GabacSeqConfSet declarations.
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
void GabacSeqConfSet::fillDecoder(const core::GenomicDescriptorProperties &desc, T &decoder_config) const {
    for (const auto &subdesc : desc.subseqs) {
        auto subseqCfg = getConfAsGabac(subdesc.id).getSubseqConfig();
        decoder_config.setSubsequenceCfg(uint8_t(subdesc.id.second), std::move(subseqCfg));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T>
const T &GabacSeqConfSet::loadDescriptorDecoderCfg(const GabacSeqConfSet::ParameterSet &parameterSet,
                                                   core::GenDesc descriptor_id) {
    auto &curDesc = parameterSet.getEncodingSet().getDescriptor(descriptor_id);
    UTILS_DIE_IF(curDesc.isClassSpecific(), "Class specific config not supported");
    auto PRESENT = core::parameter::desc_pres::DescriptorPresent::PRESENT;
    auto &base_conf = curDesc.get();
    UTILS_DIE_IF(base_conf.getPreset() != PRESENT, "Config not present");
    auto &decoder_conf = dynamic_cast<const core::parameter::desc_pres::DescriptorPresent &>(base_conf).getDecoder();
    UTILS_DIE_IF(decoder_conf.getMode() != paramcabac::DecoderRegular::MODE_CABAC, "Config is not paramcabac");

    return dynamic_cast<const T &>(decoder_conf);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_GABAC_SEQ_CONF_SET_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
