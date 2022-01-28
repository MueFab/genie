/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/file_header.h"
#include <utility>
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

const std::string& FileHeader::getMajorBrand() const { return major_brand; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& FileHeader::getMinorVersion() const { return minor_version; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::string>& FileHeader::getCompatibleBrands() const { return compatible_brands; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
