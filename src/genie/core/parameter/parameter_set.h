/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_PARAMETER_SET_H_
#define SRC_GENIE_CORE_PARAMETER_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/parameter/computed_ref.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/core/parameter/descriptor_subseq_cfg.h"
#include "genie/core/parameter/quality-values.h"
#include "genie/core/record/class-type.h"
#include "genie/util/bitreader.h"

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
 * @brief MPEG-G Part 2 parameter set
 */
class ParameterSet : public DataUnit {
 public:
    /**
     * @brief Read parameter set from bitstream
     * @param bitReader Stream to read from
     */
    explicit ParameterSet(util::BitReader &bitReader);

    /**
     * @brief Construct from raw values
     * @param _parameter_set_ID ID for this parameter set
     * @param _parent_parameter_set_ID ID of parent parameter set (set equal to _parameter_set_ID for no parent)
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
     * @brief
     */
    ParameterSet();

    /**
     * @brief
     * @return
     */
    const ComputedRef &getComputedRef() const;

    /**
     * @brief
     * @return
     */
    bool isComputedReference() const;

    /**
     * @brief
     * @return
     */
    size_t getNumberTemplateSegments() const;

    /**
     * @brief
     * @param parameter_set_crps
     */
    void setComputedRef(ComputedRef &&parameter_set_crps);

    /**
     * @brief
     * @param class_id
     * @param conf
     */
    void addClass(record::ClassType class_id, std::unique_ptr<QualityValues> conf);

    /**
     * @brief
     * @param index
     * @param descriptor
     */
    void setDescriptor(GenDesc index, DescriptorSubseqCfg &&descriptor);

    /**
     * @brief
     * @param index
     * @return
     */
    const DescriptorSubseqCfg &getDescriptor(GenDesc index) const;

    /**
     * @brief
     * @param rgroup_id
     */
    void addGroup(std::string &&rgroup_id);

    /**
     * @brief
     * @param _multiple_signature_base
     * @param _U_signature_size
     */
    void setMultipleSignatureBase(uint32_t _multiple_signature_base, uint8_t _U_signature_size);

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

    /**
     * @brief
     * @return
     */
    DatasetType getDatasetType() const;

    /**
     * @brief
     * @return
     */
    uint8_t getPosSize() const;

    /**
     * @brief
     * @return
     */
    bool hasMultipleAlignments() const;

    /**
     * @brief
     * @return
     */
    uint32_t getMultipleSignatureBase() const;

    /**
     * @brief
     * @return
     */
    uint8_t getSignatureSize() const;

    /**
     * @brief
     * @return
     */
    uint8_t getID() const;

    /**
     * @brief
     * @return
     */
    uint32_t getReadLength() const;

    /**
     * @brief
     * @param other
     * @return
     */
    ParameterSet &operator=(const ParameterSet &other);

    /**
     * @brief
     * @param other
     * @return
     */
    ParameterSet &operator=(ParameterSet &&other) noexcept;

    /**
     * @brief
     * @param other
     */
    ParameterSet(const ParameterSet &other);

    /**
     * @brief
     * @param other
     */
    ParameterSet(ParameterSet &&other) noexcept;

    /**
     * @brief
     * @param type
     * @return
     */
    const QualityValues &getQVConfig(record::ClassType type) const;

    /**
     * @brief
     * @param id
     */
    void setID(uint8_t id);

    /**
     * @brief
     * @param id
     */
    void setParentID(uint8_t id);

    /**
     * @brief
     * @return
     */
    uint8_t getParentID() const;

    /**
     * @brief
     * @param qv
     */
    void setQVDepth(uint8_t qv);

    /**
     * @brief
     * @param ps
     * @return
     */
    bool operator==(const ParameterSet &ps) const;

    /**
     * @brief
     * @return
     */
    uint64_t getLength() const;

    /**
     * @brief
     * @param writer
     */
    void preWrite(util::BitWriter &writer) const;

 private:
    /**
     * @brief
     * @param ps
     * @return
     */
    bool qual_cmp(const ParameterSet &ps) const;

    uint8_t parameter_set_ID;         //!< @brief
    uint8_t parent_parameter_set_ID;  //!< @brief

    DatasetType dataset_type;                                       //!< @brief
    AlphabetID alphabet_ID;                                         //!< @brief
    uint32_t read_length;                                           //!< @brief
    uint8_t number_of_template_segments_minus1;                     //!< @brief
    uint32_t max_au_data_unit_size;                                 //!< @brief
    bool pos_40_bits_flag;                                          //!< @brief
    uint8_t qv_depth;                                               //!< @brief
    uint8_t as_depth;                                               //!< @brief
    std::vector<record::ClassType> class_IDs;                       //!< @brief
    std::vector<DescriptorSubseqCfg> descriptors;                   //!< @brief
    std::vector<std::string> rgroup_IDs;                            //!< @brief
    bool multiple_alignments_flag;                                  //!< @brief
    bool spliced_reads_flag;                                        //!< @brief
    uint32_t multiple_signature_base;                               //!< @brief
    boost::optional<uint8_t> u_signature_size;                      //!< @brief
    std::vector<std::unique_ptr<QualityValues>> qv_coding_configs;  //!< @brief
    boost::optional<ComputedRef> parameter_set_crps;                //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
