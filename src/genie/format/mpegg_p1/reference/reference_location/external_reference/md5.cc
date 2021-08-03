/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference/reference_location/external_reference/md5.h"
#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Md5::Md5() : Checksum(Checksum::Algo::MD5) {}

// ---------------------------------------------------------------------------------------------------------------------

Md5::Md5(util::BitReader& reader) : Md5() {
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = reader.read<uint64_t>();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Md5::Md5(std::vector<uint64_t>& _data) : Md5() {
    UTILS_DIE_IF(_data.size() != _data.size(), "Invalid data length");

    for (size_t i = 0; i < _data.size(); i++) {
        data[i] = _data[i];
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Checksum> Md5::clone() const {
    auto ret = util::make_unique<Md5>();
    ret->data = this->data;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Md5::getLength() const {
    // 128 bits
    return 16;
}

// ---------------------------------------------------------------------------------------------------------------------

void Md5::write(util::BitWriter& writer) const {
    for (auto c : data) {
        writer.write(c, 64);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
