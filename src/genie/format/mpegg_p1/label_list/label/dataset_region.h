#ifndef GENIE_PART1_DATASET_REGION_H
#define GENIE_PART1_DATASET_REGION_H

#include <cstdint>
#include <vector>
#include "genie/util/bitwriter.h"
#include "genie/util/bitreader.h"


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
    /**
     *
     */
    DatasetRegion();
    /**
     *
     * @param _seq_ID
     * @param _start_pos
     * @param end_pos
     */
    DatasetRegion(uint16_t _seq_ID, uint64_t _start_pos, uint64_t end_pos);
    /**
     *
     * @param reader
     */
    void ReadDatasetRegion(util::BitReader& reader);

    /**
     *
     * @param _class_ID
     */
    void addClassID(uint8_t _class_ID);
    /**
     *
     * @param _class_IDs
     */
    void addClassIDs(std::vector<uint8_t>& _class_IDs);
    /**
     *
     * @param _class_IDs
     */
    void setClassIDs(std::vector<uint8_t>&& _class_IDs);
    /**
     *
     * @return
     */
    uint16_t getSeqID() const;
    /*
     * Implements num_classes. Variable type is u(8) instead of u(4)
     */
    uint8_t getNumClasses() const;
    /**
     *
     * @return
     */
    const std::vector<uint8_t>& getClassIDs() const;
    /**
     *
     * @return
     */
    uint64_t getStartPos() const;
    /**
     *
     * @return
     */
    uint64_t getEndPos() const;


    /**
     *
     * @return
     */
    uint64_t getBitLength() const;
    /**
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DATASET_REGION_H
