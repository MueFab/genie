/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#ifndef GENIE_MGG_FILE_H
#define GENIE_MGG_FILE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <memory>
#include <vector>
#include <sstream>
#include "file_header.h"
#include "genie/format/mpegg_p1/box.h"
#include "genie/util/bitreader.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
#include "genie/format/mpegg_p1/dataset_group.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class MggFile {
 private:
    std::vector<std::unique_ptr<Box>> boxes;
    std::istream* file;
    boost::optional<util::BitReader> reader;

 public:
    explicit MggFile(std::istream* _file) : file(_file), reader(*file) {
        while (true) {
            std::string boxname(4, '\0');
            reader->readBypass(boxname);
            if(!reader->isGood()) {
                break;
            }
            UTILS_DIE_IF(boxes.empty() && boxname != "flhd", "No file header found");
            UTILS_DIE_IF(!boxes.empty() && boxname == "flhd", "Multiple file headers found");
            if (boxname == "flhd") {
                boxes.emplace_back(genie::util::make_unique<genie::format::mpegg_p1::FileHeader>(*reader));
            } else if (boxname == "dgcn"){
                const auto& hdr = dynamic_cast<const genie::format::mpegg_p1::FileHeader&>(*boxes.front());
                boxes.emplace_back(genie::util::make_unique<genie::format::mpegg_p1::DatasetGroup>(*reader, hdr.getMinorVersion()));
            } else {
                std::cout << "Unknown Box " << boxname << " on top level of file. Exit.";
                break;
            }
        }
    }

    MggFile() : file(nullptr), reader(){

    }

    void addBox(std::unique_ptr<Box> box) {
        boxes.emplace_back(std::move(box));
    }

    void write(util::BitWriter& writer) {
        for (const auto& b : boxes) {
            b->write(writer);
        }
    }

    void print_debug(std::ostream& output, uint8_t max_depth = 100) const {
        for (const auto& b : boxes) {
            b->print_debug(output, 0, max_depth);
        }
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MGG_FILE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------