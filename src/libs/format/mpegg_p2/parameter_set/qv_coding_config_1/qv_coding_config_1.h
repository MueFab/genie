#ifndef GENIE_QV_CODING_CONFIG_1_H
#define GENIE_QV_CODING_CONFIG_1_H

// -----------------------------------------------------------------------------------------------------------------

#include <memory>
#include "../qv_coding_config.h"
#include "parameter_set_qvps.h"
#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
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

        QvCodingConfig1(util::BitReader& reader) : QvCodingConfig(QvCodingMode::ONE, false ){
            qvps_flag = reader.read(1);
            if(qvps_flag) {
                UTILS_DIE("Parameter-set-qvps not supported yet");
            } else {
                qvps_preset_ID = util::make_unique<QvpsPresetId>(QvpsPresetId(reader.read(4)));
            }
            qv_reverse_flag = reader.read(1);
        }

        explicit QvCodingConfig1(QvpsPresetId _qvps_preset_ID, bool _reverse_flag);

        void setQvps(std::unique_ptr<ParameterSetQvps> _parameter_set_qvps);

        void write(util::BitWriter &writer) const override;

        virtual std::unique_ptr<QvCodingConfig> clone() const {
            auto ret = util::make_unique<QvCodingConfig1>();
            ret->qv_coding_mode = qv_coding_mode;
            ret->qv_reverse_flag = qv_reverse_flag;
            ret->qvps_flag = qvps_flag;
            ret->parameter_set_qvps = parameter_set_qvps->clone();
            ret->qvps_preset_ID = util::make_unique<QvpsPresetId>(*qvps_preset_ID);
            return ret;
        }

    private:
        uint8_t qvps_flag : 1;                                 //!< Line 34
        std::unique_ptr<ParameterSetQvps> parameter_set_qvps;  //!< Line 36
        std::unique_ptr<QvpsPresetId> qvps_preset_ID;          //!< : 4; Line 38
    };
}
}  // namespace qv_coding1
}  // namespace format

// -----------------------------------------------------------------------------------------------------------------

#endif  // GENIE_QV_CODING_CONFIG_1_H
