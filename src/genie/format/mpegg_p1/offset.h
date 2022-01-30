

#ifndef GENIE_OFFSET_H
#define GENIE_OFFSET_H

#include <string>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"


namespace genie {
namespace format {
namespace mpegg_p1 {

class Offset {
 private:
    std::string subkey;
    uint64_t offset;
 public:
    explicit Offset(util::BitReader& bitReader) : subkey(4, '\0'){
        bitReader.readBypass(subkey);
        offset = bitReader.readBypassBE<uint64_t>();
    }

    void write(util::BitWriter& writer) const {
        writer.writeBypass("offs", 4);
        writer.writeBypass(subkey.data(), 4);
        writer.writeBypassBE(offset);
    }

    const std::string& getSubkey() const {
        return subkey;
    }

    uint64_t getOffset() const {
        return offset;
    }
};

}
}  // namespace format
}  // namespace genie

#endif  // GENIE_OFFSET_H
