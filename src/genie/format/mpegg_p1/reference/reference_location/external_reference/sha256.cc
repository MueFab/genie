#include "genie/util/runtime-exception.h"
#include "sha256.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

Sha256::Sha256(): Checksum(Checksum::Algo::SHA256) {}

Sha256::Sha256(std::vector<uint64_t> &_data): Sha256(){
    UTILS_DIE_IF(_data.size() != _data.size(), "Invalid data length");

    for (size_t i=0; i<_data.size(); i++){
        data[i] = _data[i];
    }
}
void Sha256::write(util::BitWriter &bit_writer) const {
    for (auto v: data){
        bit_writer.write(v, 64);
    }
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie