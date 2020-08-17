/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "parameter_set.h"

#include <genie/core/global-cfg.h>

#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

// ---------------------------------------------------------------------------------------------------------------------

ParameterSet::ParameterSet(util::BitReader &bitReader) : DataUnit(DataUnitType::PARAMETER_SET) {
    bitReader.read<uint16_t>(10);  // ISO 23092-2 Section 3.1 table 3
    bitReader.read<uint32_t>(22);
    parameter_set_ID = bitReader.read<uint8_t>();
    parent_parameter_set_ID = bitReader.read<uint8_t>();
    dataset_type = bitReader.read<DatasetType>(4);
    alphabet_ID = bitReader.read<AlphabetID>();
    read_length = bitReader.read<uint32_t>(24);
    number_of_template_segments_minus1 = bitReader.read<uint8_t>(2);
    bitReader.read(6);
    max_au_data_unit_size = bitReader.read<uint32_t>(29);
    pos_40_bits_flag = bitReader.read<bool>(1);
    qv_depth = bitReader.read<uint8_t>(3);
    as_depth = bitReader.read<uint8_t>(3);
    auto num_classes = bitReader.read<uint8_t>(4);
    for (size_t i = 0; i < num_classes; ++i) {
        class_IDs.push_back(bitReader.read<record::ClassType>(4));
    }
    for (size_t i = 0; i < getDescriptors().size(); ++i) {
        descriptors.emplace_back(DescriptorSubseqCfg(num_classes, GenDesc(i), bitReader));
    }
    auto num_groups = bitReader.read<uint16_t>();
    for (size_t i = 0; i < num_groups; ++i) {
        rgroup_IDs.emplace_back();
        char c = 0;
        do {
            c = bitReader.read<uint8_t>();
            rgroup_IDs.back().push_back(c);
        } while (c);
    }
    multiple_alignments_flag = bitReader.read<bool>(1);
    spliced_reads_flag = bitReader.read<bool>(1);
    multiple_signature_base = bitReader.read<uint32_t>(31);
    if (multiple_signature_base > 0) {
        u_signature_size = bitReader.read<uint8_t>(6);
    }
    for (size_t i = 0; i < num_classes; ++i) {
        auto mode = bitReader.read<uint8_t>(4);
        qv_coding_configs.emplace_back(
            GlobalCfg::getSingleton().getIndustrialPark().construct<QualityValues>(mode, bitReader));
    }
    auto crps_flag = bitReader.read<bool>(1);
    if (crps_flag) {
        parameter_set_crps = ComputedRef(bitReader);
    }
    bitReader.flush();
}

//------------------------------------------------------------------------------------------------------------------

ParameterSet::ParameterSet(uint8_t _parameter_set_ID, uint8_t _parent_parameter_set_ID, DatasetType _dataset_type,
                           AlphabetID _alphabet_id, uint32_t _read_length, bool _paired_end, bool _pos_40_bits_flag,
                           uint8_t _qv_depth, uint8_t _as_depth, bool _multiple_alignments_flag,
                           bool _spliced_reads_flag)
    : DataUnit(DataUnitType::PARAMETER_SET),
      parameter_set_ID(_parameter_set_ID),
      parent_parameter_set_ID(_parent_parameter_set_ID),
      dataset_type(_dataset_type),
      alphabet_ID(_alphabet_id),
      read_length(_read_length),
      number_of_template_segments_minus1(static_cast<uint8_t>(_paired_end)),
      max_au_data_unit_size(0),
      pos_40_bits_flag(_pos_40_bits_flag),
      qv_depth(_qv_depth),
      as_depth(_as_depth),
      class_IDs(0),
      descriptors(18),
      rgroup_IDs(0),
      multiple_alignments_flag(_multiple_alignments_flag),
      spliced_reads_flag(_spliced_reads_flag),
      multiple_signature_base(0),
      u_signature_size(),
      qv_coding_configs(0),
      parameter_set_crps() {}

// ---------------------------------------------------------------------------------------------------------------------

ParameterSet::ParameterSet()
    : DataUnit(DataUnitType::PARAMETER_SET),
      parameter_set_ID(0),
      parent_parameter_set_ID(0),
      dataset_type(DatasetType::ALIGNED),
      alphabet_ID(AlphabetID::ACGTN),
      read_length(0),
      number_of_template_segments_minus1(0),
      max_au_data_unit_size(0),
      pos_40_bits_flag(false),
      qv_depth(0),
      as_depth(0),
      class_IDs(0),
      descriptors(18),
      rgroup_IDs(0),
      multiple_alignments_flag(false),
      spliced_reads_flag(false),
      multiple_signature_base(0),
      u_signature_size(),
      qv_coding_configs(0),
      parameter_set_crps() {}

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSet::write(util::BitWriter &writer) const {
    DataUnit::write(writer);
    writer.write(0, 10);  // reserved

    // Calculate size and write structure to tmp buffer
    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    preWrite(tmp_writer);
    tmp_writer.flush();
    uint64_t bits = tmp_writer.getBitsWritten();
    const uint64_t TYPE_SIZE_SIZE = 8 + 10 + 22;  // data_unit_type, reserved, data_unit_size
    bits += TYPE_SIZE_SIZE;
    uint64_t bytes = bits / 8 + ((bits % 8) ? 1 : 0);

    // Now size is known, write to final destination
    writer.write(bytes, 22);
    writer.writeBypass(&ss);
}

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSet::preWrite(util::BitWriter &writer) const {
    writer.write(parameter_set_ID, 8);
    writer.write(parent_parameter_set_ID, 8);
    writer.write(uint8_t(dataset_type), 4);
    writer.write(uint8_t(alphabet_ID), 8);
    writer.write(read_length, 24);
    writer.write(number_of_template_segments_minus1, 2);
    writer.write(0, 6);  // reserved_2
    writer.write(max_au_data_unit_size, 29);
    writer.write(static_cast<uint8_t>(pos_40_bits_flag), 1);
    writer.write(qv_depth, 3);
    writer.write(as_depth, 3);
    writer.write(class_IDs.size(), 4);  // num_classes
    for (auto &i : class_IDs) {
        writer.write(uint8_t(i), 4);
    }
    for (auto &i : descriptors) {
        i.write(writer);
    }
    writer.write(rgroup_IDs.size(), 16);  // num_groups
    for (auto &i : rgroup_IDs) {
        for (auto &j : i) {
            writer.write(static_cast<uint8_t>(j), 8);
        }
        writer.write('\0', 8);  // NULL termination
    }
    writer.write(static_cast<uint8_t>(multiple_alignments_flag), 1);
    writer.write(static_cast<uint8_t>(spliced_reads_flag), 1);
    writer.write(multiple_signature_base, 31);
    if (u_signature_size) {
        writer.write(*u_signature_size, 6);
    }
    for (auto &i : qv_coding_configs) {
        i->write(writer);
    }
    writer.write(static_cast<uint8_t>(static_cast<bool>(parameter_set_crps)), 1);
    if (parameter_set_crps) {
        parameter_set_crps->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const ComputedRef &ParameterSet::getComputedRef() const { return *parameter_set_crps; }

// ---------------------------------------------------------------------------------------------------------------------

size_t ParameterSet::getNumberTemplateSegments() const { return number_of_template_segments_minus1 + 1; }

// ---------------------------------------------------------------------------------------------------------------------

bool ParameterSet::isComputedReference() const { return this->parameter_set_crps.has_value(); }

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSet::setComputedRef(ComputedRef &&_parameter_set_crps) {
    parameter_set_crps = std::move(_parameter_set_crps);
}

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSet::addClass(record::ClassType class_id, std::unique_ptr<QualityValues> conf) {
    for (auto &a : descriptors) {
        if (a.isClassSpecific()) {
            UTILS_THROW_RUNTIME_EXCEPTION("Adding classes not allowed once class specific descriptor configs enabled");
        }
    }
    for (const auto &a : class_IDs) {
        if (class_id == a) {
            UTILS_THROW_RUNTIME_EXCEPTION("Class already added");
        }
    }
    class_IDs.push_back(class_id);
    qv_coding_configs.push_back(std::move(conf));
}

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSet::setDescriptor(GenDesc index, DescriptorSubseqCfg &&descriptor) {
    descriptors[uint8_t(index)] = std::move(descriptor);
}

// ---------------------------------------------------------------------------------------------------------------------

const DescriptorSubseqCfg &ParameterSet::getDescriptor(GenDesc index) const { return descriptors[uint8_t(index)]; }

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSet::addGroup(std::string &&rgroup_id) { rgroup_IDs.emplace_back(std::move(rgroup_id)); }

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSet::setMultipleSignatureBase(uint32_t _multiple_signature_base, uint8_t _U_signature_size) {
    multiple_signature_base = _multiple_signature_base;
    u_signature_size = _U_signature_size;
}

// ---------------------------------------------------------------------------------------------------------------------

ParameterSet::DatasetType ParameterSet::getDatasetType() const { return dataset_type; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ParameterSet::getPosSize() const { return pos_40_bits_flag ? uint8_t(40) : uint8_t(32); }

// ---------------------------------------------------------------------------------------------------------------------

bool ParameterSet::hasMultipleAlignments() const { return multiple_alignments_flag; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ParameterSet::getMultipleSignatureBase() const { return multiple_signature_base; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ParameterSet::getSignatureSize() const { return u_signature_size ? *u_signature_size : uint8_t(0); }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ParameterSet::getID() const { return parameter_set_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ParameterSet::getReadLength() const { return read_length; }

// ---------------------------------------------------------------------------------------------------------------------

ParameterSet &ParameterSet::operator=(const ParameterSet &other) {
    if (this == &other) {
        return *this;
    }
    parameter_set_ID = other.parameter_set_ID;
    parent_parameter_set_ID = other.parent_parameter_set_ID;
    dataset_type = other.dataset_type;
    alphabet_ID = other.alphabet_ID;
    read_length = other.read_length;
    number_of_template_segments_minus1 = other.number_of_template_segments_minus1;
    max_au_data_unit_size = other.max_au_data_unit_size;
    pos_40_bits_flag = other.pos_40_bits_flag;
    qv_depth = other.qv_depth;
    as_depth = other.as_depth;
    class_IDs = other.class_IDs;
    descriptors = other.descriptors;
    rgroup_IDs = other.rgroup_IDs;
    multiple_alignments_flag = other.multiple_alignments_flag;
    spliced_reads_flag = other.spliced_reads_flag;
    multiple_signature_base = other.multiple_signature_base;
    if (other.u_signature_size) {
        u_signature_size = other.u_signature_size;
    }
    qv_coding_configs.clear();
    for (const auto &c : other.qv_coding_configs) {
        qv_coding_configs.emplace_back(c->clone());
    }
    parameter_set_crps = other.parameter_set_crps;
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

ParameterSet &ParameterSet::operator=(ParameterSet &&other) noexcept {
    parameter_set_ID = other.parameter_set_ID;
    parent_parameter_set_ID = other.parent_parameter_set_ID;
    dataset_type = other.dataset_type;
    alphabet_ID = other.alphabet_ID;
    read_length = other.read_length;
    number_of_template_segments_minus1 = other.number_of_template_segments_minus1;
    max_au_data_unit_size = other.max_au_data_unit_size;
    pos_40_bits_flag = other.pos_40_bits_flag;
    qv_depth = other.qv_depth;
    as_depth = other.as_depth;
    class_IDs = std::move(other.class_IDs);
    descriptors = std::move(other.descriptors);
    rgroup_IDs = std::move(other.rgroup_IDs);
    multiple_alignments_flag = other.multiple_alignments_flag;
    spliced_reads_flag = other.spliced_reads_flag;
    multiple_signature_base = other.multiple_signature_base;
    u_signature_size = other.u_signature_size;
    qv_coding_configs = std::move(other.qv_coding_configs);
    parameter_set_crps = std::move(other.parameter_set_crps);
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

ParameterSet::ParameterSet(const ParameterSet &other)
    : DataUnit(DataUnitType::PARAMETER_SET),
      parameter_set_ID(0),
      parent_parameter_set_ID(0),
      dataset_type(DatasetType::ALIGNED),
      alphabet_ID(AlphabetID::ACGTN),
      read_length(0),
      number_of_template_segments_minus1(0),
      max_au_data_unit_size(0),
      pos_40_bits_flag(false),
      qv_depth(0),
      as_depth(0),
      class_IDs(0),
      descriptors(18),
      rgroup_IDs(0),
      multiple_alignments_flag(false),
      spliced_reads_flag(false),
      multiple_signature_base(0),
      u_signature_size(),
      qv_coding_configs(0),
      parameter_set_crps() {
    *this = other;
}

// ---------------------------------------------------------------------------------------------------------------------

ParameterSet::ParameterSet(ParameterSet &&other) noexcept
    : DataUnit(DataUnitType::PARAMETER_SET),
      parameter_set_ID(0),
      parent_parameter_set_ID(0),
      dataset_type(DatasetType::ALIGNED),
      alphabet_ID(AlphabetID::ACGTN),
      read_length(0),
      number_of_template_segments_minus1(0),
      max_au_data_unit_size(0),
      pos_40_bits_flag(false),
      qv_depth(0),
      as_depth(0),
      class_IDs(0),
      descriptors(18),
      rgroup_IDs(0),
      multiple_alignments_flag(false),
      spliced_reads_flag(false),
      multiple_signature_base(0),
      u_signature_size(),
      qv_coding_configs(0),
      parameter_set_crps() {
    *this = std::move(other);
}

// ---------------------------------------------------------------------------------------------------------------------

const QualityValues &ParameterSet::getQVConfig(record::ClassType type) const {
    for (size_t i = 0; i < class_IDs.size(); ++i) {
        if (class_IDs[i] == type) {
            return *(qv_coding_configs[i]);
        }
    }
    UTILS_DIE("No matching qv config " + std::to_string(int(type)) + " in parameter set");
}

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSet::setID(uint8_t id) { parameter_set_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSet::setParentID(uint8_t id) { parent_parameter_set_ID = id; }

// ---------------------------------------------------------------------------------------------------------------------

void ParameterSet::setQVDepth(uint8_t qv) { qv_depth = qv; }

// ---------------------------------------------------------------------------------------------------------------------

bool ParameterSet::operator==(const ParameterSet &ps) const {
    return dataset_type == ps.dataset_type && alphabet_ID == ps.alphabet_ID && read_length == ps.read_length &&
           number_of_template_segments_minus1 == ps.number_of_template_segments_minus1 &&
           max_au_data_unit_size == ps.max_au_data_unit_size && pos_40_bits_flag == ps.pos_40_bits_flag &&
           qv_depth == ps.qv_depth && as_depth == ps.as_depth && class_IDs == ps.class_IDs &&
           descriptors == ps.descriptors && rgroup_IDs == ps.rgroup_IDs &&
           multiple_alignments_flag == ps.multiple_alignments_flag && spliced_reads_flag == ps.spliced_reads_flag &&
           multiple_signature_base == ps.multiple_signature_base && u_signature_size == ps.u_signature_size &&
           qual_cmp(ps) && parameter_set_crps == ps.parameter_set_crps;
}

// ---------------------------------------------------------------------------------------------------------------------

bool ParameterSet::qual_cmp(const ParameterSet &ps) const {
    if (ps.qv_coding_configs.size() != qv_coding_configs.size()) {
        return false;
    }
    for (size_t i = 0; i < ps.qv_coding_configs.size(); ++i) {
        if (!(qv_coding_configs[i]->equals(ps.qv_coding_configs[i].get()))) {
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
