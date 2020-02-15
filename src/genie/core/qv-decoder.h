#ifndef GENIE_QV_DECODER_H
#define GENIE_QV_DECODER_H

#include "access-unit-raw.h"

namespace genie {
namespace core {

class QVDecoder {
   public:
    virtual std::string decode(const parameter::QualityValues& param, const std::string& ecigar,
                               AccessUnitRaw::Descriptor& desc) = 0;
    /**
     * @Brief For polymorphic destruction
     */
    ~QVDecoder() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

#endif  // GENIE_QV_DECODER_H
