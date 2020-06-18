/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_QV_CODING_CONFIG_1_H
#define GENIE_QV_CODING_CONFIG_1_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/parameter/parameter_set.h>
#include <genie/core/parameter/quality-values.h>
#include <genie/util/bitwriter.h>
#include <memory>
#include "parameter_set.h"

#include <boost/optional/optional.hpp>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace paramqv1 {

/**
 *
 */
class QualityValues1 : public core::parameter::QualityValues {
   public:
    static const uint8_t MODE_QV1 = 1; //!<

    /**
     *
     */
    enum class QvpsPresetId { ASCII = 0, OFFSET33_RANGE41 = 1, OFFSET64_RANGE40 = 2 };

    /**
     *
     * @param reader
     */
    explicit QualityValues1(util::BitReader& reader);

    /**
     *
     * @param _qvps_preset_ID
     * @param _reverse_flag
     */
    explicit QualityValues1(QvpsPresetId _qvps_preset_ID, bool _reverse_flag);

    /**
     *
     * @param _parameter_set_qvps
     */
    void setQvps(ParameterSet&& _parameter_set_qvps);

    /**
     *
     * @param writer
     */
    void write(util::BitWriter& writer) const override;

    /**
     *
     * @return
     */
    std::unique_ptr<QualityValues> clone() const override;

    /**
     *
     * @param reader
     * @return
     */
    static std::unique_ptr<QualityValues> create(util::BitReader& reader);

    /**
     *
     * @param id
     * @return
     */
    static const Codebook& getPresetCodebook(QvpsPresetId id);

    /**
     *
     * @param type
     * @return
     */
    static std::unique_ptr<core::parameter::QualityValues> getDefaultSet(core::record::ClassType type);

    /**
     *
     * @return
     */
    size_t getNumberCodeBooks() const;

    /**
     *
     * @param id
     * @return
     */
    const Codebook& getCodebook(size_t id) const;

    /**
     *
     * @return
     */
    size_t getNumSubsequences() const override;

   private:
    boost::optional<ParameterSet> parameter_set_qvps; //!<
    boost::optional<QvpsPresetId> qvps_preset_ID; //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_QV_CODING_CONFIG_1_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------