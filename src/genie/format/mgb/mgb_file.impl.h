/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_MGB_FILE_IMPL_H_
#define SRC_GENIE_FORMAT_MGB_MGB_FILE_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgb {

// ---------------------------------------------------------------------------------------------------------------------

template <typename LAMBDA>
bool MgbFile::base_sorter(const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u1,
                          const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u2,
                          LAMBDA lambda) {
    if (data_unit_order(*u1.second) != data_unit_order(*u2.second)) {
        return data_unit_order(*u1.second) < data_unit_order(*u2.second);
    }
    if (u1.second->getDataUnitType() == genie::core::parameter::DataUnit::DataUnitType::PARAMETER_SET) {
        return dynamic_cast<const genie::core::parameter::ParameterSet&>(*u1.second).getID() <
               dynamic_cast<const genie::core::parameter::ParameterSet&>(*u2.second).getID();
    }
    if (u1.second->getDataUnitType() == genie::core::parameter::DataUnit::DataUnitType::RAW_REFERENCE) {
        const auto& r1 = dynamic_cast<const genie::format::mgb::RawReference&>(*u1.second);
        const auto& r2 = dynamic_cast<const genie::format::mgb::RawReference&>(*u2.second);
        if (r1.begin()->getSeqID() != r2.begin()->getSeqID()) {
            return r1.begin()->getSeqID() < r2.begin()->getSeqID();
        }
        if (r1.begin()->getStart() != r2.begin()->getStart()) {
            return r1.begin()->getStart() < r2.begin()->getStart();
        }
        return r1.begin()->getEnd() < r2.begin()->getEnd();
    }
    if (u1.second->getDataUnitType() == genie::core::parameter::DataUnit::DataUnitType::ACCESS_UNIT) {
        const auto& r1 = dynamic_cast<const genie::format::mgb::AccessUnit&>(*u1.second);
        const auto& r2 = dynamic_cast<const genie::format::mgb::AccessUnit&>(*u2.second);
        return lambda(r1, r2);
    }
    return false;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgb

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_MGB_FILE_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
