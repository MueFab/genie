#ifndef GENIE_DECODER_CONFIGURATION_H
#define GENIE_DECODER_CONFIGURATION_H

#include "descriptor_subsequence_cfg.h"

#include <memory>

class BitWriter;

/**
 * ISO 23092-2 Section 3.3.2.1 table 8 lines 4 to 8
 */
class Decoder_configuration {
public:
    enum class Encoding_mode_ID : uint8_t {
        CABAC = 0 //!< See Text in section
    };

    virtual void write(BitWriter *writer) const;

    virtual void addSubsequenceCfg(std::unique_ptr<Descriptor_subsequence_cfg> cfg) = 0;

    virtual std::unique_ptr<Decoder_configuration> clone() const = 0;

    Decoder_configuration(Encoding_mode_ID _encoding_mode_id);

    virtual ~Decoder_configuration() = default;

protected:
    Encoding_mode_ID encoding_mode_ID; //!< : 8; Line 4
};

#endif //GENIE_DECODER_CONFIGURATION_H
