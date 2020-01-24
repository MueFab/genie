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

QualityValues1::QualityValues1(QvpsPresetId _qvps_preset_ID, bool _reverse_flag)
    : QualityValues(MODE_QV1, _reverse_flag), parameter_set_qvps() {
    qvps_preset_ID = _qvps_preset_ID;
}

// ---------------------------------------------------------------------------------------------------------------------

QualityValues1::QualityValues1(util::BitReader& reader) : QualityValues(MODE_QV1, false) {
    auto qvps_flag = reader.read<bool>(1);
    if (qvps_flag) {
        parameter_set_qvps = ParameterSet(reader);
    } else {
        qvps_preset_ID = reader.read<QvpsPresetId>(4);
    }
    qv_reverse_flag = reader.read<bool>(1);
}

// ---------------------------------------------------------------------------------------------------------------------

void QualityValues1::setQvps(ParameterSet&& _parameter_set_qvps) {
    qvps_preset_ID.reset();
    parameter_set_qvps = std::move(_parameter_set_qvps);
}

// ---------------------------------------------------------------------------------------------------------------------

void QualityValues1::write(util::BitWriter& writer) const {
    writer.write(uint8_t(qv_coding_mode), 4);
    writer.write((bool)parameter_set_qvps, 1);
    if (parameter_set_qvps) {
        parameter_set_qvps->write(writer);
    }
    if (qvps_preset_ID) {
        writer.write(uint64_t(*qvps_preset_ID), 4);
    }
    writer.write(qv_reverse_flag, 1);
}

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<core::parameter::QualityValues> QualityValues1::clone() const {
    auto ret = util::make_unique<QualityValues1>(
        qvps_preset_ID ? qvps_preset_ID.get() : QualityValues1::QvpsPresetId ::ASCII, qv_reverse_flag);
    if (parameter_set_qvps) {
        auto qvps = parameter_set_qvps;
        ret->setQvps(std::move(qvps.get()));
    }
    ret->qvps_preset_ID = qvps_preset_ID;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------