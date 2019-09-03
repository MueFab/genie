#ifndef GENIE_CABAC_BINARIZATION_PARAMETERS_H
#define GENIE_CABAC_BINARIZATION_PARAMETERS_H

#include <memory>

class BitWriter;

/**
 * ISO 23092-2 Section 8.3.3.1 table 98
 */
class Cabac_binarization_parameters {
private:
    std::unique_ptr<uint8_t> cmax; //!< : 8; Line 3
    std::unique_ptr<uint8_t> cmax_teg; //!< : 8; Line 5
    std::unique_ptr<uint8_t> cmax_dtu; //!< : 8; Line 7
    std::unique_ptr<uint8_t> split_unit_size; //!< : 4; Line 10
public:

    enum class Binarization_ID : uint8_t {
        Binary_Coding = 0,
        Truncated_Unary = 1,
        Exponential_Golomb = 2,
        Signed_Exponential_Gomb = 3,
        Truncated_Exponential_Golomb = 4,
        Signed_Truncated_Exponential_Golomb = 5,
        Split_Unit_wise_Truncated_Unary = 6,
        Signed_Split_Unit_wise_Truncated_Unary = 7,
        Double_Truncated_Unary = 8,
        Signed_Double_Truncated_Unary = 9,
    };

    virtual void write(BitWriter *writer) const;

    Cabac_binarization_parameters();

    Cabac_binarization_parameters(const Binarization_ID &_binarization_id, uint8_t param);

    std::unique_ptr<Cabac_binarization_parameters> clone() const;
};

#endif //GENIE_CABAC_BINARIZATION_PARAMETERS_H
