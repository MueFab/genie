/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_GABAC_SEQ_CONF_SET_H
#define GENIE_GABAC_SEQ_CONF_SET_H

// ---------------------------------------------------------------------------------------------------------------------

#include <format/mpegg_p2/parameter_set/descriptor_configuration_present/cabac/descriptor_subsequence_cfg.h>
#include <format/mpegg_p2/parameter_set/descriptor_configuration_present/cabac/transform_subseq_parameters.h>
#include <gabac/gabac.h>
#include <vector>
#include "mpegg-raw-au.h"

/**
 *
 */
class GabacSeqConfSet {
   private:
    typedef std::vector<gabac::EncodingConfiguration> SeqConf;  //!<
    std::vector<SeqConf> conf;                                  //!<

    using TransformSubseqParameters = format::mpegg_p2::desc_conf_pres::cabac::TransformSubseqParameters;
    using TransformIdSubsym = format::mpegg_p2::desc_conf_pres::cabac::SupportValues::TransformIdSubsym;
    using CabacBinarization = format::mpegg_p2::desc_conf_pres::cabac::CabacBinarization;
    using DescriptorSubsequenceCfg = format::mpegg_p2::desc_conf_pres::cabac::DescriptorSubsequenceCfg;

   public:
    /**
     *
     */
    GabacSeqConfSet();

    /**
     *
     * @param conf
     * @return
     */
    std::unique_ptr<TransformSubseqParameters> transParamsFromGabac(MpeggRawAu::GenomicDescriptor desc,
                                                                    size_t sub) const;

    /**
     *
     * @param tSeqConf
     * @return
     */
    static TransformIdSubsym inferTransform(const gabac::TransformedSequenceConfiguration& tSeqConf);

    /**
     *
     * @param tSeqConf
     * @return
     */
    static std::unique_ptr<CabacBinarization> inferBinarization(
        const gabac::TransformedSequenceConfiguration& tSeqConf);

    /**
     *
     * @param conf
     * @param sub_conf
     */
    void fillSubseqFromGabac(MpeggRawAu::GenomicDescriptor desc, size_t sub, DescriptorSubsequenceCfg* sub_conf) const;

    /**
     *
     * @param desc
     * @param sub
     * @return
     */
    const gabac::EncodingConfiguration& getConfAsGabac(MpeggRawAu::GenomicDescriptor desc, size_t sub) const;

    void storeParameters(format::mpegg_p2::ParameterSet* parameterSet) const;
};

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_GABAC_SEQ_CONF_SET_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
