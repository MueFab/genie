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

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace paramqv1 {

/**
 * ISO 23092-2 Section 3.3.2 table 7 lines 34 to 38
 */
class QvCodingConfig1 : public core::parameter::QualityValues {
   public:
    static const uint8_t MODE_QV1 = 1;

    /**
     * ISO 23092-2 Section 3.3.2.2.1
     */
    enum class QvpsPresetId {
        ASCII = 0,             //!< 3.3.2.2.1.1
        OFFSET33_RANGE41 = 1,  //!< 3.3.2.2.1.2
        OFFSET64_RANGE40 = 2   //!< 3.3.2.2.1.3
    };

    QvCodingConfig1();

    QvCodingConfig1(util::BitReader& reader);

    explicit QvCodingConfig1(QvpsPresetId _qvps_preset_ID, bool _reverse_flag);

    void setQvps(std::unique_ptr<ParameterSetQvps> _parameter_set_qvps);

    void write(util::BitWriter& writer) const override;

    virtual std::unique_ptr<QualityValues> clone() const;

    static std::unique_ptr<QualityValues> create(util::BitReader& reader) {
        return util::make_unique<QvCodingConfig1>(reader);
    }

   private:
    uint8_t qvps_flag : 1;                                 //!< Line 34
    std::unique_ptr<ParameterSetQvps> parameter_set_qvps;  //!< Line 36
    std::unique_ptr<QvpsPresetId> qvps_preset_ID;          //!< : 4; Line 38
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_QV_CODING_CONFIG_1_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------