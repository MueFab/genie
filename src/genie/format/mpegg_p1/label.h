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
#include "label_dataset.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/format/mpegg_p1/gen_info.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Label : public GenInfo {
 private:
    /**
     * ISO 23092-1 Section 6.5.1.5.4 table 15
     **/
    std::string label_ID;
    std::vector<LabelDataset> dataset_infos;

 public:

    const std::string& getKey() const override {
        static const std::string key = "lbll";
        return key;
    }

    uint64_t getSize() const override {
        auto bitSize = (label_ID.size() + 1 + sizeof(uint16_t)) * 8;
        for (const auto& d : dataset_infos) {
            bitSize += d.getBitLength();
        }
        return bitSize / 8 + (bitSize % 8 ? 1 : 0) + GenInfo::getSize();
    }

    /**
     *
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


    const std::vector<LabelDataset>& getDatasets() const {
        return dataset_infos;
    }

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

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_LABEL_LABEL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
