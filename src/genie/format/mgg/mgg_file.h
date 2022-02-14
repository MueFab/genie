/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef SRC_GENIE_FORMAT_MGG_MGG_FILE_H_
#define SRC_GENIE_FORMAT_MGG_MGG_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include "genie/format/mgg/box.h"
#include "genie/format/mgg/dataset_group.h"
#include "genie/format/mgg/file_header.h"
#include "genie/util/bitreader.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

/**
 * @brief
 */
class MggFile {
 private:
    std::vector<std::unique_ptr<Box>> boxes;  //!< @brief
    std::istream* file;                       //!< @brief
    boost::optional<util::BitReader> reader;  //!< @brief

 public:
    /**
     * @brief
     * @param _file
     */
    explicit MggFile(std::istream* _file);

    /**
     * @brief
     * @return
     */
    std::vector<std::unique_ptr<Box>>& getBoxes();

    /**
     * @brief
     */
    MggFile();

    /**
     * @brief
     * @param box
     */
    void addBox(std::unique_ptr<Box> box);

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter& writer);

    /**
     * @brief
     * @param output
     * @param max_depth
     */
    void print_debug(std::ostream& output, uint8_t max_depth = 100) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_MGG_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
