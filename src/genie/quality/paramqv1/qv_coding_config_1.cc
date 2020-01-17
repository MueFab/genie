/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "qv_coding_config_1.h"
#include <genie/util/bitwriter.h>
#include <genie/util/exceptions.h>
#include <genie/util/make-unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace paramqv1 {

// ---------------------------------------------------------------------------------------------------------------------

QvCodingConfig1::QvCodingConfig1() : QvCodingConfig1(QvpsPresetId::ASCII, false) {}

// ---------------------------------------------------------------------------------------------------------------------

QvCodingConfig1::QvCodingConfig1(QvpsPresetId _qvps_preset_ID, bool _reverse_flag)
    : QualityValues(MODE_QV1, _reverse_flag), qvps_flag(false), parameter_set_qvps(nullptr) {
    qvps_preset_ID = util::make_unique<QvpsPresetId>(_qvps_preset_ID);
}

// ---------------------------------------------------------------------------------------------------------------------

QvCodingConfig1::QvCodingConfig1(util::BitReader& reader) : QualityValues(MODE_QV1, false) {
    qvps_flag = reader.read(1);
    if (qvps_flag) {
        UTILS_DIE("Parameter-set-qvps not supported yet");
    } else {
        qvps_preset_ID = util::make_unique<QvpsPresetId>(QvpsPresetId(reader.read(4)));
    }
    qv_reverse_flag = reader.read(1);
}

// ---------------------------------------------------------------------------------------------------------------------

void QvCodingConfig1::setQvps(std::unique_ptr<ParameterSetQvps> _parameter_set_qvps) {
    qvps_flag = true;
    qvps_preset_ID = nullptr;
    parameter_set_qvps = std::move(_parameter_set_qvps);
}

// ---------------------------------------------------------------------------------------------------------------------

void QvCodingConfig1::write(util::BitWriter& writer) const {
    writer.write(uint8_t(qv_coding_mode), 4);
    writer.write(qvps_flag, 1);
    if (parameter_set_qvps) {
        parameter_set_qvps->write(writer);
    }
    if (qvps_preset_ID) {
        writer.write(uint64_t(*qvps_preset_ID), 4);
    }
    writer.write(qv_reverse_flag, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<core::parameter::QualityValues> QvCodingConfig1::clone() const {
    auto ret = util::make_unique<QvCodingConfig1>();
    ret->qv_coding_mode = qv_coding_mode;
    ret->qv_reverse_flag = qv_reverse_flag;
    ret->qvps_flag = qvps_flag;
    if (parameter_set_qvps) {
        ret->parameter_set_qvps = parameter_set_qvps->clone();
    }
    ret->qvps_preset_ID = util::make_unique<QvpsPresetId>(*qvps_preset_ID);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------