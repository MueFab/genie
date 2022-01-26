/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_LABEL_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_LABEL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/format/mpegg_p1/label_list/label/dataset_info.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Label {
 private:
    /**
     * ISO 23092-1 Section 6.5.1.5.4 table 15
     **/
    std::string label_ID;
    std::vector<DatasetInfo> dataset_infos;

 public:
    /**
     *
     */
    Label();
    /**
     *
     * @param _label_ID
     */
    explicit Label(std::string& _label_ID);
    /**
     *
     * @param reader
     */
    void ReadLabel(util::BitReader& reader);

    /**
     *
     * @param _ds_info
     */
    void addDatasetInfo(DatasetInfo&& _ds_info);
    /**
     *
     * @param _ds_infos
     */
    void addDatasetInfos(std::vector<DatasetInfo>& _ds_infos);
    /**
     *
     * @param _ds_infos
     */
    void setDatasetInfos(std::vector<DatasetInfo>&& _ds_infos);
    /**
     *
     * @return
     */
    const std::string& getLabelID() const;
    /**
     *
     * @return
     */
    uint16_t getNumDatasets() const;

    /**
     *
     * @return
     */
    uint64_t getLength() const;
    /**
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_LABEL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
