#ifndef GENIE_PART1_DATASET_REGION_H
#define GENIE_PART1_DATASET_REGION_H

#include <cstdint>
#include <vector>

#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class DatasetRegion{
   private:
    uint16_t seq_ID;
    std::vector<uint8_t> class_IDs;
    uint64_t start_pos;
    uint64_t end_pos;

   public:

    DatasetRegion(uint16_t _seq_ID, uint64_t _start_pos, uint64_t end_pos);

    void addClassID(uint8_t _class_ID);
    void addClassIDs(std::vector<uint8_t>& _class_IDs);
    void setClassIDs(std::vector<uint8_t>&& _class_IDs);

    /*
     *
     */
    uint16_t getSeqID() const;

    /*
     * Implements num_classes. Variable type is u(8) instead of u(4)
     */
    uint8_t getNumClasses() const;

    /*
     *
     */
    const std::vector<uint8_t>& getClassIDs() const;

    /*
     *
     */
    uint64_t getStartPos() const;

    /*
     *
     */
    uint64_t getEndPos() const;

    /**
    * ISO 23092-1 Section 6.5.1.5.4 table 15
    *   (for num_regions)
    **/
    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DATASET_REGION_H
