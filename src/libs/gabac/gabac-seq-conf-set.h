/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GABAC_SEQ_CONF_SET_H
#define GENIE_GABAC_SEQ_CONF_SET_H

// ---------------------------------------------------------------------------------------------------------------------

#include <mpegg_p2/parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac.h>
#include <mpegg_p2/parameter_set/descriptor_configuration_present/cabac/descriptor_subsequence_cfg.h>
#include <mpegg_p2/parameter_set/descriptor_configuration_present/cabac/transform_subseq_parameters.h>

#include <gabac/gabac.h>

#include <vector>

#include <backbone/mpegg-raw-au.h>
namespace genie {
namespace gabac {

/**
 * @brief Manages the current set of gabac configurations and handles the translation to MPEG-G P2 parameter sets.
 */
class GabacSeqConfSet {
   private:
    typedef std::vector<gabac::EncodingConfiguration> SeqConf;  //!< @brief Configuration for one genomic descriptor
    std::vector<SeqConf> conf;                                  //!< @brief Configuration for all genomic descriptors

    // Shortcuts
    using TransformSubseqParameters = mpegg_p2::desc_conf_pres::cabac::TransformSubseqParameters;
    using TransformIdSubsym = mpegg_p2::desc_conf_pres::cabac::SupportValues::TransformIdSubsym;
    using CabacBinarization = mpegg_p2::desc_conf_pres::cabac::CabacBinarization;
    using DescriptorSubsequenceCfg = mpegg_p2::desc_conf_pres::cabac::DescriptorSubsequenceCfg;
    using DecoderConfigurationCabac = mpegg_p2::desc_conf_pres::cabac::DecoderConfigurationCabac;
    using ParameterSet = mpegg_p2::ParameterSet;
    using TransformSubseqCfg = mpegg_p2::desc_conf_pres::cabac::TransformSubseqCfg;

    /**
     * @brief Convert gabac::EncodingConfiguration to mpegg_p2::desc_conf_pres::cabac::TransformSubseqParameters
     * @param gabac_configuration Input gabac config
     * @return Newly created transform parameter object
     */
    static std::unique_ptr<TransformSubseqParameters> storeTransParams(
        const gabac::EncodingConfiguration& gabac_configuration);

    /**
     * @brief Convert gabac::TransformedSequenceConfiguration to
     * mpegg_p2::desc_conf_pres::cabac::SupportValues::TransformIdSubsym
     * @param tSeqConf Input gabac config
     * @return Newly created MPEG-G Transformation ID
     */
    static TransformIdSubsym storeTransform(const gabac::TransformedSequenceConfiguration& tSeqConf);

    /**
     * @brief Convert gabac::TransformedSequenceConfiguration to
     * mpegg_p2::desc_conf_pres::cabac::CabacBinarization
     * @param tSeqConf Input gabac config
     * @return Newly created MPEG-G Binarization object
     */
    static std::unique_ptr<CabacBinarization> storeBinarization(
        const gabac::TransformedSequenceConfiguration& tSeqConf);

    /**
     * @brief Convert gabac::EncodingConfiguration to mpegg_p2::desc_conf_pres::cabac::DescriptorSubsequenceCfg.
     * The complete subdescriptor sequence configuration will be processed including binarization, transformation, cabac
     * @param gabac_configuration Input gabac config
     * @param sub_conf Output where to put the converted data
     */
    static void storeSubseq(const gabac::EncodingConfiguration& gabac_configuration,
                            DescriptorSubsequenceCfg& sub_conf);

    /**
     * @brief Extract the mpegg_p2::desc_conf_pres::cabac::DecoderConfigurationCabac from a
     * mpegg_p2::ParameterSet. An exception will be thrown if none is available.
     * @param parameterSet Input parameter set
     * @param descriptor_id Which DecoderConfigurationCabac to extract
     * @return A properly casted DecoderConfigurationCabac. No new object is created, the object was already inside the
     * input parameter set
     */
    static const DecoderConfigurationCabac& loadDescriptorDecoderCfg(const ParameterSet& parameterSet,
                                                                     genie::GenDesc descriptor_id);

    /**
     * @brief Convert mpegg_p2::desc_conf_pres::cabac::TransformSubseqCfg to
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
    const gabac::EncodingConfiguration& getConfAsGabac(genie::GenSubIndex sub) const;

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
};
}  // namespace gabac
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GABAC_SEQ_CONF_SET_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
