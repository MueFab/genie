/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/format/mpegg_p1/label.h"
#include "genie/util/bitreader.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class LabelList : public GenInfo {
 private:
    uint8_t dataset_group_ID;   //!< @brief
    std::vector<Label> labels;  //!< @brief

 public:

    void patchID(uint8_t groupID) {
        dataset_group_ID = groupID;
    }
    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;

    std::vector<genie::core::meta::Label> decapsulate(uint16_t dataset) {
        std::vector<genie::core::meta::Label> ret;
        for(auto& l : labels) {
            ret.emplace_back(l.decapsulate(dataset));
        }
        return ret;
    }

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @param _ds_group_ID
     */
    explicit LabelList(uint8_t _ds_group_ID);

    /**
     * @brief
     * @param reader
     */
    explicit LabelList(util::BitReader& reader);

    /**
     * @brief
     * @return
     */
    uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    const std::vector<Label>& getLabels() const;

    /**
     * @brief
     * @param l
     */
    void addLabel(Label l);

    /**
     * @brief
     * @param bit_writer
     */
    void box_write(genie::util::BitWriter& bit_writer) const override;


    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override {
        print_offset(output, depth, max_depth, "* Label list");
        for(const auto& l : labels) {
            l.print_debug(output, depth + 1, max_depth);
        }
    }


    void read_box(util::BitReader& reader, bool in_offset) {
        std::string tmp_str(4, '\0');
        reader.readBypass(tmp_str);
        if (tmp_str == "lbll") {
            labels.emplace_back(reader);
        } else if (tmp_str == "offs") {
            UTILS_DIE_IF(in_offset, "Recursive offset not permitted");
            reader.readBypass(tmp_str);
            uint64_t offset = reader.readBypassBE<uint64_t>();
            if (offset == ~static_cast<uint64_t>(0)) {
                read_box(reader, in_offset);
                return;
            }
            auto pos_save = reader.getPos();
            reader.setPos(offset);
            read_box(reader, true);
            reader.setPos(pos_save);
        } else {
            UTILS_DIE("Unknown box");
        }
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_LABEL_LIST_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
