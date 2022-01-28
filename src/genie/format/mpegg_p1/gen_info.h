

#ifndef GENIE_GEN_INFO_H
#define GENIE_GEN_INFO_H

#include <string>
#include "genie/util/bitwriter.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class GenInfo {
 public:
    virtual const std::string& getKey() const = 0;
    virtual uint64_t getSize() const {return 4 + sizeof(uint64_t);}
    virtual void write(genie::util::BitWriter& bitWriter) const {
        bitWriter.writeBypass(getKey().data(), getKey().length());
        bitWriter.writeBypassBE<uint64_t>(getSize());
    }
    virtual ~GenInfo() = 0;
};

}
}
}

#endif  // GENIE_GEN_INFO_H
