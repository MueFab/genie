#ifndef GENIE_NAME_DECODER_H
#define GENIE_NAME_DECODER_H

#include "access-unit-raw.h"

namespace genie {
namespace core {

class NameDecoder {
   public:
    virtual std::vector<std::string> process(AccessUnitRaw::Descriptor& desc) = 0;
    /**
     * @Brief For polymorphic destruction
     */
    ~NameDecoder() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

#endif  // GENIE_QV_DECODER_H
