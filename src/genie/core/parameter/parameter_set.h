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
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

namespace stats {
class PerfStats;
}

// ---------------------------------------------------------------------------------------------------------------------

namespace parameter {

class EncodingSet {
 private:
    struct SignatureCfg {
        boost::optional<uint8_t> signature_length;

        bool operator==(const SignatureCfg &cfg) const { return signature_length == cfg.signature_length; }
    };

    DataUnit::DatasetType dataset_type;            //!< @brief
    AlphabetID alphabet_ID;                        //!< @brief
    uint32_t read_length;                          //!< @brief
    uint8_t number_of_template_segments_minus1;    //!< @brief
    uint32_t max_au_data_unit_size;                //!< @brief
    bool pos_40_bits_flag;                         //!< @brief
    uint8_t qv_depth;                              //!< @brief
    uint8_t as_depth;                              //!< @brief
    std::vector<record::ClassType> class_IDs;      //!< @brief
    std::vector<DescriptorSubseqCfg> descriptors;  //!< @brief
    std::vector<std::string> rgroup_IDs;           //!< @brief
    bool multiple_alignments_flag;                 //!< @brief
    bool spliced_reads_flag;                       //!< @brief
    uint32_t reserved{};
    boost::optional<SignatureCfg> signature_cfg;                    //!< @brief
    std::vector<std::unique_ptr<QualityValues>> qv_coding_configs;  //!< @brief
    boost::optional<ComputedRef> parameter_set_crps;                //!< @brief

    /**
     * @brief
     * @param ps
     * @return
     */
    bool qual_cmp(const EncodingSet &ps) const;

 public:
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
     * @return
     */
    DataUnit::DatasetType getDatasetType() const;

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
    uint32_t getReadLength() const;

    /**
     * @brief
     * @param other
     * @return
     */
    EncodingSet &operator=(const EncodingSet &other);

    /**
     * @brief
     * @param other
     * @return
     */
    EncodingSet &operator=(EncodingSet &&other) noexcept;

    /**
     * @brief
     * @param other
     */
    EncodingSet(const EncodingSet &other);

    /**
     * @brief
     * @param other
     */
    EncodingSet(EncodingSet &&other) noexcept;

    /**
     * @brief
     * @param type
     * @return
     */
    const QualityValues &getQVConfig(record::ClassType type) const;

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
    bool operator==(const EncodingSet &ps) const;

    void activateSignature() { signature_cfg = SignatureCfg(); }

    bool isSignatureActivated() const { return signature_cfg != boost::none; }

    bool isSignatureConstLength() const {
        return isSignatureActivated() && signature_cfg->signature_length != boost::none;
    }

    uint8_t getSignatureConstLength() const { return *signature_cfg->signature_length; }

    void setSignatureLength(uint8_t length) {
        UTILS_DIE_IF(!isSignatureActivated(), "Signature not activated");
        signature_cfg->signature_length = length;
    }

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter &writer) const;

    //------------------------------------------------------------------------------------------------------------------

    EncodingSet(DataUnit::DatasetType _dataset_type, AlphabetID _alphabet_id, uint32_t _read_length, bool _paired_end,
                bool _pos_40_bits_flag, uint8_t _qv_depth, uint8_t _as_depth, bool _multiple_alignments_flag,
                bool _spliced_reads_flag);

    // ---------------------------------------------------------------------------------------------------------------------

    EncodingSet();

    explicit EncodingSet(util::BitReader &bitReader);
};

/**
 * @brief MPEG-G Part 2 parameter set
 */
class ParameterSet : public DataUnit {
 private:
 public:
    /**
     * @brief Read parameter set from bitstream
     * @param bitReader Stream to read from
     */
    explicit ParameterSet(util::BitReader &bitReader) : DataUnit(DataUnitType::PARAMETER_SET) {
        bitReader.read<uint16_t>(10);
        bitReader.read<uint32_t>(22);
        parameter_set_ID = bitReader.read<uint8_t>(8);
        parent_parameter_set_ID = bitReader.read<uint8_t>(8);
        set = EncodingSet(bitReader);
    }

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
                 uint8_t _qv_depth, uint8_t _as_depth, bool _multiple_alignments_flag, bool _spliced_reads_flag)
        : DataUnit(DataUnitType::PARAMETER_SET),
          parameter_set_ID(_parameter_set_ID),
          parent_parameter_set_ID(_parent_parameter_set_ID),
          set(_dataset_type, _alphabet_id, _read_length, _paired_end, _pos_40_bits_flag, _qv_depth, _as_depth,
              _multiple_alignments_flag, _spliced_reads_flag) {}

    /**
     * @brief
     */
    ParameterSet() : DataUnit(DataUnitType::PARAMETER_SET), parameter_set_ID(0), parent_parameter_set_ID(0), set() {}

    bool operator==(const ParameterSet &pset) const {
        return parameter_set_ID == pset.parameter_set_ID && parent_parameter_set_ID == pset.parent_parameter_set_ID &&
               set == pset.set;
    }

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter &writer) const override;

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
     * @return
     */
    uint8_t getID() const;

    EncodingSet &getEncodingSet() { return set; }

    const EncodingSet &getEncodingSet() const { return set; }

    /**
     * @brief
     * @return
     */
    virtual uint64_t getLength() const;

 private:
    uint8_t parameter_set_ID;         //!< @brief
    uint8_t parent_parameter_set_ID;  //!< @brief

    EncodingSet set;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
