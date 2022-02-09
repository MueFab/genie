/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/master_index_table.h"
#include <sstream>
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

// ---------------------------------------------------------------------------------------------------------------------

bool MasterIndexTable::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const MasterIndexTable&>(info);
    return aligned_aus == other.aligned_aus && unaligned_aus == other.unaligned_aus;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<std::vector<master_index_table::AlignedAUIndex>>>& MasterIndexTable::getAlignedAUs()
    const {
    return aligned_aus;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<master_index_table::UnalignedAUIndex>& MasterIndexTable::getUnalignedAUs() const {
    return unaligned_aus;
}

// ---------------------------------------------------------------------------------------------------------------------

MasterIndexTable::MasterIndexTable(uint16_t seq_count, uint8_t num_classes) {
    aligned_aus.resize(seq_count, std::vector<std::vector<master_index_table::AlignedAUIndex>>(
                                      num_classes, std::vector<master_index_table::AlignedAUIndex>()));
}

// ---------------------------------------------------------------------------------------------------------------------

MasterIndexTable::MasterIndexTable(util::BitReader& reader, const DatasetHeader& hdr) {
    auto start_pos = reader.getPos() - 4;
    auto length = reader.read<uint64_t>();
    aligned_aus.resize(hdr.getReferenceOptions().getSeqIDs().size(),
                       std::vector<std::vector<master_index_table::AlignedAUIndex>>(
                           hdr.getMITConfigs().size(), std::vector<master_index_table::AlignedAUIndex>()));
    for (size_t seq = 0; seq < hdr.getReferenceOptions().getSeqIDs().size(); ++seq) {
        for (size_t ci = 0; ci < hdr.getMITConfigs().size(); ++ci) {
            if (core::record::ClassType::CLASS_U == hdr.getMITConfigs()[ci].getClassID()) {
                continue;
            }
            for (size_t au_id = 0; au_id < hdr.getReferenceOptions().getSeqBlocks()[seq]; ++au_id) {
                aligned_aus[seq][ci].emplace_back(reader, hdr.getByteOffsetSize(), hdr.getPosBits(),
                                                  hdr.getDatasetType(), hdr.getMultipleAlignmentFlag(),
                                                  hdr.isBlockHeaderEnabled(),
                                                  hdr.getMITConfigs()[ci].getDescriptorIDs());
            }
        }
    }
    for (size_t uau_id = 0; uau_id < hdr.getNumUAccessUnits(); ++uau_id) {
        unaligned_aus.emplace_back(
            reader, hdr.getByteOffsetSize(), hdr.getPosBits(), hdr.getDatasetType(), hdr.getUOptions().hasSignature(),
            hdr.getUOptions().hasSignature() && hdr.getUOptions().getSignature().isConstLength(),
            hdr.getUOptions().hasSignature() && hdr.getUOptions().getSignature().isConstLength()
                ? hdr.getUOptions().getSignature().getConstLength()
                : static_cast<uint8_t>(0),
            hdr.isBlockHeaderEnabled(), hdr.getMITConfigs().back().getDescriptorIDs(), hdr.getAlphabetID());
    }
    UTILS_DIE_IF(start_pos + length != uint64_t(reader.getPos()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

void MasterIndexTable::box_write(genie::util::BitWriter& bitWriter) const {
    for (const auto& a : aligned_aus) {
        for (const auto& b : a) {
            for (const auto& c : b) {
                c.write(bitWriter);
            }
        }
    }
    for (const auto& a : unaligned_aus) {
        a.write(bitWriter);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& MasterIndexTable::getKey() const {
    static const std::string key = "mitb";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<uint64_t> MasterIndexTable::getDescriptorStreamOffsets(uint8_t class_index, uint8_t desc_index,
                                                                   bool isUnaligned, uint64_t total_size) const {
    std::vector<uint64_t> offsets;
    if (isUnaligned) {
        for (const auto& unaligned_au : unaligned_aus) {
            offsets.emplace_back(unaligned_au.getBlockOffsets().at(desc_index));
        }
    } else {
        for (const auto& seq : aligned_aus) {
            for (const auto& aligned_au : seq.at(class_index)) {
                offsets.emplace_back(aligned_au.getBlockOffsets().at(desc_index));
            }
        }
    }
    std::sort(offsets.begin(), offsets.end());
    uint64_t last = offsets.front();
    offsets.erase(offsets.begin());
    offsets.emplace_back(total_size + last);
    for (uint64_t& offset : offsets) {
        uint64_t tmp = offset;
        offset = offset - last;
        last = tmp;
    }
    return offsets;
}

// ---------------------------------------------------------------------------------------------------------------------

void MasterIndexTable::print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
    print_offset(output, depth, max_depth, "* Master index table");
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
