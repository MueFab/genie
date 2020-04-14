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

class QualityValues1 : public core::parameter::QualityValues {
   public:
    static const uint8_t MODE_QV1 = 1;

    enum class QvpsPresetId { ASCII = 0, OFFSET33_RANGE41 = 1, OFFSET64_RANGE40 = 2 };

    explicit QualityValues1(util::BitReader& reader);

    explicit QualityValues1(QvpsPresetId _qvps_preset_ID, bool _reverse_flag);

    void setQvps(ParameterSet&& _parameter_set_qvps);

    void write(util::BitWriter& writer) const override;

    std::unique_ptr<QualityValues> clone() const override;

    static std::unique_ptr<QualityValues> create(util::BitReader& reader) {
        return util::make_unique<QualityValues1>(reader);
    }

    static const Codebook& getPresetCodebook(QvpsPresetId id) {
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

    static std::unique_ptr<core::parameter::QualityValues> getDefaultSet(core::record::ClassType type) {
        auto ret = util::make_unique<paramqv1::QualityValues1>(QvpsPresetId::ASCII, false);
        ParameterSet set;

        auto codebook = QualityValues1::getPresetCodebook(paramqv1::QualityValues1::QvpsPresetId::ASCII);
        set.addCodeBook(std::move(codebook));

        if(type == core::record::ClassType::CLASS_I || type == core::record::ClassType::CLASS_HM) {
            codebook = paramqv1::QualityValues1::getPresetCodebook(paramqv1::QualityValues1::QvpsPresetId::ASCII);
            set.addCodeBook(std::move(codebook));
        }
        ret->setQvps(std::move(set));
        return ret;
    }

    size_t getNumberCodeBooks() const {
        if (qvps_preset_ID.is_initialized()) {
            return 1;
        }
        return parameter_set_qvps->getCodebooks().size();
    }

    const Codebook& getCodebook(size_t id) const {
        if (qvps_preset_ID.is_initialized()) {
            UTILS_DIE_IF(id > 0, "Codebook out of bounds");
            return getPresetCodebook(qvps_preset_ID.get());
        }
        return parameter_set_qvps->getCodebooks()[id];
    }

    size_t getNumSubsequences() const override { return getNumberCodeBooks() + 2; }

   private:
    boost::optional<ParameterSet> parameter_set_qvps;
    boost::optional<QvpsPresetId> qvps_preset_ID;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace paramqv1
}  // namespace quality
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_QV_CODING_CONFIG_1_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------