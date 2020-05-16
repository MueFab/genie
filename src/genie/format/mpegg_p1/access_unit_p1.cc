/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "access_unit_p1.h"
namespace genie {
namespace format {
namespace mpegg_p1 {

AccessUnit::AccessUnit(genie::format::mgb::AccessUnit&& au_p2) : au_p1(std::move(au_p2)) {}

uint64_t AccessUnit::getLength() const {
    uint64_t length = 12;  // gen_info
    // TODO
    return length;
}

void AccessUnit::writeToFile(genie::util::BitWriter& bitWriter) const {
    bitWriter.write("aucn");

    bitWriter.write(this->getLength(), 64);
    // TODO

    bitWriter.flush();
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie