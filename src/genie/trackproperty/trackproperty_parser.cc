/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/trackproperty/trackproperty_parser.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace track_property {

// ---------------------------------------------------------------------------------------------------------------------

void TrackPropertyParser::processRecord(core::record::track_property::Record& rec) {
    descriptors.write(rec);
    
    // Track properties are stored as attributes in the annotation parameter set
    const auto& props = rec.GetProperties();
    if (!props.empty()) {
        attributes.add(props);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace track_property
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
