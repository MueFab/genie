#ifndef GENIE_CABAC_CONTEXT_PARAMETERS_H
#define GENIE_CABAC_CONTEXT_PARAMETERS_H

#include <memory>
#include <vector>

class BitWriter;

/**
 * ISO 23092-2 Section 8.3.3.2 table 99
 */
class Cabac_context_parameters {
private:
    uint8_t adaptive_mode_flag : 1; //!< Line 2
    uint16_t num_contexts : 16; //!< Line 3
    std::vector<uint8_t> context_initialization_value; //!< : 7; Lines 4-6
    std::unique_ptr<bool> share_subsym_ctx_flag; //!< : 1; Line 8
public:
    Cabac_context_parameters(bool adaptive_mode_flag, uint8_t coding_subsym_size, uint8_t output_symbol_size,
                             bool _share_subsym_ctx_flag);

    Cabac_context_parameters();

    void addContextInitializationValue(uint8_t _context_initialization_value);

    virtual void write(BitWriter *writer) const;

    std::unique_ptr<Cabac_context_parameters> clone() const;
};


#endif //GENIE_CABAC_CONTEXT_PARAMETERS_H
