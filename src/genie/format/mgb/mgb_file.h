/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MGB_FILE_H
#define GENIE_MGB_FILE_H

#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <vector>
#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgb/data-unit-factory.h"
#include "genie/format/mgb/raw_reference.h"
#include "genie/util/bitreader.h"

namespace genie {
namespace format {
namespace mgb {

class MgbFile {
 private:
    std::vector<std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>> units;
    std::istream* file;
    util::BitReader reader;

    std::map<size_t, core::parameter::EncodingSet> parameterSets;

    static uint8_t data_unit_order(const genie::core::parameter::DataUnit& u) {
        switch (u.getDataUnitType()) {
            case core::parameter::DataUnit::DataUnitType::PARAMETER_SET:
                return 0;
            case core::parameter::DataUnit::DataUnitType::RAW_REFERENCE:
                return 1;
            case core::parameter::DataUnit::DataUnitType::ACCESS_UNIT:
                return 2;
        }
        return 255;
    }

    template <typename LAMBDA>
    static bool base_sorter(const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u1,
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

 public:
    explicit MgbFile(std::istream* _file) : file(_file), reader(*file) {
        while (true) {
            uint64_t pos = reader.getPos();
            auto unit_type = reader.readBypassBE<core::parameter::DataUnit::DataUnitType>();
            if (!reader.isGood()) {
                break;
            }

            switch (unit_type) {
                case core::parameter::DataUnit::DataUnitType::PARAMETER_SET: {
                    auto set = util::make_unique<core::parameter::ParameterSet>(reader);
                    parameterSets.emplace(std::make_pair(set->getID(), set->getEncodingSet()));
                    units.emplace_back(std::make_pair(pos, std::move(set)));
                } break;
                case core::parameter::DataUnit::DataUnitType::ACCESS_UNIT:
                    units.emplace_back(
                        std::make_pair(pos, util::make_unique<format::mgb::AccessUnit>(parameterSets, reader)));
                    break;
                case core::parameter::DataUnit::DataUnitType::RAW_REFERENCE:
                    units.emplace_back(std::make_pair(pos, util::make_unique<format::mgb::RawReference>(reader)));
                    break;
                default:
                    UTILS_DIE("Unknown data unit");
            }
        }
    }

    void print_debug(std::ostream& output, uint8_t max_depth = 100) const {
        for (const auto& b : units) {
            b.second->print_debug(output, 0, max_depth);
        }
    }

    void sort_by_class() {
        auto sorter = [](const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u1,
                         const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u2) -> bool {
            return base_sorter(
                u1, u2, [](const genie::format::mgb::AccessUnit& a1, const genie::format::mgb::AccessUnit& a2) -> bool {
                    if (a1.getHeader().getClass() != a2.getHeader().getClass()) {
                        return uint8_t(a1.getHeader().getClass()) < uint8_t(a2.getHeader().getClass());
                    }
                    if (a1.getHeader().getClass() == core::record::ClassType::CLASS_U) {
                        return false;
                    }
                    if (a1.getHeader().getAlignmentInfo().getRefID() != a2.getHeader().getAlignmentInfo().getRefID()) {
                        return a1.getHeader().getAlignmentInfo().getRefID() <
                               a2.getHeader().getAlignmentInfo().getRefID();
                    }
                    if (a1.getHeader().getAlignmentInfo().getStartPos() !=
                        a2.getHeader().getAlignmentInfo().getStartPos()) {
                        return a1.getHeader().getAlignmentInfo().getStartPos() <
                               a2.getHeader().getAlignmentInfo().getStartPos();
                    }
                    return a1.getHeader().getAlignmentInfo().getEndPos() <
                           a2.getHeader().getAlignmentInfo().getEndPos();
                });
        };
        std::sort(units.begin(), units.end(), sorter);
    }

    void sort_by_position() {
        auto sorter = [](const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u1,
                         const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u2) -> bool {
            return base_sorter(
                u1, u2, [](const genie::format::mgb::AccessUnit& a1, const genie::format::mgb::AccessUnit& a2) -> bool {
                    if (a1.getHeader().getClass() == core::record::ClassType::CLASS_U) {
                        return false;
                    }
                    if (a2.getHeader().getClass() == core::record::ClassType::CLASS_U) {
                        return true;
                    }
                    if (a1.getHeader().getAlignmentInfo().getRefID() != a2.getHeader().getAlignmentInfo().getRefID()) {
                        return a1.getHeader().getAlignmentInfo().getRefID() <
                               a2.getHeader().getAlignmentInfo().getRefID();
                    }
                    if (a1.getHeader().getAlignmentInfo().getStartPos() !=
                        a2.getHeader().getAlignmentInfo().getStartPos()) {
                        return a1.getHeader().getAlignmentInfo().getStartPos() <
                               a2.getHeader().getAlignmentInfo().getStartPos();
                    }
                    if (a1.getHeader().getAlignmentInfo().getEndPos() !=
                        a2.getHeader().getAlignmentInfo().getEndPos()) {
                        return a1.getHeader().getAlignmentInfo().getEndPos() <
                               a2.getHeader().getAlignmentInfo().getEndPos();
                    }
                    return uint8_t(a1.getHeader().getClass()) < uint8_t(a2.getHeader().getClass());
                });
        };
        std::sort(units.begin(), units.end(), sorter);
    }

    void remove_class(core::record::ClassType type) {
        auto remover =
            [type](const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u1) -> bool {
            if (u1.second->getDataUnitType() != genie::core::parameter::DataUnit::DataUnitType::ACCESS_UNIT) {
                return false;
            }
            return dynamic_cast<const genie::format::mgb::AccessUnit&>(*u1.second).getHeader().getClass() == type;
        };
        units.erase(std::remove_if(units.begin(), units.end(), remover), units.end());
    }

    void select_mapping_range(uint16_t ref_id, uint64_t start_pos, uint64_t end_pos) {
        auto remover = [ref_id, start_pos, end_pos](
                           const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u1) -> bool {
            if (u1.second->getDataUnitType() != genie::core::parameter::DataUnit::DataUnitType::ACCESS_UNIT) {
                return false;
            }

            if (dynamic_cast<const genie::format::mgb::AccessUnit&>(*u1.second).getHeader().getClass() ==
                core::record::ClassType::CLASS_U) {
                return false;
            }

            return dynamic_cast<const genie::format::mgb::AccessUnit&>(*u1.second)
                           .getHeader()
                           .getAlignmentInfo()
                           .getRefID() != ref_id ||
                   dynamic_cast<const genie::format::mgb::AccessUnit&>(*u1.second)
                           .getHeader()
                           .getAlignmentInfo()
                           .getStartPos() < start_pos ||
                   dynamic_cast<const genie::format::mgb::AccessUnit&>(*u1.second)
                           .getHeader()
                           .getAlignmentInfo()
                           .getEndPos() >= end_pos;
        };
        units.erase(std::remove_if(units.begin(), units.end(), remover), units.end());
    }

    void remove_unused_parametersets() {
        std::set<uint8_t> ids;
        for (const auto& u : units) {
            if (u.second->getDataUnitType() == genie::core::parameter::DataUnit::DataUnitType::ACCESS_UNIT) {
                ids.emplace(
                    dynamic_cast<const genie::format::mgb::AccessUnit&>(*u.second).getHeader().getParameterID());
            }
        }

        units.erase(
            std::remove_if(
                units.begin(), units.end(),
                [&ids](const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u1) -> bool {
                    if (u1.second->getDataUnitType() != genie::core::parameter::DataUnit::DataUnitType::PARAMETER_SET) {
                        return false;
                    }
                    if (ids.find(dynamic_cast<const genie::core::parameter::ParameterSet&>(*u1.second).getID()) ==
                        ids.end()) {
                        return true;
                    } else {
                        return false;
                    }
                }),
            units.end());
    }

    std::vector<Block> extractDescriptor(core::record::ClassType type, core::GenDesc descriptor, const std::vector<uint8_t>& param_sets) {
        std::vector<Block> ret;
        for (auto& u : units) {
            if (u.second->getDataUnitType() == genie::core::parameter::DataUnit::DataUnitType::ACCESS_UNIT) {
                auto& au = dynamic_cast<genie::format::mgb::AccessUnit&>(*u.second);
                if (std::find(param_sets.begin(), param_sets.end(), au.getHeader().getParameterID()) ==
                    param_sets.end()) {
                    continue;
                }
                if (au.getHeader().getClass() != type) {
                    continue;
                }
                bool found = false;
                for (auto& a : au.getBlocks()) {
                    if (core::GenDesc(a.getDescriptorID()) == descriptor) {
                        found = true;
                        ret.emplace_back(std::move(a));
                        break;
                    }
                }
                if (!found) {
                    ret.emplace_back(Block(uint8_t(descriptor), core::AccessUnit::Descriptor(descriptor)));
                }
            }
        }
        return ret;
    }

    void clearAUBlocks(const std::vector<uint8_t>& param_sets) {
        for (auto& u : units) {
            if (u.second->getDataUnitType() == genie::core::parameter::DataUnit::DataUnitType::ACCESS_UNIT) {
                auto& au = dynamic_cast<genie::format::mgb::AccessUnit&>(*u.second);
                if (std::find(param_sets.begin(), param_sets.end(), au.getHeader().getParameterID()) !=
                    param_sets.end()) {
                    au.getBlocks().clear();
                }
            }
        }
    }

    std::vector<std::unique_ptr<core::parameter::ParameterSet>> extractParameters(
        const std::vector<uint8_t>& param_sets) {
        std::vector<std::unique_ptr<core::parameter::ParameterSet>> ret;
        for (auto it = units.begin(); it != units.end();) {
            if (it->second->getDataUnitType() == core::parameter::DataUnit::DataUnitType::PARAMETER_SET) {
                if (std::find(param_sets.begin(), param_sets.end(),
                              dynamic_cast<core::parameter::ParameterSet*>(it->second.get())->getID()) !=
                    param_sets.end()) {
                    ret.emplace_back(dynamic_cast<core::parameter::ParameterSet*>(it->second.release()));
                    it = units.erase(it);
                } else {
                    it++;
                }
            } else {
                it++;
            }
        }
        return ret;
    }

    std::vector<std::unique_ptr<format::mgb::AccessUnit>> extractAUs(const std::vector<uint8_t>& param_sets) {
        std::vector<std::unique_ptr<format::mgb::AccessUnit>> ret;
        for (auto it = units.begin(); it != units.end();) {
            if (it->second->getDataUnitType() == core::parameter::DataUnit::DataUnitType::ACCESS_UNIT) {
                if (std::find(param_sets.begin(), param_sets.end(),
                              dynamic_cast<format::mgb::AccessUnit*>(it->second.get())->getHeader().getParameterID()) !=
                    param_sets.end()) {
                    ret.emplace_back(dynamic_cast<format::mgb::AccessUnit*>(it->second.release()));
                    it = units.erase(it);
                } else {
                    it++;
                }
            } else {
                it++;
            }
        }
        return ret;
    }

    std::vector<uint8_t> collect_param_ids(bool multipleAlignments, bool pos40,
                                           genie::core::parameter::DataUnit::DatasetType dataset_type,
                                           genie::core::AlphabetID alphabet) {
        std::vector<uint8_t> ret;
        for (auto & parameterSet : parameterSets) {
            if (parameterSet.second.hasMultipleAlignments() == multipleAlignments &&
                (parameterSet.second.getPosSize() == 40) == pos40 && parameterSet.second.getDatasetType() == dataset_type &&
                parameterSet.second.getAlphabetID() == alphabet) {
                ret.emplace_back(parameterSet.first);
            }
        }
        return ret;
    }
};

}  // namespace mgb
}  // namespace format
}  // namespace genie

#endif  // GENIE_MGB_FILE_H
