/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/reference/reference_location/external_reference/sha256.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Sha256::Sha256(): Checksum(Checksum::Algo::SHA256),
                  data(){}

// ---------------------------------------------------------------------------------------------------------------------

Sha256::Sha256(util::BitReader &reader) : Sha256() {
    /// SHA256 u(256)
    for (size_t i = 0; i < 4; i++) {
        data.emplace_back(reader.read<uint64_t>());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

//Sha256::Sha256(std::vector<uint64_t> &_data) : Sha256() {
//    UTILS_DIE_IF(_data.size() != 4data.emplace_back(reader.read<uint64_t>());, "Invalid data length");
//
//    for (size_t i = 0; i < _data.size(); i++) {
//        data.emplace_back(_data[i]);
//    }
//}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<Checksum> Sha256::clone() const {
    auto ret = util::make_unique<Sha256>();
    ret->data = this->data;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Sha256::getLength() const {
    /// SHA256 u(256)
    return 256/8;
}

// ---------------------------------------------------------------------------------------------------------------------

void Sha256::write(util::BitWriter &bit_writer) const {
    for (auto v : data) {
        bit_writer.write(v, 64);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
