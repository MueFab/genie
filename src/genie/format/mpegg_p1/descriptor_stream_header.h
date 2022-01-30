/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_HEADER_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_HEADER_H_

#include <memory>
#include <string>
#include <vector>
#include "dataset_header.h"
#include "genie/core/constants.h"
#include "genie/core/parameter/data_unit.h"
#include "genie/core/record/class-type.h"
#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgb/au_type_cfg.h"
#include "genie/format/mgb/mm_cfg.h"
#include "genie/format/mgb/ref_cfg.h"
#include "genie/format/mpegg_p1/file_header.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
#include "signature_cfg.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

class DSProtection : public GenInfo {
 private:
    std::string DSProtectionValue;

 public:
    DSProtection(std::string _DSProtectionValue) : DSProtectionValue(std::move(_DSProtectionValue)) {}

    DSProtection(genie::util::BitReader& reader) {
        uint64_t length = reader.readBypassBE<uint64_t>();
        DSProtectionValue.resize(length);
        reader.readBypass(DSProtectionValue);
    }

    uint64_t getSize() const override { return GenInfo::getSize() + DSProtectionValue.size(); }

    const std::string getProtectionValue() const { return DSProtectionValue; }

    const std::string& getKey() const override {
        static const std::string key = "dspr";
        return key;
    }

    void write(genie::util::BitWriter& bitWriter) const override {
        GenInfo::write(bitWriter);
        bitWriter.writeBypass(DSProtectionValue.data(), DSProtectionValue.size());
    }
};

/**
 * @brief
 */
class DescriptorStreamHeader : public GenInfo {
 private:
    bool reserved;
    genie::core::GenDesc descriptor_id;
    core::record::ClassType class_id;
    uint32_t num_blocks;

 public:
    bool getReserved() const { return reserved; }

    genie::core::GenDesc getDescriptorID() const { return descriptor_id; }

    core::record::ClassType getClassType() const { return class_id; }

    uint32_t getNumBlocks() const { return num_blocks; }

    void addBlock() { num_blocks++; }

    DescriptorStreamHeader() : DescriptorStreamHeader(0, genie::core::GenDesc(0), core::record::ClassType::NONE, 0) {}

    explicit DescriptorStreamHeader(bool _reserved, genie::core::GenDesc _genDesc, core::record::ClassType _class_id,
                                    uint32_t _num_blocks)
        : reserved(_reserved), descriptor_id(_genDesc), class_id(_class_id), num_blocks(_num_blocks) {}

    void write(genie::util::BitWriter& bitWriter) const override {
        GenInfo::write(bitWriter);
        bitWriter.write(reserved, 1);
        bitWriter.write(static_cast<uint8_t>(descriptor_id), 7);
        bitWriter.write(static_cast<uint8_t>(class_id), 4);
        bitWriter.write(num_blocks, 32);
        bitWriter.flush();
    }

    explicit DescriptorStreamHeader(genie::util::BitReader& reader) {
        reserved = reader.read<bool>(1);
        descriptor_id = reader.read<genie::core::GenDesc>(7);
        class_id = reader.read<core::record::ClassType>(4);
        num_blocks = reader.read<uint32_t>(32);
        reader.flush();
    }

    uint64_t getSize() const override {
        return GenInfo::getSize() + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t);
    }

    const std::string& getKey() const override {
        static const std::string key = "dshd";
        return key;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_DATASET_DESCRIPTOR_STREAM_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
