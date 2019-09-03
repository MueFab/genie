#include "qv_coding_config_1.h"
#include "make_unique.h"
#include "bitwriter.h"

Qv_coding_config_1::Qv_coding_config_1() : Qv_coding_config_1(Qvps_preset_ID::ASCII, false) {
}

Qv_coding_config_1::Qv_coding_config_1(Qvps_preset_ID _qvps_preset_ID, bool _reverse_flag) : Qv_coding_config(
        Qv_coding_mode::ONE, _reverse_flag), qvps_flag(false), parameter_set_qvps(nullptr) {
    qvps_preset_ID = make_unique<Qvps_preset_ID>(_qvps_preset_ID);
}

void Qv_coding_config_1::setQvps(std::unique_ptr<Parameter_set_qvps> _parameter_set_qvps) {
    qvps_flag = true;
    qvps_preset_ID = nullptr;
    parameter_set_qvps = std::move(_parameter_set_qvps);
}

void Qv_coding_config_1::write(BitWriter *writer) const {
    writer->write(qvps_flag, 1);
    if (parameter_set_qvps) {
        parameter_set_qvps->write(writer);
    }
    if (qvps_preset_ID) {
        writer->write(uint64_t(*qvps_preset_ID), 4);
    }
    writer->write(qv_reverse_flag, 1);
}