#ifndef GENIE_DESCRIPTOR_CONFIGURATION_PRESENT_H
#define GENIE_DESCRIPTOR_CONFIGURATION_PRESENT_H

#include "descriptor_configuration.h"
#include "decoder_configuration.h"

#include <memory>

/**
* ISO 23092-2 Section 3.3.2.1 table 8, lines 4 to 9
*/
class Descriptor_configuration_present : public Descriptor_configuration {
private:
    std::unique_ptr<Decoder_configuration> decoder_configuration; //!< Line 5 to 9 (fused)
public:
    explicit Descriptor_configuration_present();

    std::unique_ptr<Descriptor_configuration> clone() const override;

    void write(BitWriter *writer) const override;

    void set_decoder_configuration(std::unique_ptr<Decoder_configuration> conf);

    void _deactivate(); // TODO: get rid of this
};

#endif //GENIE_DESCRIPTOR_CONFIGURATION_PRESENT_H

