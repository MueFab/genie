/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_MGB_FILE_H_
#define SRC_GENIE_FORMAT_MGB_MGB_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <utility>
#include <vector>
#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgb/data-unit-factory.h"
#include "genie/format/mgb/raw_reference.h"
#include "genie/util/bit_reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 * @brief
 */
class MgbFile {
 private:
    std::vector<std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>> units;  //!< @brief
    std::istream* file;                                                                         //!< @brief
    std::unique_ptr<util::BitReader> reader;                                                    //!< @brief

    std::map<size_t, core::parameter::EncodingSet> parameterSets;  //!< @brief

    /**
     * @brief
     * @param u
     * @return
     */
    static uint8_t data_unit_order(const genie::core::parameter::DataUnit& u);

    /**
     * @brief
     * @tparam LAMBDA
     * @param u1
     * @param u2
     * @param lambda
     * @return
     */
    template <typename LAMBDA>
    static bool base_sorter(const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u1,
                            const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u2,
                            LAMBDA lambda);

 public:
    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer);

    /**
     * @brief
     * @param unit
     */
    void addUnit(std::unique_ptr<genie::core::parameter::DataUnit> unit);

    /**
     * @brief
     */
    MgbFile();

    /**
     * @brief
     * @param _file
     */
    explicit MgbFile(std::istream* _file);

    /**
     * @brief
     * @param output
     * @param max_depth
     */
    void print_debug(std::ostream& output, uint8_t max_depth = 100) const;

    /**
     * @brief
     */
    void sort_by_class();

    /**
     * @brief
     */
    void sort_by_position();

    /**
     * @brief
     * @param type
     */
    void remove_class(core::record::ClassType type);

    /**
     * @brief
     * @param ref_id
     * @param start_pos
     * @param end_pos
     */
    void select_mapping_range(uint16_t ref_id, uint64_t start_pos, uint64_t end_pos);

    /**
     * @brief
     */
    void remove_unused_parametersets();

    /**
     * @brief
     * @param type
     * @param descriptor
     * @param param_sets
     * @return
     */
    std::vector<Block> extractDescriptor(core::record::ClassType type, core::GenDesc descriptor,
                                         const std::vector<uint8_t>& param_sets);

    /**
     * @brief
     * @param param_sets
     */
    void clearAUBlocks(const std::vector<uint8_t>& param_sets);

    /**
     * @brief
     * @param param_sets
     * @return
     */
    std::vector<std::unique_ptr<core::parameter::ParameterSet>> extractParameters(
        const std::vector<uint8_t>& param_sets);

    /**
     * @brief
     * @param param_sets
     * @return
     */
    std::vector<std::unique_ptr<format::mgb::AccessUnit>> extractAUs(const std::vector<uint8_t>& param_sets);

    /**
     * @brief
     * @param multipleAlignments
     * @param pos40
     * @param dataset_type
     * @param alphabet
     * @return
     */
    std::vector<uint8_t> collect_param_ids(bool multipleAlignments, bool pos40,
                                           genie::core::parameter::DataUnit::DatasetType dataset_type,
                                           genie::core::AlphabetID alphabet);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/format/mgb/mgb_file.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_MGB_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
