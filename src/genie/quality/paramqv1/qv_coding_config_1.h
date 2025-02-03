/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_PARAMQV1_QV_CODING_CONFIG_1_H_
#define SRC_GENIE_QUALITY_PARAMQV1_QV_CODING_CONFIG_1_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include "boost/optional/optional.hpp"
#include "genie/core/parameter/parameter_set.h"
#include "genie/core/parameter/quality-values.h"
#include "genie/quality/paramqv1/parameter_set.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace paramqv1 {

/**
 * @brief
 */
class QualityValues1 : public core::parameter::QualityValues {
 public:
    static const uint8_t MODE_QV1 = 1;  //!< @brief

    /**
     * @brief
     */
    enum class QvpsPresetId { ASCII = 0, OFFSET33_RANGE41 = 1, OFFSET64_RANGE40 = 2 };

    /**
     * @brief
     * @param desc
     * @param reader
     */
    explicit QualityValues1(genie::core::GenDesc desc, util::BitReader& reader);

    /**
     * @brief
     * @param _qvps_preset_ID
     * @param _reverse_flag
     */
    explicit QualityValues1(QvpsPresetId _qvps_preset_ID, bool _reverse_flag);

    /**
     * @brief
     * @param _parameter_set_qvps
     */
    void setQvps(ParameterSet&& _parameter_set_qvps);

    /**
     * @brief
     * @param writer
     */
    void write(util::BitWriter& writer) const override;

    /**
     * @brief
     * @return
     */
    std::unique_ptr<QualityValues> clone() const override;

    /**
     * @brief
     * @param desc
     * @param reader
     * @return
     */
    static std::unique_ptr<QualityValues> create(genie::core::GenDesc desc, util::BitReader& reader);

    /**
     * @brief
     * @param id
     * @return
     */
    static const Codebook& getPresetCodebook(QvpsPresetId id);

    /**
     * @brief
     * @param type
     * @return
     */
    static std::unique_ptr<core::parameter::QualityValues> getDefaultSet(core::record::ClassType type);

    /**
     * @brief
     * @return
     */
    size_t getNumberCodeBooks() const;

    /**
     * @brief
     * @param id
     * @return
     */
    const Codebook& getCodebook(size_t id) const;

    /**
     * @brief
     * @return
     */
    size_t getNumSubsequences() const override;

    /**
     * @brief
     * @param qv
     * @return
     */
    bool equals(const QualityValues* qv) const override;

 private:
    boost::optional<ParameterSet> parameter_set_qvps;  //!< @brief
    boost::optional<QvpsPresetId> qvps_preset_ID;      //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_PARAMQV1_QV_CODING_CONFIG_1_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
