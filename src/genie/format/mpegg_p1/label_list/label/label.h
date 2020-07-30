/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef GENIE_PART1_LABEL_H
#define GENIE_PART1_LABEL_H

#include <string>
#include <vector>

#include "dataset_info.h"
#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class Label{
   private:
    /**
    * ISO 23092-1 Section 6.5.1.5.4 table 15
    **/
    std::string label_ID;

    std::vector<DatasetInfo> dataset_infos;

   public:

    Label();
    explicit Label(std::string& _label_ID);

    /*
     *
     */
    void addDatasetInfo(DatasetInfo&& _ds_info);

    /*
     *
     */
    void addDatasetInfos(std::vector<DatasetInfo>& _ds_infos);

    /*
     *
     */
    void setDatasetInfos(std::vector<DatasetInfo>&& _ds_infos);

    /*
     *
     */
    const std::string& getLabelID() const;

    /*
     * Implements num_datasets
     */
    uint16_t getNumDatasets() const;

    uint64_t getLength() const;

    void write(genie::util::BitWriter& bit_writer) const;
};


}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_LABEL_H
