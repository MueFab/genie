#ifndef GENIE_DECODER_TOKENTYPE_H
#define GENIE_DECODER_TOKENTYPE_H

#include "decoder.h"

namespace genie {
namespace core {
namespace parameter {
namespace desc_pres {

class DecoderTokentype : public Decoder {
   public:
    explicit DecoderTokentype(uint8_t _encoding_mode_id) : Decoder(_encoding_mode_id) {

    }
    virtual ~DecoderTokentype() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace desc_pres
}  // namespace parameter
}  // namespace core
}  // namespace genie


#endif  // GENIE_DECODER_TOKENTYPE_H
