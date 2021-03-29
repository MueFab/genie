/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_SUPPORT_VALUES_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_SUPPORT_VALUES_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace paramcabac {

/**
 * ISO 23092-2 Section 8.3.2 table
 */
class SupportValues {
 public:
    /**
     *
     */
    enum class TransformIdSubsym : uint8_t { NO_TRANSFORM = 0, LUT_TRANSFORM = 1, DIFF_CODING = 2 };

    /**
     *
     */
    SupportValues();

    /**
     *
     * @param _output_symbol_size
     * @param _coding_subsym_size
     * @param _coding_order
     * @param _share_subsym_lut_flag
     * @param _share_subsym_prv_flag
     */
    SupportValues(uint8_t _output_symbol_size, uint8_t _coding_subsym_size, uint8_t _coding_order,
                  bool _share_subsym_lut_flag = true, bool _share_subsym_prv_flag = true);

    /**
     *
     * @param transformIdSubsym
     * @param reader
     */
    SupportValues(TransformIdSubsym transformIdSubsym, util::BitReader &reader);

    /**
     *
     */
    virtual ~SupportValues() = default;

    /**
     *
     * @param transformIdSubsym
     * @param writer
     */
    virtual void write(TransformIdSubsym transformIdSubsym, util::BitWriter &writer) const;

    /**
     *
     * @return
     */
    uint8_t getOutputSymbolSize() const;

    /**
     *
     * @return
     */
    uint8_t getCodingSubsymSize() const;

    /**
     *
     * @return
     */
    uint8_t getCodingOrder() const;

    /**
     *
     * @return
     */
    bool getShareSubsymLutFlag() const;

    /**
     *
     * @return
     */
    bool getShareSubsymPrvFlag() const;

    bool operator==(const SupportValues &val) const {
        return output_symbol_size == val.output_symbol_size && coding_subsym_size == val.coding_subsym_size &&
               coding_order == val.coding_order && share_subsym_lut_flag == val.share_subsym_lut_flag &&
               share_subsym_prv_flag == val.share_subsym_prv_flag;
    }

 private:
    uint8_t output_symbol_size;  //!<
    uint8_t coding_subsym_size;  //!<
    uint8_t coding_order;        //!<
    bool share_subsym_lut_flag;  //!<
    bool share_subsym_prv_flag;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramcabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_SUPPORT_VALUES_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
