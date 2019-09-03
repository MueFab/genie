#ifndef GENIE_CABAC_BINARIZATION_H
#define GENIE_CABAC_BINARIZATION_H

#include <memory>

#include "cabac_context_parameters.h"
#include "cabac_binarization_parameters.h"

class BitWriter;

/**
 * ISO 23092-2 Section 8.3.3 table
 */
class Cabac_binarization {
public:
    Cabac_binarization(const Cabac_binarization_parameters::Binarization_ID &_binarization_ID,
                       std::unique_ptr<Cabac_binarization_parameters> _cabac_binarization_parameters);

    Cabac_binarization();

    void setContextParameters(std::unique_ptr<Cabac_context_parameters> _cabac_context_parameters);

    virtual void write(BitWriter *writer) const;

    std::unique_ptr<Cabac_binarization> clone() const;

private:
    Cabac_binarization_parameters::Binarization_ID binarization_ID; //!< : 5; Line 2
    uint8_t bypass_flag : 1; //!< Line 3
    std::unique_ptr<Cabac_binarization_parameters> cabac_binarization_parameters; //!< Line 4
    std::unique_ptr<Cabac_context_parameters> cabac_context_parameters; //!< Line 6
};

#endif //GENIE_CABAC_BINARIZATION_H
