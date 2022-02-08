/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <utility>
#include <string>
#include "genie/format/mpegg_p1/mgg_file.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

MggFile::MggFile(std::istream* _file) : file(_file), reader(*file) {
    while (true) {
        std::string boxname(4, '\0');
        reader->readBypass(boxname);
        if (!reader->isGood()) {
            break;
        }
        UTILS_DIE_IF(boxes.empty() && boxname != "flhd", "No file header found");
        UTILS_DIE_IF(!boxes.empty() && boxname == "flhd", "Multiple file headers found");
        if (boxname == "flhd") {
            boxes.emplace_back(genie::util::make_unique<genie::format::mpegg_p1::FileHeader>(*reader));
        } else if (boxname == "dgcn") {
            const auto& hdr = dynamic_cast<const genie::format::mpegg_p1::FileHeader&>(*boxes.front());
            boxes.emplace_back(
                genie::util::make_unique<genie::format::mpegg_p1::DatasetGroup>(*reader, hdr.getMinorVersion()));
        } else {
            std::cout << "Unknown Box " << boxname << " on top level of file. Exit.";
            break;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::unique_ptr<Box>>& MggFile::getBoxes() { return boxes; }

// ---------------------------------------------------------------------------------------------------------------------

MggFile::MggFile() : file(nullptr), reader() {}

// ---------------------------------------------------------------------------------------------------------------------

void MggFile::addBox(std::unique_ptr<Box> box) { boxes.emplace_back(std::move(box)); }

// ---------------------------------------------------------------------------------------------------------------------

void MggFile::write(util::BitWriter& writer) {
    for (const auto& b : boxes) {
        b->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MggFile::print_debug(std::ostream& output, uint8_t max_depth) const {
    for (const auto& b : boxes) {
        b->print_debug(output, 0, max_depth);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
