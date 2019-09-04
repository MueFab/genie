#ifndef GENIE_QV_CODING_CONFIG_1_H
#define GENIE_QV_CODING_CONFIG_1_H

// -----------------------------------------------------------------------------------------------------------------

#include "ureads-encoder/format/part2/qv_coding_config.h"
#include <memory>

// -----------------------------------------------------------------------------------------------------------------

namespace format {
    class BitWriter;
    namespace qv_coding1 {
        class ParameterSetQvps {
        private:
    /*      uint8_t qv_num_codebooks_total : 4;
            std::vector<QvCodebook> qv_codebooks;

        public:
            ParameterSetQvps();

            void addCodeBook(const QvCodebook &book); */
        public:
            virtual void write(BitWriter *writer) const;
        };

        // -----------------------------------------------------------------------------------------------------------------

        /**
        * ISO 23092-2 Section 3.3.2 table 7 lines 34 to 38
        */
        class QvCodingConfig1 : public QvCodingConfig {
        public:
            /**
            * ISO 23092-2 Section 3.3.2.2.1
            */
            enum class QvpsPresetId {
                ASCII = 0, //!< 3.3.2.2.1.1
                OFFSET33_RANGE41 = 1, //!< 3.3.2.2.1.2
                OFFSET64_RANGE40 = 2 //!< 3.3.2.2.1.3
            };

            QvCodingConfig1();

            explicit QvCodingConfig1(QvpsPresetId _qvps_preset_ID, bool _reverse_flag);

            void setQvps(std::unique_ptr<ParameterSetQvps> _parameter_set_qvps);

            void write(BitWriter *writer) const override;

        private:
            uint8_t qvps_flag : 1; //!< Line 34
            std::unique_ptr<ParameterSetQvps> parameter_set_qvps; //!< Line 36
            std::unique_ptr<QvpsPresetId> qvps_preset_ID; //!< : 4; Line 38
        };
    }
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_QV_CODING_CONFIG_1_H
