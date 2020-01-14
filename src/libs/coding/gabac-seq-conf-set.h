/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GABAC_SEQ_CONF_SET_H
#define GENIE_GABAC_SEQ_CONF_SET_H

// ---------------------------------------------------------------------------------------------------------------------

#include <format/mpegg_p2/parameter_set/descriptor_configuration_present/cabac/decoder_configuration_cabac.h>
#include <format/mpegg_p2/parameter_set/descriptor_configuration_present/cabac/descriptor_subsequence_cfg.h>
#include <format/mpegg_p2/parameter_set/descriptor_configuration_present/cabac/transform_subseq_parameters.h>

#include <gabac/gabac.h>

#include <vector>

#include "mpegg-raw-au.h"

namespace coding {

/**
 * @brief Manages the current set of gabac configurations and handles the translation to MPEG-G P2 parameter sets.
 */
class GabacSeqConfSet {
   private:
    typedef std::vector<gabac::EncodingConfiguration> SeqConf;  //!< @brief Configuration for one genomic descriptor
    std::vector<SeqConf> conf;                                  //!< @brief Configuration for all genomic descriptors

    // Shortcuts
    using TransformSubseqParameters = format::mpegg_p2::desc_conf_pres::cabac::TransformSubseqParameters;
    using TransformIdSubsym = format::mpegg_p2::desc_conf_pres::cabac::SupportValues::TransformIdSubsym;
    using CabacBinarization = format::mpegg_p2::desc_conf_pres::cabac::CabacBinarization;
    using DescriptorSubsequenceCfg = format::mpegg_p2::desc_conf_pres::cabac::DescriptorSubsequenceCfg;
    using DecoderConfigurationCabac = format::mpegg_p2::desc_conf_pres::cabac::DecoderConfigurationCabac;
    using ParameterSet = format::mpegg_p2::ParameterSet;
    using TransformSubseqCfg = format::mpegg_p2::desc_conf_pres::cabac::TransformSubseqCfg;

    /**
     * @brief Convert gabac::EncodingConfiguration to format::mpegg_p2::desc_conf_pres::cabac::TransformSubseqParameters
     * @param gabac_configuration Input gabac config
     * @return Newly created transform parameter object
     */
    static std::unique_ptr<TransformSubseqParameters> storeTransParams(
        const gabac::EncodingConfiguration& gabac_configuration);

    /**
     * @brief Convert gabac::TransformedSequenceConfiguration to
     * format::mpegg_p2::desc_conf_pres::cabac::SupportValues::TransformIdSubsym
     * @param tSeqConf Input gabac config
     * @return Newly created MPEG-G Transformation ID
     */
    static TransformIdSubsym storeTransform(const gabac::TransformedSequenceConfiguration& tSeqConf);

    /**
     * @brief Convert gabac::TransformedSequenceConfiguration to
     * format::mpegg_p2::desc_conf_pres::cabac::CabacBinarization
     * @param tSeqConf Input gabac config
     * @return Newly created MPEG-G Binarization object
     */
    static std::unique_ptr<CabacBinarization> storeBinarization(
        const gabac::TransformedSequenceConfiguration& tSeqConf);

    /**
     * @brief Convert gabac::EncodingConfiguration to format::mpegg_p2::desc_conf_pres::cabac::DescriptorSubsequenceCfg.
     * The complete subdescriptor sequence configuration will be processed including binarization, transformation, cabac
     * @param gabac_configuration Input gabac config
     * @param sub_conf Output where to put the converted data
     */
    static void storeSubseq(const gabac::EncodingConfiguration& gabac_configuration,
                            DescriptorSubsequenceCfg& sub_conf);

    /**
     * @brief Extract the format::mpegg_p2::desc_conf_pres::cabac::DecoderConfigurationCabac from a
     * format::mpegg_p2::ParameterSet. An exception will be thrown if none is available.
     * @param parameterSet Input parameter set
     * @param descriptor_id Which DecoderConfigurationCabac to extract
     * @return A properly casted DecoderConfigurationCabac. No new object is created, the object was already inside the
     * input parameter set
     */
    static const DecoderConfigurationCabac& loadDescriptorDecoderCfg(const ParameterSet& parameterSet,
                                                                     GenDesc descriptor_id);

    /**
     * @brief Convert format::mpegg_p2::desc_conf_pres::cabac::TransformSubseqCfg to
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
    const gabac::EncodingConfiguration& getConfAsGabac(GenSubIndex sub) const;

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

}  // namespace coding

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GABAC_SEQ_CONF_SET_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
