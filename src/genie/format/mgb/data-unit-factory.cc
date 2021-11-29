/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgb/data-unit-factory.h"
#include <iostream>
#include <string>
#include <utility>
#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgb/raw_reference.h"
#include "genie/format/mgb/reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

DataUnitFactory::DataUnitFactory(core::ReferenceManager* mgr, mgb::Importer* _importer, bool ref)
    : refmgr(mgr), importer(_importer), referenceOnly(ref) {}

// ---------------------------------------------------------------------------------------------------------------------

const core::parameter::ParameterSet& DataUnitFactory::getParams(size_t id) const { return parameters.at(id); }

// ---------------------------------------------------------------------------------------------------------------------

boost::optional<AccessUnit> DataUnitFactory::read(util::BitReader& bitReader) {
    core::parameter::DataUnit::DataUnitType type;
    int i = 0;
    do {
        type = bitReader.read<core::parameter::DataUnit::DataUnitType>();
        size_t pos = bitReader.getPos();
        if (!bitReader.isGood()) {
            bitReader.clear();
            return boost::none;
        }
        switch (type) {
            case core::parameter::DataUnit::DataUnitType::RAW_REFERENCE: {
                pos += 10;
                auto r = RawReference(bitReader, true);
                for (auto& ref : r) {
                    pos += 12;
                    std::cerr << "Found ref(raw) " << ref.getSeqID() << ":[" << ref.getStart() << ", " << ref.getEnd()
                              << "] ..." << std::endl;
                    refmgr->validateRefID(ref.getSeqID());
                    refmgr->addRef(i++,
                                   util::make_unique<mgb::Reference>(refmgr->ID2Ref(ref.getSeqID()), ref.getStart(),
                                                                     ref.getEnd() + 1, importer, pos, true));
                    pos += (ref.getEnd() - ref.getStart() + 1);
                }
                break;
            }
            case core::parameter::DataUnit::DataUnitType::PARAMETER_SET: {
                auto p = core::parameter::ParameterSet(bitReader);
                std::cerr << "Found PS " << (uint32_t)p.getID() << "..." << std::endl;
                parameters.insert(std::make_pair(p.getID(), std::move(p)));
                break;
            }
            case core::parameter::DataUnit::DataUnitType::ACCESS_UNIT: {
                auto ret = AccessUnit(parameters, bitReader, true);
                if (getParams(ret.getParameterID()).getDatasetType() == mgb::AccessUnit::DatasetType::REFERENCE) {
                    const auto& ref = ret.getRefCfg();
                    refmgr->validateRefID(ref.getSeqID());
                    std::cerr << "Found ref(compressed) " << ref.getSeqID() << ":[" << ref.getStart() << ", "
                              << ref.getEnd() << "] ..." << std::endl;
                    refmgr->addRef(i++,
                                   util::make_unique<mgb::Reference>(refmgr->ID2Ref(ref.getSeqID()), ref.getStart(),
                                                                     ref.getEnd() + 1, importer, pos, false));
                    bitReader.skip(ret.getPayloadSize());
                } else {
                    std::string lut[] = {"NONE", "P", "N", "M", "I", "HM", "U"};
                    if (!referenceOnly) {
                        ret.loadPayload(bitReader);
                        std::cerr << "Decompressing AU " << ret.getID() << " of class "
                                  << lut[static_cast<size_t>(ret.getClass())];
                        UTILS_DIE_IF(ret.getClass() == genie::core::record::ClassType::CLASS_HM,
                                     "Class HM not supported");
                        if (ret.getClass() == genie::core::record::ClassType::CLASS_U) {
                            if (!parameters.at(ret.getParameterID()).isComputedReference()) {
                                std::cerr << " (Low Latency)";
                            } else {
                                if (parameters.at(ret.getParameterID()).getComputedRef().getAlgorithm() ==
                                    core::parameter::ComputedRef::Algorithm::GLOBAL_ASSEMBLY) {
                                    std::cerr << " (Global Assembly)";
                                } else {
                                    UTILS_DIE(
                                        "Computed ref not supported: " +
                                        std::to_string(static_cast<int>(
                                            parameters.at(ret.getParameterID()).getComputedRef().getAlgorithm())));
                                }
                            }
                        } else {
                            if (!parameters.at(ret.getParameterID()).isComputedReference()) {
                                std::cerr << " (Reference)";
                            } else {
                                if (parameters.at(ret.getParameterID()).getComputedRef().getAlgorithm() ==
                                    core::parameter::ComputedRef::Algorithm::LOCAL_ASSEMBLY) {
                                    std::cerr << " (Local Assembly)";
                                } else {
                                    UTILS_DIE(
                                        "Computed ref not supported: " +
                                        std::to_string(static_cast<int>(
                                            parameters.at(ret.getParameterID()).getComputedRef().getAlgorithm())));
                                }
                            }
                        }
                        std::cerr << "..." << std::endl;
                        return ret;
                    } else {
                        bitReader.skip(ret.getPayloadSize());
                    }
                }
                break;
            }
            default: {
                UTILS_DIE("DataUnitFactory invalid DataUnitType!");
            }
        }
    } while (true);
}

// ---------------------------------------------------------------------------------------------------------------------

const std::map<size_t, core::parameter::ParameterSet>& DataUnitFactory::getParams() const { return parameters; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
