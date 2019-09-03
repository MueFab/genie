#ifndef GENIE_DECODER_CONFIGURATION_CABAC_H
#define GENIE_DECODER_CONFIGURATION_CABAC_H

#include "decoder_configuration.h"

/**
 * Base for ISO 23092-2 Section 8.3.1 and ISO 23092-2 Section 8.3.5
 */
class Decoder_configuration_cabac : public Decoder_configuration {
protected:

public:
    Decoder_configuration_cabac();
};

#endif //GENIE_DECODER_CONFIGURATION_CABAC_H
