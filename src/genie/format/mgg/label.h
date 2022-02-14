/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_LABEL_H_
#define SRC_GENIE_FORMAT_MGG_LABEL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/core/meta/label.h"
#include "genie/format/mgg/gen_info.h"
#include "genie/format/mgg/label_dataset.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

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
     * @param dataset
     * @return
     */
    core::meta::Label decapsulate(uint16_t dataset);

    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;

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
    void box_write(genie::util::BitWriter& bit_writer) const override;

    /**
     * @brief
     * @param output
     * @param depth
     * @param max_depth
     */
    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_LABEL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
