#ifndef GENIE_PART1_REFERENCE_METADATA_H
#define GENIE_PART1_REFERENCE_METADATA_H

#include <cstdint>
#include <vector>

#include <genie/util/bitwriter.h>
#include <genie/util/bitreader.h>

//#include "reference_metadata_value.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class ReferenceMetadata {
   private:
    uint8_t dataset_group_ID;
    uint8_t reference_ID;
//    std::vector<ReferenceMetadataValue> reference_metadata_value;

   public:
    /**
     * Default
     */
    ReferenceMetadata();
    /**
        *
        * @param _ds_group_ID
        * @param _ref_ID
        */
    explicit ReferenceMetadata(uint8_t _ds_group_ID, uint8_t _ref_ID);
    /**
    *
    * @param reader
    * @param length
    */
    //ReferenceMetadata(util::BitReader& reader, size_t length);

    /**
     *
     * @param _dataset_group_ID
     */
    void setDatasetGroupId(uint8_t _dataset_group_ID);

    /**
     *
     * @return
     */
    uint64_t getLength() const;
    /**
     *
     * @param bit_writer
     */
    void write(util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
#endif  // GENIE_PART1_REFERENCE_METADATA_H
