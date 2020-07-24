#include "genie/util/exception.h"
#include "genie/util/runtime-exception.h"

#include "md5.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

Md5::Md5(): Checksum(Checksum::Algo::MD5) {}

Md5::Md5(std::vector<uint64_t>& _data): Md5() {
    UTILS_DIE_IF(_data.size() != _data.size(), "Invalid data length");

    for (size_t i=0; i < _data.size(); i++){
        data[i] = _data[i];
    }
}

void Md5::write(util::BitWriter& bit_writer) const {
    for (auto c: data){
        bit_writer.write(c, 64);
    }
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie
