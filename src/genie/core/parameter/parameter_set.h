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
 public:
    /**
     * @brief
     */
    struct SignatureCfg {
        boost::optional<uint8_t> signature_length;  //!< @brief

        /**
         * @brief
         * @param cfg
         * @return
         */
        bool operator==(const SignatureCfg &cfg) const;
    };

 private:
    DataUnit::DatasetType dataset_type;                             //!< @brief
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
    uint32_t reserved{};                                            //!< @brief
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

    AlphabetID getAlphabetID() const {
        return alphabet_ID;
    }

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

    /**
     * @brief
     */
    void activateSignature();

    /**
     * @brief
     * @return
     */
    bool isSignatureActivated() const;

    /**
     * @brief
     * @return
     */
    bool isSignatureConstLength() const;

    /**
     * @brief
     * @return
     */
    uint8_t getSignatureConstLength() const;

    /**
     * @brief
     * @param length
     */
    void setSignatureLength(uint8_t length);

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter &writer) const;

    /**
     * @brief
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
    EncodingSet(DataUnit::DatasetType _dataset_type, AlphabetID _alphabet_id, uint32_t _read_length, bool _paired_end,
                bool _pos_40_bits_flag, uint8_t _qv_depth, uint8_t _as_depth, bool _multiple_alignments_flag,
                bool _spliced_reads_flag);

    /**
     * @brief
     */
    EncodingSet();

    /**
     * @brief
     * @param bitReader
     */
    explicit EncodingSet(util::BitReader &bitReader);
};

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
     * @param _parameter_set_ID
     * @param _parent_parameter_set_ID
     * @param _set
     */
    ParameterSet(uint8_t _parameter_set_ID, uint8_t _parent_parameter_set_ID, EncodingSet _set);

    /**
     * @brief
     * @param pset
     * @return
     */
    bool operator==(const ParameterSet &pset) const;

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

    /**
     * @brief
     * @return
     */
    EncodingSet &getEncodingSet() { return set; }

    /**
     * @brief
     * @return
     */
    const EncodingSet &getEncodingSet() const { return set; }

    /**
     * @brief
     * @return
     */
    virtual uint64_t getLength() const;

    void print_debug(std::ostream &output, uint8_t, uint8_t) const override {
        output << "* Parameter set " << uint32_t (getID());
        if(getEncodingSet().isComputedReference()) {
            switch (getEncodingSet().getComputedRef().getAlgorithm()) {
                case core::parameter::ComputedRef::Algorithm::LOCAL_ASSEMBLY:
                    output << ", local assembly";
                    break;
                case core::parameter::ComputedRef::Algorithm::GLOBAL_ASSEMBLY:
                    output << ", global assembly";
                    break;
                case core::parameter::ComputedRef::Algorithm::REF_TRANSFORM:
                    output << ", ref transform";
                    break;
                case core::parameter::ComputedRef::Algorithm::PUSH_IN:
                    output << ", push in";
                    break;
                case core::parameter::ComputedRef::Algorithm::RESERVED:
                    output << ", reserved";
                    break;
            }
        } else {
            output << ", reference based";
        }
        switch (getEncodingSet().getAlphabetID()) {
            case core::AlphabetID::ACGTN:
                output << ", alphabet ACTGN";
                break;
            case core::AlphabetID::ACGTRYSWKMBDHVN_:
                output << ", alphabet ACGTRYSWKMBDHVN_";
                break;
            default:
                UTILS_DIE("Unknown alphabet ID");
        }
        output << ", " << getEncodingSet().getNumberTemplateSegments() << " segments" << std::endl;
    }

 private:
    uint8_t parameter_set_ID;         //!< @brief
    uint8_t parent_parameter_set_ID;  //!< @brief

    EncodingSet set;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_PARAMETER_SET_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
