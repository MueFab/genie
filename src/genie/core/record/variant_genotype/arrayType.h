#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

namespace genie {
namespace core {
namespace record {
namespace variant_genotype {

class arrayType {
 private:

 public:
    std::vector<uint8_t> toArray(uint8_t type, util::BitReader& reader);
    void toFile(uint8_t type, std::vector<uint8_t> bytearray, util::BitWriter& writer) const;
    std::string toString(uint8_t type, std::vector<uint8_t> value) const;
};

}  // namespace variant_genotype
}  // namespace record
}  // namespace core
}  // namespace genie
