#ifndef GENIE_PART1_DATASET_INFO_H
#define GENIE_PART1_DATASET_INFO_H

#include <cstdint>
#include <vector>
#include "genie/util/bitwriter.h"
#include "genie/util/bitreader.h"

#include "dataset_region.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class DatasetInfo{
   private:
    /**
    * ISO 23092-1 Section 6.5.1.5.4 table 15 (for num_dataset)
    *
    **/
    uint16_t dataset_ID;

    std::vector<DatasetRegion> dataset_regions;

   public:
    /**
     *
     */
    DatasetInfo();
    /**
     *
     * @param _ds_ID
     */
    explicit DatasetInfo(uint16_t _ds_ID);
    /**
     *
     * @param reader
     */
    void ReadDatasetInfo(util::BitReader& reader);


    /**
     *
     * @param _ds_region
     */
    void addDatasetRegion(DatasetRegion&& _ds_region);
    /**
     *
     * @param _ds_regions
     */
    void addDatasetRegions(std::vector<DatasetRegion>& _ds_regions);
    /**
     *
     * @param _ds_regions
     */
    void setDatasetRegions(std::vector<DatasetRegion>&& _ds_regions);
    /**
     *
     * @return
     */
    uint16_t getDatasetID() const;
    /**
     *
     * @return
     */
    uint8_t getNumRegions() const;


    /**
     *
     * @return
     */
    uint64_t getBitLength() const;
    /**
     *
     * @param bit_writer
     */
    void writeToFile(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DATASET_INFO_H
