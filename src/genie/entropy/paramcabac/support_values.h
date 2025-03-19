/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_PARAMCABAC_SUPPORT_VALUES_H_
#define SRC_GENIE_ENTROPY_PARAMCABAC_SUPPORT_VALUES_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

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
    [[nodiscard]] uint8_t GetOutputSymbolSize() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t GetCodingSubsymSize() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] uint8_t GetCodingOrder() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool GetShareSubsymLutFlag() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool GetShareSubsymPrvFlag() const;

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
    [[nodiscard]] nlohmann::json ToJson(TransformIdSubsym transformIdSubsym) const;

 private:
    uint8_t output_symbol_size_;  //!< @brief
    uint8_t coding_subsym_size_;  //!< @brief
    uint8_t coding_order_;        //!< @brief
    bool share_subsym_lut_flag_;  //!< @brief
    bool share_subsym_prv_flag_;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

} // namespace genie::entropy::paramcabac



// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_PARAMCABAC_SUPPORT_VALUES_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
