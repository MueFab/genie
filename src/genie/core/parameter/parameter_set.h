/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_QV_PARAMETER_SET_H
#define GENIE_QV_PARAMETER_SET_H

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>

#include <genie/core/constants.h>
#include <genie/util/bitreader.h>
#include <genie/core/record/class-type.h>

#include "computed_ref.h"
#include "data_unit.h"
#include "descriptor_subseq_cfg.h"
#include "quality-values.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

namespace stats {
class PerfStats;
}

// ---------------------------------------------------------------------------------------------------------------------

namespace parameter {
/**
 *
 */
class ParameterSet : public DataUnit {
   public:
    /**
     *
     * @param bitReader
     */
    explicit ParameterSet(util::BitReader &bitReader);

    /**
     *
     * @param _parameter_set_ID
     * @param _parent_parameter_set_ID
     * @param _dataset_type
     * @param _alphabet_id
     * @param _read_length
     * @param _paired_end
     * @param _pos_40_bits_flag
     * @param _qv_depth
     * @param _as_depth
     * @param _multiple_alignments_flag
     * @param _spliced_reads_flag
     */
    ParameterSet(uint8_t _parameter_set_ID, uint8_t _parent_parameter_set_ID, DatasetType _dataset_type,
                 AlphabetID _alphabet_id, uint32_t _read_length, bool _paired_end, bool _pos_40_bits_flag,
                 uint8_t _qv_depth, uint8_t _as_depth, bool _multiple_alignments_flag, bool _spliced_reads_flag);

    /**
     *
     */
    ParameterSet();

    /**
     *
     * @return
     */
    const ComputedRef &getComputedRef() const;

    /**
     *
     * @return
     */
    bool isComputedReference() const;

    /**
     *
     * @return
     */
    size_t getNumberTemplateSegments() const;

    /**
     *
     * @param parameter_set_crps
     */
    void setComputedRef(ComputedRef &&parameter_set_crps);

    /**
     *
     * @param class_id
     * @param conf
     */
    void addClass(record::ClassType class_id, std::unique_ptr<QualityValues> conf);

    /**
     *
     * @param index
     * @param descriptor
     */
    void setDescriptor(GenDesc index, DescriptorSubseqCfg &&descriptor);

    /**
     *
     * @param index
     * @return
     */
    const DescriptorSubseqCfg &getDescriptor(GenDesc index) const;

    /**
     *
     * @param rgroup_id
     */
    void addGroup(std::string &&rgroup_id);

    /**
     *
     * @param _multiple_signature_base
     * @param _U_signature_size
     */
    void setMultipleSignatureBase(uint32_t _multiple_signature_base, uint8_t _U_signature_size);

    /**
     *
     * @param writer
     * @param stats
     */
    void write(util::BitWriter &writer) const override;

    /**
     *
     * @return
     */
    DatasetType getDatasetType() const;

    /**
     *
     * @return
     */
    uint8_t getPosSize() const;

    /**
     *
     * @return
     */
    bool hasMultipleAlignments() const;

    /**
     *
     * @return
     */
    uint32_t getMultipleSignatureBase() const;

    /**
     *
     * @return
     */
    uint8_t getSignatureSize() const;

    /**
     *
     * @return
     */
    uint8_t getID() const;

    /**
     *
     * @return
     */
    uint32_t getReadLength() const;

    /**
     *
     * @param other
     * @return
     */
    ParameterSet &operator=(const ParameterSet &other);

    /**
     *
     * @param other
     * @return
     */
    ParameterSet &operator=(ParameterSet &&other) noexcept;

    /**
     *
     * @param other
     */
    ParameterSet(const ParameterSet &other);

    /**
     *
     * @param other
     */
    ParameterSet(ParameterSet &&other) noexcept;

    /**
     *
     * @param type
     * @return
     */
    const QualityValues &getQVConfig(record::ClassType type) const;

    /**
     *
     * @param id
     */
    void setID(uint8_t id);

    /**
     *
     * @param id
     */
    void setParentID(uint8_t id);

    /**
     *
     * @param qv
     */
    void setQVDepth(uint8_t qv);

    /**
     *
     * @param ps
     * @return
     */
    bool operator==(const ParameterSet &ps) const;

   private:
    /**
     *
     * @param ps
     * @return
     */
    bool qual_cmp(const ParameterSet &ps) const;

    /**
     *
     * @param writer
     */
    void preWrite(util::BitWriter &writer) const;

    uint8_t parameter_set_ID;         //!<
    uint8_t parent_parameter_set_ID;  //!<

    DatasetType dataset_type;                                       //!<
    AlphabetID alphabet_ID;                                         //!<
    uint32_t read_length;                                           //!<
    uint8_t number_of_template_segments_minus1;                     //!<
    uint32_t max_au_data_unit_size;                                 //!<
    bool pos_40_bits_flag;                                          //!<
    uint8_t qv_depth;                                               //!<
    uint8_t as_depth;                                               //!<
    std::vector<record::ClassType> class_IDs;                       //!<
    std::vector<DescriptorSubseqCfg> descriptors;                   //!<
    std::vector<std::string> rgroup_IDs;                            //!<
    bool multiple_alignments_flag;                                  //!<
    bool spliced_reads_flag;                                        //!<
    uint32_t multiple_signature_base;                               //!<
    boost::optional<uint8_t> u_signature_size;                      //!<
    std::vector<std::unique_ptr<QualityValues>> qv_coding_configs;  //!<
    boost::optional<ComputedRef> parameter_set_crps;                //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_PARAMETER_SET_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
