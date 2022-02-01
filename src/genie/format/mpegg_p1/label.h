/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_LABEL_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_LABEL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/format/mpegg_p1/label_dataset.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Label : public GenInfo {
 private:
    std::string label_ID;                     //!< @brief
    std::vector<LabelDataset> dataset_infos;  //!< @brief

 public:
    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;

    /**
     * @brief
     * @return
     */
    uint64_t getSize() const override;

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     */
    Label();

    /**
     *
     * @param _label_ID
     */
    explicit Label(std::string _label_ID);

    /**
     *
     * @param reader
     */
    explicit Label(util::BitReader& reader);

    /**
     *
     * @param _ds_info
     */
    void addDataset(LabelDataset _ds_info);

    /**
     *
     * @return
     */
    const std::string& getLabelID() const;

    /**
     * @brief
     * @return
     */
    const std::vector<LabelDataset>& getDatasets() const;

    /**
     *
     * @param bit_writer
     */
    void write(genie::util::BitWriter& bit_writer) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_LABEL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
