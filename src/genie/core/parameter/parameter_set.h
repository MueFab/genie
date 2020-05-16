/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_QV_PARAMETER_SET_H
#define GENIE_QV_PARAMETER_SET_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/constants.h>
#include <genie/core/record/record.h>
#include <genie/entropy/gabac/gabac.h>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <boost/optional/optional.hpp>
#include <cstdint>
#include <memory>
#include <vector>
#include "computed_ref.h"
#include "data_unit.h"
#include "descriptor_subseq_cfg.h"
#include "quality-values.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace stats {
class PerfStats;
}
namespace parameter {

class ParameterSet : public DataUnit {
   public:
    explicit ParameterSet(util::BitReader &bitReader);

    ParameterSet(uint8_t _parameter_set_ID, uint8_t _parent_parameter_set_ID, DatasetType _dataset_type,
                 AlphabetID _alphabet_id, uint32_t _read_length, bool _paired_end, bool _pos_40_bits_flag,
                 uint8_t _qv_depth, uint8_t _as_depth, bool _multiple_alignments_flag, bool _spliced_reads_flag);

    ParameterSet();

    const ComputedRef &getComputedRef() const;

    bool isComputedReference() const { return this->parameter_set_crps.has_value(); }

    size_t getNumberTemplateSegments() const;

    void setComputedRef(ComputedRef &&parameter_set_crps);

    void addClass(record::ClassType class_id, std::unique_ptr<QualityValues> conf);

    void setDescriptor(GenDesc index, DescriptorSubseqCfg &&descriptor);

    const DescriptorSubseqCfg &getDescriptor(GenDesc index) const;

    void addGroup(std::string &&rgroup_id);

    void setMultipleSignatureBase(uint32_t _multiple_signature_base, uint8_t _U_signature_size);

    void write(util::BitWriter &writer, genie::core::stats::PerfStats *stats = nullptr) const override;

    DatasetType getDatasetType() const;

    uint8_t getPosSize() const;

    bool hasMultipleAlignments() const;

    uint32_t getMultipleSignatureBase() const;

    uint8_t getSignatureSize() const;

    uint8_t getID() const;

    uint32_t getReadLength() const;

    ParameterSet &operator=(const ParameterSet &other);

    ParameterSet &operator=(ParameterSet &&other) noexcept;

    ParameterSet(const ParameterSet &other);

    ParameterSet(ParameterSet &&other) noexcept;

    const QualityValues &getQVConfig(record::ClassType type) const;

   private:
    void preWrite(util::BitWriter &writer) const;

    uint8_t parameter_set_ID;
    uint8_t parent_parameter_set_ID;

    DatasetType dataset_type;
    AlphabetID alphabet_ID;
    uint32_t read_length;
    uint8_t number_of_template_segments_minus1;
    uint32_t max_au_data_unit_size;
    bool pos_40_bits_flag;
    uint8_t qv_depth;
    uint8_t as_depth;
    std::vector<record::ClassType> class_IDs;
    std::vector<DescriptorSubseqCfg> descriptors;
    std::vector<std::string> rgroup_IDs;
    bool multiple_alignments_flag;
    bool spliced_reads_flag;
    uint32_t multiple_signature_base;
    boost::optional<uint8_t> u_signature_size;
    std::vector<std::unique_ptr<QualityValues>> qv_coding_configs;
    boost::optional<ComputedRef> parameter_set_crps;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_PARAMETER_SET_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
