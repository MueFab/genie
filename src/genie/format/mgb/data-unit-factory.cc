/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "data-unit-factory.h"
#include "access_unit.h"
#include "raw_reference.h"
#include "reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

const core::parameter::ParameterSet& DataUnitFactory::getParams(size_t id) const { return parameters.at(id); }

// ---------------------------------------------------------------------------------------------------------------------

boost::optional<AccessUnit> DataUnitFactory::read(util::BitReader& bitReader) {
    core::parameter::DataUnit::DataUnitType type;
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
                for(auto& ref : r) {
                    pos += 12;
                    std::cout << "Found ref(raw) " << ref.getSeqID() << ":[" << ref.getStart() << ", " << ref.getEnd() << "] ..." << std::endl;
                    refmgr->validateRefID(ref.getSeqID());
                    refmgr->addRef(util::make_unique<mgb::Reference>(refmgr->ID2Ref(ref.getSeqID()), ref.getStart(), ref.getEnd() + 1, importer, pos, true));
                    pos += (ref.getEnd() - ref.getStart() + 1);
                }
                break;
            }
            case core::parameter::DataUnit::DataUnitType::PARAMETER_SET: {
                auto p = core::parameter::ParameterSet(bitReader);
                std::cout << "Found PS " << (uint32_t )p.getID() << "..." << std::endl;
                parameters.insert(std::make_pair(p.getID(), std::move(p)));
                break;
            }
            case core::parameter::DataUnit::DataUnitType::ACCESS_UNIT: {
                auto ret = AccessUnit(parameters, bitReader, true);
                if(getParams(ret.getParameterID()).getDatasetType() == mgb::AccessUnit::DatasetType::REFERENCE) {
                    const auto& ref = ret.getRefCfg();
                    refmgr->validateRefID(ref.getSeqID());
                    std::cout << "Found ref(compressed) " << ref.getSeqID() << ":[" << ref.getStart() << ", " << ref.getEnd() << "] ..." << std::endl;
                    refmgr->addRef(util::make_unique<mgb::Reference>(refmgr->ID2Ref(ref.getSeqID()), ref.getStart(), ref.getEnd() + 1, importer, pos, false));
                    bitReader.skip(ret.getPayloadSize());
                } else {
                    if(!referenceOnly) {
                        ret.loadPayload(bitReader);
                        std::cout << "Decompressing AU " << ret.getID() << "..." << std::endl;
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

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------