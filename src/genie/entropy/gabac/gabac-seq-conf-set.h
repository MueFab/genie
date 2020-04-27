/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GABAC_SEQ_CONF_SET_H
#define GENIE_GABAC_SEQ_CONF_SET_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/access-unit-raw.h>
#include <genie/entropy/gabac/gabac.h>
#include <genie/entropy/paramcabac/decoder.h>
#include <genie/entropy/paramcabac/subsequence.h>
#include <genie/entropy/paramcabac/transformed-subseq.h>
#include <vector>

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
    using DecoderConfigurationCabac = entropy::paramcabac::DecoderRegular;
    using ParameterSet = core::parameter::ParameterSet;

    /**
     * @brief Extract the mgb::desc_conf_pres::paramcabac::DecoderConfigurationCabac from a
     * mgb::ParameterSet. An exception will be thrown if none is available.
     * @param parameterSet Input parameter set
     * @param descriptor_id Which DecoderConfigurationCabac to extract
     * @return A properly casted DecoderConfigurationCabac. No new object is created, the object was already inside the
     * input parameter set
     */
    static const DecoderConfigurationCabac& loadDescriptorDecoderCfg(const ParameterSet& parameterSet,
                                                                     core::GenDesc descriptor_id);

   public:
    /**
     * @brief Retrieve a configuration from the current set, fitting for the specified subsequence
     * @param sub - identifies descriptor subseuqence
     * @return Gabac configuration
     */
    const gabac::EncodingConfiguration& getConfAsGabac(core::GenSubIndex sub) const;

    gabac::EncodingConfiguration& getConfAsGabac(core::GenSubIndex sub) {
        return conf[uint8_t(sub.first)][uint8_t(sub.second)];
    };

    /**
     * @brief Set a configuration for the specified subsequence
     * @param sub - identifies descriptor subsequence
     * @param subseqCfg - the descritpor subsequence configuration
     * @return none
     */
    void setConfAsGabac(core::GenSubIndex sub, DescriptorSubsequenceCfg &subseqCfg);

    /**
     *  @brief Create a default config guaranteed to work (bypass, no transformation, binary binarization)
     */
    GabacSeqConfSet();

    /**
     * @brief Store the complete set of gabac configurations in an MPEG-G parameter set
     * @param parameterSet Output object
     */
    void storeParameters(ParameterSet& parameterSet) const;

    /**
     * @brief Load a complete set of gabac configurations to the internal memory of gabac configurations
     * @param parameterSet Input object
     */
    void loadParameters(const ParameterSet& parameterSet);

    template <typename T>
    void fillDecoder(const core::GenomicDescriptorProperties& desc, T& decoder_config) const {
        for (const auto& subdesc : desc.subseqs) {
            auto subseqCfg = getConfAsGabac(subdesc.id).getSubseqConfig();
            decoder_config.setSubsequenceCfg(subdesc.id.second, std::move(subseqCfg));
        }
    }
};
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GABAC_SEQ_CONF_SET_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
