#include "file_header.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

FileHeader::FileHeader(std::vector<std::string>* const x_compatible_brand) {
    compatible_brand = std::move(*x_compatible_brand);
}

void FileHeader::writeToFile(genie::util::BitWriter& bitWriter) const {
    (void)bitWriter;  // silence compiler warnings
    // TODO
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie