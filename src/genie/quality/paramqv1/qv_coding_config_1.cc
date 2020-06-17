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

std::unique_ptr<QualityValues1::QualityValues> QualityValues1::create(util::BitReader& reader) {
    return util::make_unique<QualityValues1>(reader);
}

const Codebook& QualityValues1::getPresetCodebook(QvpsPresetId id) {
    const static std::vector<Codebook> pSet = []() -> std::vector<Codebook> {
        std::vector<Codebook> ret;
        Codebook set(33, 34);
        for (uint8_t p = 2; p < 94; ++p) {
            set.addEntry(p + 33);
        }
        ret.emplace_back(std::move(set));

        set = Codebook(33, 41);
        for (uint8_t p = 46; p <= 66; p += 5) {
            set.addEntry(p);
        }
        set.addEntry(74);
        ret.emplace_back(std::move(set));

        set = Codebook(64, 72);
        for (uint8_t p = 77; p <= 97; p += 5) {
            set.addEntry(p);
        }
        set.addEntry(104);
        ret.emplace_back(std::move(set));
        return ret;
    }();
    return pSet[uint8_t(id)];
}

std::unique_ptr<core::parameter::QualityValues> QualityValues1::getDefaultSet(core::record::ClassType type) {
    auto ret = util::make_unique<paramqv1::QualityValues1>(QvpsPresetId::ASCII, false);
    ParameterSet set;

    auto codebook = QualityValues1::getPresetCodebook(paramqv1::QualityValues1::QvpsPresetId::ASCII);
    set.addCodeBook(std::move(codebook));

    if (type == core::record::ClassType::CLASS_I || type == core::record::ClassType::CLASS_HM) {
        codebook = paramqv1::QualityValues1::getPresetCodebook(paramqv1::QualityValues1::QvpsPresetId::ASCII);
        set.addCodeBook(std::move(codebook));
    }
    ret->setQvps(std::move(set));
    return ret;
}

size_t QualityValues1::getNumberCodeBooks() const {
    if (qvps_preset_ID.is_initialized()) {
        return 1;
    }
    return parameter_set_qvps->getCodebooks().size();
}

const Codebook& QualityValues1::getCodebook(size_t id) const {
    if (qvps_preset_ID.is_initialized()) {
        UTILS_DIE_IF(id > 0, "Codebook out of bounds");
        return getPresetCodebook(qvps_preset_ID.get());
    }
    return parameter_set_qvps->getCodebooks()[id];
}

size_t QualityValues1::getNumSubsequences() const { return getNumberCodeBooks() + 2; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------