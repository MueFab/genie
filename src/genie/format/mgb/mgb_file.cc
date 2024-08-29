/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgb/mgb_file.h"
#include <algorithm>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgb {

// ---------------------------------------------------------------------------------------------------------------------

uint8_t MgbFile::data_unit_order(const genie::core::parameter::DataUnit& u) {
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

// ---------------------------------------------------------------------------------------------------------------------

void MgbFile::write(genie::util::BitWriter& writer) {
    for (auto& u : units) {
        u.second->write(writer);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MgbFile::addUnit(std::unique_ptr<genie::core::parameter::DataUnit> unit) {
    units.emplace_back(std::make_pair(0, std::move(unit)));
}

// ---------------------------------------------------------------------------------------------------------------------

MgbFile::MgbFile() : file(nullptr) {}

// ---------------------------------------------------------------------------------------------------------------------

MgbFile::MgbFile(std::istream* _file) : file(_file), reader(genie::util::make_unique<genie::util::BitReader>(*file)) {
    while (true) {
        uint64_t pos = reader->getPos();
        auto unit_type = reader->readBypassBE<core::parameter::DataUnit::DataUnitType>();
        if (!reader->isGood()) {
            break;
        }

        switch (unit_type) {
            case core::parameter::DataUnit::DataUnitType::PARAMETER_SET: {
                auto set = util::make_unique<core::parameter::ParameterSet>(*reader);
                parameterSets.emplace(std::make_pair(set->getID(), set->getEncodingSet()));
                units.emplace_back(std::make_pair(pos, std::move(set)));
            } break;
            case core::parameter::DataUnit::DataUnitType::ACCESS_UNIT:
                units.emplace_back(
                    std::make_pair(pos, util::make_unique<format::mgb::AccessUnit>(parameterSets, *reader)));
                break;
            case core::parameter::DataUnit::DataUnitType::RAW_REFERENCE:
                units.emplace_back(std::make_pair(pos, util::make_unique<format::mgb::RawReference>(*reader)));
                break;
            default:
                UTILS_DIE("Unknown data unit");
        }
    }
    file->clear();
    file->seekg(0, std::ios::beg);
}

// ---------------------------------------------------------------------------------------------------------------------

void MgbFile::print_debug(std::ostream& output, uint8_t max_depth) const {
    for (const auto& b : units) {
        b.second->print_debug(output, 0, max_depth);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void MgbFile::sort_by_class() {
    auto sorter = [](const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u1,
                     const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u2) -> bool {
        return base_sorter(
            u1, u2, [](const genie::format::mgb::AccessUnit& a1, const genie::format::mgb::AccessUnit& a2) -> bool {
                if (a1.getHeader().getClass() != a2.getHeader().getClass()) {
                    return uint8_t(a1.getHeader().getClass()) < uint8_t(a2.getHeader().getClass());
                }
                if (a1.getHeader().getClass() == core::record::ClassType::CLASS_U &&
                    a2.getHeader().getClass() == core::record::ClassType::CLASS_U) {
                    return a1.getHeader().getID() < a2.getHeader().getID();
                }
                if (a1.getHeader().getClass() == core::record::ClassType::CLASS_U) {
                    return false;
                }
                if (a1.getHeader().getAlignmentInfo().getRefID() != a2.getHeader().getAlignmentInfo().getRefID()) {
                    return a1.getHeader().getAlignmentInfo().getRefID() < a2.getHeader().getAlignmentInfo().getRefID();
                }
                if (a1.getHeader().getAlignmentInfo().getStartPos() !=
                    a2.getHeader().getAlignmentInfo().getStartPos()) {
                    return a1.getHeader().getAlignmentInfo().getStartPos() <
                           a2.getHeader().getAlignmentInfo().getStartPos();
                }
                return a1.getHeader().getAlignmentInfo().getEndPos() < a2.getHeader().getAlignmentInfo().getEndPos();
            });
    };
    std::sort(units.begin(), units.end(), sorter);
}

// ---------------------------------------------------------------------------------------------------------------------

void MgbFile::sort_by_position() {
    auto sorter = [](const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u1,
                     const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u2) -> bool {
        return base_sorter(
            u1, u2, [](const genie::format::mgb::AccessUnit& a1, const genie::format::mgb::AccessUnit& a2) -> bool {
                if (a1.getHeader().getClass() == core::record::ClassType::CLASS_U &&
                    a2.getHeader().getClass() == core::record::ClassType::CLASS_U) {
                    return a1.getHeader().getID() < a2.getHeader().getID();
                }
                if (a1.getHeader().getClass() == core::record::ClassType::CLASS_U) {
                    return false;
                }
                if (a2.getHeader().getClass() == core::record::ClassType::CLASS_U) {
                    return true;
                }
                if (a1.getHeader().getAlignmentInfo().getRefID() != a2.getHeader().getAlignmentInfo().getRefID()) {
                    return a1.getHeader().getAlignmentInfo().getRefID() < a2.getHeader().getAlignmentInfo().getRefID();
                }
                if (a1.getHeader().getAlignmentInfo().getStartPos() !=
                    a2.getHeader().getAlignmentInfo().getStartPos()) {
                    return a1.getHeader().getAlignmentInfo().getStartPos() <
                           a2.getHeader().getAlignmentInfo().getStartPos();
                }
                if (a1.getHeader().getAlignmentInfo().getEndPos() != a2.getHeader().getAlignmentInfo().getEndPos()) {
                    return a1.getHeader().getAlignmentInfo().getEndPos() <
                           a2.getHeader().getAlignmentInfo().getEndPos();
                }
                return uint8_t(a1.getHeader().getClass()) < uint8_t(a2.getHeader().getClass());
            });
    };
    std::sort(units.begin(), units.end(), sorter);
}

// ---------------------------------------------------------------------------------------------------------------------

void MgbFile::remove_class(core::record::ClassType type) {
    auto remover = [type](const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u1) -> bool {
        if (u1.second->getDataUnitType() != genie::core::parameter::DataUnit::DataUnitType::ACCESS_UNIT) {
            return false;
        }
        return dynamic_cast<const genie::format::mgb::AccessUnit&>(*u1.second).getHeader().getClass() == type;
    };
    units.erase(std::remove_if(units.begin(), units.end(), remover), units.end());
}

// ---------------------------------------------------------------------------------------------------------------------

void MgbFile::select_mapping_range(uint16_t ref_id, uint64_t start_pos, uint64_t end_pos) {
    auto remover = [ref_id, start_pos,
                    end_pos](const std::pair<uint64_t, std::unique_ptr<genie::core::parameter::DataUnit>>& u1) -> bool {
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

// ---------------------------------------------------------------------------------------------------------------------

void MgbFile::remove_unused_parametersets() {
    std::set<uint8_t> ids;
    for (const auto& u : units) {
        if (u.second->getDataUnitType() == genie::core::parameter::DataUnit::DataUnitType::ACCESS_UNIT) {
            ids.emplace(dynamic_cast<const genie::format::mgb::AccessUnit&>(*u.second).getHeader().getParameterID());
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

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Block> MgbFile::extractDescriptor(core::record::ClassType type, core::GenDesc descriptor,
                                              const std::vector<uint8_t>& param_sets) {
    std::vector<Block> ret;
    for (auto& u : units) {
        if (u.second->getDataUnitType() == genie::core::parameter::DataUnit::DataUnitType::ACCESS_UNIT) {
            auto& au = dynamic_cast<genie::format::mgb::AccessUnit&>(*u.second);
            if (std::find(param_sets.begin(), param_sets.end(), au.getHeader().getParameterID()) == param_sets.end()) {
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

// ---------------------------------------------------------------------------------------------------------------------

void MgbFile::clearAUBlocks(const std::vector<uint8_t>& param_sets) {
    for (auto& u : units) {
        if (u.second->getDataUnitType() == genie::core::parameter::DataUnit::DataUnitType::ACCESS_UNIT) {
            auto& au = dynamic_cast<genie::format::mgb::AccessUnit&>(*u.second);
            if (std::find(param_sets.begin(), param_sets.end(), au.getHeader().getParameterID()) != param_sets.end()) {
                au.getBlocks().clear();
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::unique_ptr<core::parameter::ParameterSet>> MgbFile::extractParameters(
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

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::unique_ptr<format::mgb::AccessUnit>> MgbFile::extractAUs(const std::vector<uint8_t>& param_sets) {
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

// ---------------------------------------------------------------------------------------------------------------------

std::vector<uint8_t> MgbFile::collect_param_ids(bool multipleAlignments, bool pos40,
                                                genie::core::parameter::DataUnit::DatasetType dataset_type,
                                                genie::core::AlphabetID alphabet) {
    std::vector<uint8_t> ret;
    for (auto& parameterSet : parameterSets) {
        if (parameterSet.second.hasMultipleAlignments() == multipleAlignments &&
            (parameterSet.second.getPosSize() == 40) == pos40 && parameterSet.second.getDatasetType() == dataset_type &&
            parameterSet.second.getAlphabetID() == alphabet) {
            ret.emplace_back(static_cast<uint8_t>(parameterSet.first));
        }
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgb

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
