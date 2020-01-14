#ifndef GENIE_QV_CODING_CONFIG_1_H
#define GENIE_QV_CODING_CONFIG_1_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>
#include "mpegg_p2/parameter_set/qv_coding_config.h"
#include "parameter_set_qvps.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------
namespace genie {
namespace mpegg_p2 {

namespace qv_coding1 {
/**
 * ISO 23092-2 Section 3.3.2 table 7 lines 34 to 38
 */
class QvCodingConfig1 : public QvCodingConfig {
   public:
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

    virtual std::unique_ptr<QvCodingConfig> clone() const;

   private:
    uint8_t qvps_flag : 1;                                 //!< Line 34
    std::unique_ptr<ParameterSetQvps> parameter_set_qvps;  //!< Line 36
    std::unique_ptr<QvpsPresetId> qvps_preset_ID;          //!< : 4; Line 38
};
}  // namespace qv_coding1
}  // namespace mpegg_p2
}  // namespace genie
// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_QV_CODING_CONFIG_1_H
