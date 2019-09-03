#ifndef GENIE_DECODER_CONFIGURATION_CABAC_REGULAR_H
#define GENIE_DECODER_CONFIGURATION_CABAC_REGULAR_H

#include <memory>

#include "decoder_configuration.h"
#include "decoder_configuration_cabac.h"

/**
 * ISO 23092-2 Section 8.3.1 table line 3 to 13
 */
class Decoder_configuration_cabac_regular : public Decoder_configuration_cabac {
private:
    uint8_t num_descriptor_subsequence_cfgs_minus1 : 8; //!< Line 3
    std::vector<std::unique_ptr<Descriptor_subsequence_cfg>> descriptor_subsequence_cfgs; //!< Line 4 to 13
public:
    Decoder_configuration_cabac_regular();

    void addSubsequenceCfg(std::unique_ptr<Descriptor_subsequence_cfg> cfg) override;

    std::unique_ptr<Decoder_configuration> clone() const override;

    void write(BitWriter *writer) const override;
};


#endif //GENIE_DECODER_CONFIGURATION_CABAC_REGULAR_H
