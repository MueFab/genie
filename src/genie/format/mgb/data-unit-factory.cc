/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgb/data-unit-factory.h"
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "genie/format/mgb/access_unit.h"
#include "genie/format/mgb/raw_reference.h"
#include "genie/format/mgb/reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgb {

// ---------------------------------------------------------------------------------------------------------------------

DataUnitFactory::DataUnitFactory(core::ReferenceManager* mgr, mgb::Importer* _importer, bool ref)
    : refmgr(mgr), importer(_importer), referenceOnly(ref) {}

// ---------------------------------------------------------------------------------------------------------------------

const core::parameter::EncodingSet& DataUnitFactory::getParams(size_t id) const { return parameters.at(id); }

// ---------------------------------------------------------------------------------------------------------------------

std::optional<AccessUnit> DataUnitFactory::read(util::BitReader& bitReader) {
    core::parameter::DataUnit::DataUnitType type;
    int i = 0;
    do {
        type = bitReader.read<core::parameter::DataUnit::DataUnitType>();
        size_t pos = bitReader.getStreamPosition();
        if (!bitReader.isStreamGood()) {
            bitReader.clearStreamState();
            return std::nullopt;
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
                    refmgr->addRef(i++, std::make_unique<mgb::Reference>(refmgr->ID2Ref(ref.getSeqID()), ref.getStart(),
                                                                         ref.getEnd() + 1, importer, pos, true));
                    pos += (ref.getEnd() - ref.getStart() + 1);
                }
                break;
            }
            case core::parameter::DataUnit::DataUnitType::PARAMETER_SET: {
                auto p = core::parameter::ParameterSet(bitReader);
                std::cerr << "Found PS " << static_cast<uint32_t>(p.getID()) << "..." << std::endl;
                parameters.insert(std::make_pair(p.getID(), std::move(p.getEncodingSet())));
                break;
            }
            case core::parameter::DataUnit::DataUnitType::ACCESS_UNIT: {
                auto ret = AccessUnit(parameters, bitReader, true);
                if (getParams(ret.getHeader().getParameterID()).getDatasetType() ==
                    mgb::AccessUnit::DatasetType::REFERENCE) {
                    const auto& ref = ret.getHeader().getRefCfg();
                    refmgr->validateRefID(ref.getSeqID());
                    std::cerr << "Found ref(compressed) " << ref.getSeqID() << ":[" << ref.getStart() << ", "
                              << ref.getEnd() << "] ..." << std::endl;
                    refmgr->addRef(i++, std::make_unique<mgb::Reference>(refmgr->ID2Ref(ref.getSeqID()), ref.getStart(),
                                                                         ref.getEnd() + 1, importer, pos, false));
                    bitReader.skipAlignedBytes(ret.getPayloadSize());
                } else {
                    if (!referenceOnly) {
                        ret.loadPayload(bitReader);
                        for (auto& b : ret.getBlocks()) {
                            b.load();
                            b.parse();
                        }
                        UTILS_DIE_IF(ret.getHeader().getClass() == genie::core::record::ClassType::CLASS_HM,
                                     "Class HM not supported");
                        ret.debugPrint(parameters.at(ret.getHeader().getParameterID()));
                        return ret;
                    } else {
                        bitReader.skipAlignedBytes(ret.getPayloadSize());
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

const std::map<size_t, core::parameter::EncodingSet>& DataUnitFactory::getParams() const { return parameters; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgb

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
