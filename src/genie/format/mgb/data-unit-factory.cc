/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "data-unit-factory.h"
#include "access_unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

const core::parameter::ParameterSet& DataUnitFactory::getParams(size_t id) const { return *parameters.at(id).get(); }

// ---------------------------------------------------------------------------------------------------------------------

std::unique_ptr<AccessUnit> DataUnitFactory::read(util::BitReader& bitReader) {
    core::parameter::DataUnit::DataUnitType type;
    do {
        type = core::parameter::DataUnit::DataUnitType(bitReader.read(8));
        if (!bitReader.isGood()) {
            return nullptr;
        }
        switch (type) {
            case core::parameter::DataUnit::DataUnitType::RAW_REFERENCE: {
                UTILS_DIE("DataUnitFactory RAW_REFERENCE not supported yet!");
            }
            case core::parameter::DataUnit::DataUnitType::PARAMETER_SET: {
                auto p = util::make_unique<core::parameter::ParameterSet>(bitReader);
                parameters.insert(std::make_pair(p->getID(), std::move(p)));
                break;
            }
            case core::parameter::DataUnit::DataUnitType::ACCESS_UNIT: {
                auto ret = util::make_unique<AccessUnit>(parameters, bitReader);
                std::cout << "Decompressing AU " << ret->getID() << "..." << std::endl;
                return ret;
            }
            default: {
                UTILS_DIE("DataUnitFactory invalid DataUnitType!");
            }
        }
    } while (type != core::parameter::DataUnit::DataUnitType::ACCESS_UNIT);
    return std::unique_ptr<AccessUnit>();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------