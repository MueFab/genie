/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SUPPORT_VALUES_H
#define GENIE_SUPPORT_VALUES_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 * ISO 23092-2 Section 8.3.2 table
 */
class SupportValues {
   public:
    enum class TransformIdSubsym : uint8_t {
        NO_TRANSFORM = 0,
        LUT_TRANSFORM = 1,
        DIFF_CODING = 2
    };

    SupportValues();

    SupportValues(uint8_t _output_symbol_size, uint8_t _coding_subsym_size, uint8_t _coding_order,
                  bool _share_subsym_lut_flag = true, bool _share_subsym_prv_flag = true);

    SupportValues(TransformIdSubsym transformIdSubsym, util::BitReader &reader);

    virtual ~SupportValues() = default;

    virtual void write(TransformIdSubsym transformIdSubsym, util::BitWriter &writer) const;

    uint8_t getOutputSymbolSize() const;

    uint8_t getCodingSubsymSize() const;

    uint8_t getCodingOrder() const;

    bool getShareSubsymLutFlag() const;

    bool getShareSubsymPrvFlag() const;

   private:
    uint8_t output_symbol_size;
    uint8_t coding_subsym_size;
    uint8_t coding_order;
    bool share_subsym_lut_flag : true;
    bool share_subsym_prv_flag : true;

};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_SUPPORT_VALUES_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
