/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "access_unit_p1.h"
namespace genie {
namespace format {
namespace mpegg_p1 {

AccessUnit::AccessUnit(genie::format::mgb::AccessUnit&& au_p2) : au_p1(std::move(au_p2)) { }
}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie