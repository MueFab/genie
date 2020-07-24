#ifndef GENIE_PART1_DATASET_INFO_H
#define GENIE_PART1_DATASET_INFO_H

#include <cstdint>
#include <vector>
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

    explicit DatasetInfo(uint16_t _ds_ID);

    /*
     *
     */
    void addDatasetRegion(DatasetRegion&& _ds_region);

    /*
     *
     */
    void addDatasetRegions(std::vector<DatasetRegion>& _ds_regions);

    /*
     *
     */
    void setDatasetRegions(std::vector<DatasetRegion>&& _ds_regions);

    /*
     *
     */
    uint16_t getDatasetID() const;

    /*
     *
     */
    uint8_t getNumRegions() const;

    /*
     *
     */
    const std::vector<DatasetRegion>& getDatasetRegions() const ;

    void write(genie::util::BitWriter& bit_writer) const;
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DATASET_INFO_H
