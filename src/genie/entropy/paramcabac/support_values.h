/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_SUPPORT_VALUES_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_SUPPORT_VALUES_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

#include "nlohmann/json.hpp"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::paramcabac {

/**
 * @brief
 */
class SupportValues {
 public:
    /**
     * @brief
     */
    enum class TransformIdSubsym : uint8_t { NO_TRANSFORM = 0, LUT_TRANSFORM = 1, DIFF_CODING = 2 };

    /**
     * @brief
     */
    SupportValues();

    /**
     * @brief
     * @param _output_symbol_size
     * @param _coding_subsym_size
     * @param _coding_order
     * @param _share_subsym_lut_flag
     * @param _share_subsym_prv_flag
     */
    SupportValues(uint8_t _output_symbol_size, uint8_t _coding_subsym_size, uint8_t _coding_order,
                  bool _share_subsym_lut_flag = true, bool _share_subsym_prv_flag = true);

    /**
     * @brief
     * @param transformIdSubsym
     * @param reader
     */
    SupportValues(TransformIdSubsym transformIdSubsym, util::BitReader &reader);

    /**
     * @brief
     */
    virtual ~SupportValues() = default;

    /**
     * @brief
     * @param transformIdSubsym
     * @param writer
     */
    virtual void write(TransformIdSubsym transformIdSubsym, util::BitWriter &writer) const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getOutputSymbolSize() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getCodingSubsymSize() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t getCodingOrder() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool getShareSubsymLutFlag() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool getShareSubsymPrvFlag() const;

    /**
     * @brief
     * @param val
     * @return
     */
    bool operator==(const SupportValues &val) const;

    /**
     * @brief
     * @param j
     * @param transformIdSubsym
     */
    explicit SupportValues(nlohmann::json j, TransformIdSubsym transformIdSubsym);

    /**
     * @brief
     * @param transformIdSubsym
     * @return
     */
    [[nodiscard]] nlohmann::json toJson(TransformIdSubsym transformIdSubsym) const;

 private:
    uint8_t output_symbol_size;  //!< @brief
    uint8_t coding_subsym_size;  //!< @brief
    uint8_t coding_order;        //!< @brief
    bool share_subsym_lut_flag;  //!< @brief
    bool share_subsym_prv_flag;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::paramcabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_SUPPORT_VALUES_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
