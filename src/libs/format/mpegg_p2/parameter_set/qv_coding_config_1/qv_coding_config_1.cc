#include "qv_coding_config_1.h"
#include "../../make_unique.h"
#include "util/bitwriter.h"

namespace format {
namespace qv_coding1 {
QvCodingConfig1::QvCodingConfig1() : QvCodingConfig1(QvpsPresetId::ASCII, false) {}

// -----------------------------------------------------------------------------------------------------------------

QvCodingConfig1::QvCodingConfig1(QvpsPresetId _qvps_preset_ID, bool _reverse_flag)
    : QvCodingConfig(QvCodingMode::ONE, _reverse_flag), qvps_flag(false), parameter_set_qvps(nullptr) {
    qvps_preset_ID = make_unique<QvpsPresetId>(_qvps_preset_ID);
}

// -----------------------------------------------------------------------------------------------------------------

void QvCodingConfig1::setQvps(std::unique_ptr<ParameterSetQvps> _parameter_set_qvps) {
    qvps_flag = true;
    qvps_preset_ID = nullptr;
    parameter_set_qvps = std::move(_parameter_set_qvps);
}

// -----------------------------------------------------------------------------------------------------------------

void QvCodingConfig1::write(util::BitWriter *writer) const {
    writer->write(uint8_t(qv_coding_mode), 4);
    writer->write(qvps_flag, 1);
    if (parameter_set_qvps) {
        parameter_set_qvps->write(writer);
    }
    if (qvps_preset_ID) {
        writer->write(uint64_t(*qvps_preset_ID), 4);
    }
    writer->write(qv_reverse_flag, 1);
}
}  // namespace qv_coding1
}  // namespace format
