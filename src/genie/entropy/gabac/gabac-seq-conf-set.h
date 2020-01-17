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
#include <genie/entropy/paramcabac/transformed-seq.h>
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
    using TransformSubseqParameters = entropy::paramcabac::TransformedParameters;
    using TransformIdSubsym = entropy::paramcabac::SupportValues::TransformIdSubsym;
    using CabacBinarization = entropy::paramcabac::Binarization;
    using DescriptorSubsequenceCfg = entropy::paramcabac::Subsequence;
    using DecoderConfigurationCabac = entropy::paramcabac::DecoderRegular;
    using ParameterSet = core::parameter::ParameterSet;
    using TransformSubseqCfg = entropy::paramcabac::TransformedSeq;

    /**
     * @brief Convert gabac::EncodingConfiguration to mgb::desc_conf_pres::paramcabac::TransformSubseqParameters
     * @param gabac_configuration Input gabac config
     * @return Newly created transform parameter object
     */
    static std::unique_ptr<TransformSubseqParameters> storeTransParams(
        const gabac::EncodingConfiguration& gabac_configuration);

    /**
     * @brief Convert gabac::TransformedSequenceConfiguration to
     * mgb::desc_conf_pres::paramcabac::SupportValues::TransformIdSubsym
     * @param tSeqConf Input gabac config
     * @return Newly created MPEG-G Transformation ID
     */
    static TransformIdSubsym storeTransform(const gabac::TransformedSequenceConfiguration& tSeqConf);

    /**
     * @brief Convert gabac::TransformedSequenceConfiguration to
     * mgb::desc_conf_pres::paramcabac::CabacBinarization
     * @param tSeqConf Input gabac config
     * @return Newly created MPEG-G Binarization object
     */
    static std::unique_ptr<CabacBinarization> storeBinarization(
        const gabac::TransformedSequenceConfiguration& tSeqConf);

    /**
     * @brief Convert gabac::EncodingConfiguration to mgb::desc_conf_pres::paramcabac::DescriptorSubsequenceCfg.
     * The complete subdescriptor sequence configuration will be processed including binarization, transformation,
     * paramcabac
     * @param gabac_configuration Input gabac config
     * @param sub_conf Output where to put the converted data
     */
    static void storeSubseq(const gabac::EncodingConfiguration& gabac_configuration,
                            DescriptorSubsequenceCfg& sub_conf);

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

    /**
     * @brief Convert mgb::desc_conf_pres::paramcabac::TransformSubseqCfg to
     * gabac::TransformedSequenceConfiguration.
     * @param transformedDesc Input subsequence configuration
     * @param gabacTransCfg Where to write the output data.
     */
    static gabac::TransformedSequenceConfiguration loadTransformedSequence(const TransformSubseqCfg& transformedDesc);

   public:
    /**
     * @brief Retrieve a configuration from the current set, fitting for the specified subsequence
     * @param desc Which descriptor
     * @param sub Which subsequence
     * @return Gabac configuration
     */
    const gabac::EncodingConfiguration& getConfAsGabac(core::GenSubIndex sub) const;

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
    void fillDecoder(const core::GenomicDescriptorProperties& desc, T* decoder_config) const {
        for (const auto& subdesc : desc.subseqs) {
            auto transform_params = storeTransParams(getConfAsGabac(subdesc.id));
            decoder_config->setSubsequenceCfg(subdesc.id.second, std::move(transform_params));

            // This is where actual translation of one gabac config to MPEGG takes place
            auto subseq_cfg = decoder_config->getSubsequenceCfg(subdesc.id.second);
            storeSubseq(getConfAsGabac(subdesc.id), *subseq_cfg);
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
