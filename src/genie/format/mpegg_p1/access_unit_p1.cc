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

AU_information::AU_information() : AU_information_value() {
    AU_information_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                            0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                            0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
}

/**
 *
 */
AU_protection::AU_protection() : AU_protection_value() {
    AU_protection_value = {0x37, 0xfd, 0x58, 0x7a, 0x00, 0x5a, 0x04, 0x00, 0xd6, 0xe6, 0x46,
                           0xb4, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x1c, 0x21, 0x44, 0xb6, 0x1f,
                           0x7d, 0xf3, 0x00, 0x01, 0x00, 0x00, 0x04, 0x00, 0x5a, 0x59};
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie