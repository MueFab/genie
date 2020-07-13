/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "data-unit-factory.h"
#include "access_unit.h"
#include "raw_reference.h"

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
        if (!bitReader.isGood()) {
            return boost::none;
        }
        switch (type) {
            case core::parameter::DataUnit::DataUnitType::RAW_REFERENCE: {
                auto r = RawReference(bitReader);
                break;
            }
            case core::parameter::DataUnit::DataUnitType::PARAMETER_SET: {
                auto p = core::parameter::ParameterSet(bitReader);
                parameters.insert(std::make_pair(p.getID(), std::move(p)));
                break;
            }
            case core::parameter::DataUnit::DataUnitType::ACCESS_UNIT: {
                auto ret = AccessUnit(parameters, bitReader);
                std::cout << "Decompressing AU " << ret.getID() << "..." << std::endl;
                return ret;
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