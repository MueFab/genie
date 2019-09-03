#ifndef GENIE_SUPPORT_VALUES_H
#define GENIE_SUPPORT_VALUES_H

#include <memory>

class BitWriter;

/**
 * ISO 23092-2 Section 8.3.2 table
 */
class Support_values {
private:
    uint8_t output_symbol_size : 6; //!< Line 2
    uint8_t coding_subsym_size : 6; //!< Line 3
    uint8_t coding_order : 2; //!< Line 4
    std::unique_ptr<bool> share_subsym_lut_flag; //!< : 1; Line 7
    std::unique_ptr<bool> share_subsym_prv_flag; //!< : 1; Line 8
public:
    enum class Transform_ID_subsym : uint8_t {
        no_transform = 0,
        lut_transform = 1,
        diff_coding = 2
    };

    Support_values(uint8_t _output_symbol_size,
                   uint8_t _coding_subsym_size,
                   uint8_t _coding_order,
                   const Transform_ID_subsym &_transform_ID_subsym,
                   bool _share_subsym_prv_flag = true,
                   bool _share_subsym_lut_flag = true);

    Support_values();

    std::unique_ptr<Support_values> clone() const;

    virtual void write(BitWriter *writer) const;
};

#endif //GENIE_SUPPORT_VALUES_H
