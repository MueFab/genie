

#ifndef GENIE_REFERENCE_METADATA
#define GENIE_REFERENCE_METADATA

#include <string>
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/util/bitreader.h"
#include "util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class ReferenceMetadata : public GenInfo {
 private:
    uint8_t dataset_group_id;
    uint8_t reference_id;
    std::string reference_metadata_value;

 public:
    const std::string& getKey() const override {
        static const std::string key = "rfmd";
        return key;
    }

    uint64_t getSize() const override {
        return GenInfo::getSize() + sizeof(uint8_t) * 2 + reference_metadata_value.length();
    }

    ReferenceMetadata(uint8_t _dataset_group_id, uint8_t _reference_id, std::string _reference_metadata_value)
        : dataset_group_id(_dataset_group_id),
          reference_id(_reference_id),
          reference_metadata_value(std::move(_reference_metadata_value)) {}

    explicit ReferenceMetadata(genie::util::BitReader& bitreader) {
        auto length = bitreader.readBypassBE<uint64_t>();
        auto len_value = (length - sizeof(uint8_t) * 2);
        reference_metadata_value.resize(len_value);
        dataset_group_id = bitreader.readBypassBE<uint8_t>();
        reference_id = bitreader.readBypassBE<uint8_t>();
        bitreader.readBypass(reference_metadata_value);
    }

    void write(genie::util::BitWriter& bitWriter) const override {
        GenInfo::write(bitWriter);
        bitWriter.writeBypassBE<uint8_t>(dataset_group_id);
        bitWriter.writeBypassBE<uint8_t>(reference_id);
        bitWriter.writeBypass(reference_metadata_value.data(), reference_metadata_value.length());
    }

    uint8_t getDatasetGroupID() const { return dataset_group_id; }

    uint8_t getReferenceID() const { return reference_id; }

    const std::string& getReferenceMetadataValue() const { return reference_metadata_value; }
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_REFERENCE_METADATA
