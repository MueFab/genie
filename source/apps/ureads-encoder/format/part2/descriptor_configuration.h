#ifndef GENIE_DESCRIPTOR_CONFIGURATION_H
#define GENIE_DESCRIPTOR_CONFIGURATION_H

#include <memory>

class BitWriter;

/**
* ISO 23092-2 Section 3.3.2.1 table 8
*/
class Descriptor_configuration {
public:
    enum class Dec_cfg_preset : uint8_t {
        PRESENT = 0 //!< See text in section 3.3.2.1
    };

    explicit Descriptor_configuration(Dec_cfg_preset _dec_cfg_preset);

    virtual std::unique_ptr<Descriptor_configuration> clone() const = 0;

    virtual void write(BitWriter *writer) const;

    virtual ~Descriptor_configuration() = default;

protected:
    Dec_cfg_preset dec_cfg_preset : 8; //!< Line 2
};

#endif //GENIE_DESCRIPTOR_CONFIGURATION_H
