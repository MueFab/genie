#ifndef GENIE_SUPPORT_VALUES_H
#define GENIE_SUPPORT_VALUES_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>

// -----------------------------------------------------------------------------------------------------------------

namespace format {
class BitWriter;
namespace desc_conf_pres {
namespace cabac {

/**
 * ISO 23092-2 Section 8.3.2 table
 */
class SupportValues {
   private:
    uint8_t output_symbol_size : 6;               //!< Line 2
    uint8_t coding_subsym_size : 6;               //!< Line 3
    uint8_t coding_order : 2;                     //!< Line 4
    std::unique_ptr<bool> share_subsym_lut_flag;  //!< : 1; Line 7
    std::unique_ptr<bool> share_subsym_prv_flag;  //!< : 1; Line 8
   public:
    enum class TransformIdSubsym : uint8_t { NO_TRANSFORM = 0, LUT_TRANSFORM = 1, DIFF_CODING = 2 };

    SupportValues(uint8_t _output_symbol_size, uint8_t _coding_subsym_size, uint8_t _coding_order,
                  const TransformIdSubsym &_transform_ID_subsym, bool _share_subsym_prv_flag = true,
                  bool _share_subsym_lut_flag = true);

    SupportValues();

    std::unique_ptr<SupportValues> clone() const;

    virtual void write(BitWriter *writer) const;
};
}  // namespace cabac
}  // namespace desc_conf_pres
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SUPPORT_VALUES_H
