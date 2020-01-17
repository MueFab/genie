#ifndef GENIE_DECODER_REGULAR_H
#define GENIE_DECODER_REGULAR_H

#include "decoder.h"

namespace genie {
namespace core {
namespace parameter {
namespace desc_pres {

class DecoderRegular : public Decoder {
   public:
    explicit DecoderRegular(uint8_t _encoding_mode_id) : Decoder(_encoding_mode_id) {

    }
    ~DecoderRegular() override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace desc_pres
}  // namespace parameter
}  // namespace core
}  // namespace genie


#endif  // GENIE_DECODER_REGULAR_H
