/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/encapsulator/decapsulated_file.h"
#include <iostream>
#include <string>
#include <vector>
#include "genie/format/mgg/mgg_file.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::encapsulator {

// ---------------------------------------------------------------------------------------------------------------------

DecapsulatedFile::DecapsulatedFile(const std::string& input_file) : reader(input_file), mpegg_file(&reader) {
    mpegg_file.print_debug(std::cout, 2);

    for (auto& box : mpegg_file.getBoxes()) {
        auto* grp = dynamic_cast<genie::format::mgg::DatasetGroup*>(box.get());
        if (!grp) {
            continue;
        }

        groups.emplace_back(grp);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<DecapsulatedDatasetGroup>& DecapsulatedFile::getGroups() { return groups; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
