/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_GABAC_SEQ_CONF_SET_H_
#define SRC_GENIE_ENTROPY_GABAC_GABAC_SEQ_CONF_SET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <utility>
#include <vector>
#include "genie/core/access-unit.h"
#include "genie/core/parameter/descriptor_present/descriptor_present.h"
#include "genie/entropy/gabac/gabac.h"
#include "genie/entropy/paramcabac/decoder.h"
#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/entropy/paramcabac/transformed-subseq.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 * @brief Manages the current set of gabac configurations and handles the translation to MPEG-G P2 parameter sets.
 */
class GabacSeqConfSet {
 private:
    typedef std::vector<gabac::EncodingConfiguration> SeqConf;  //!< @brief Configuration for one genomic descriptor
    std::vector<SeqConf> conf;                                  //!< @brief Configuration for all genomic descriptors

    // Shortcuts
    using DescriptorSubsequenceCfg = entropy::paramcabac::Subsequence;
    using ParameterSet = core::parameter::ParameterSet;

    /**
     * @brief Extract the mgb::desc_conf_pres::paramcabac::DecoderConfigurationCabac from a
     * mgb::ParameterSet. An exception will be thrown if none is available.
     * @param parameterSet Input parameter set
     * @param descriptor_id Which DecoderConfigurationCabac to extract
     * @return A properly casted DecoderConfigurationCabac. No new object is created, the object was already inside the
     * input parameter set
     */
    template <typename T>
    static const T &loadDescriptorDecoderCfg(const GabacSeqConfSet::ParameterSet &parameterSet,
                                             core::GenDesc descriptor_id) {
        auto &curDesc = parameterSet.getEncodingSet().getDescriptor(descriptor_id);
        UTILS_DIE_IF(curDesc.isClassSpecific(), "Class specific config not supported");
        auto PRESENT = core::parameter::desc_pres::DescriptorPresent::PRESENT;
        auto &base_conf = curDesc.get();
        UTILS_DIE_IF(base_conf.getPreset() != PRESENT, "Config not present");
        auto &decoder_conf =
            dynamic_cast<const core::parameter::desc_pres::DescriptorPresent &>(base_conf).getDecoder();
        UTILS_DIE_IF(decoder_conf.getMode() != paramcabac::DecoderRegular::MODE_CABAC, "Config is not paramcabac");

        return dynamic_cast<const T &>(decoder_conf);
    }

 public:
    /**
     * @brief Retrieve a configuration from the current set, fitting for the specified subsequence
     * @param sub - identifies descriptor subseuqence
     * @return Gabac configuration
     */
    const gabac::EncodingConfiguration &getConfAsGabac(core::GenSubIndex sub) const;

    /**
     * @brief
     * @param sub
     * @return
     */
    gabac::EncodingConfiguration &getConfAsGabac(core::GenSubIndex sub);

    /**
     * @brief Set a configuration for the specified subsequence
     * @param sub - identifies descriptor subsequence
     * @param subseqCfg - the descritpor subsequence configuration
     */
    void setConfAsGabac(core::GenSubIndex sub, DescriptorSubsequenceCfg &&subseqCfg);

    /**
     *  @brief Create a default config guaranteed to work (bypass, no transformation, binary binarization)
     */
    GabacSeqConfSet();

    /**
     * @brief Store the complete set of gabac configurations in an MPEG-G parameter set
     * @param parameterSet Output object
     */
    void storeParameters(ParameterSet &parameterSet) const;

    /**
     * @brief
     * @param desc
     * @param parameterSet
     */
    void storeParameters(core::GenDesc desc, core::parameter::DescriptorSubseqCfg &parameterSet) const;

    /**
     * @brief Load a complete set of gabac configurations to the internal memory of gabac configurations
     * @param parameterSet Input object
     */
    void loadParameters(const ParameterSet &parameterSet);

    /**
     * @brief
     * @tparam T
     * @param desc
     * @param decoder_config
     */
    template <typename T>
    void fillDecoder(const core::GenomicDescriptorProperties &desc, T &decoder_config) const {
        for (const auto &subdesc : desc.subseqs) {
            auto subseqCfg = getConfAsGabac(subdesc.id).getSubseqConfig();
            decoder_config.setSubsequenceCfg(uint8_t(subdesc.id.second), std::move(subseqCfg));
        }
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_GABAC_SEQ_CONF_SET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
