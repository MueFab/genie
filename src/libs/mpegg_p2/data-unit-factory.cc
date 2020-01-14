#include "data-unit-factory.h"
#include "access_unit.h"

namespace genie {
namespace mpegg_p2 {
    const ParameterSet& DataUnitFactory::getParams(size_t id) const { return *parameters.at(id).get(); }
    std::unique_ptr<AccessUnit> DataUnitFactory::read(util::BitReader& bitReader) {
        DataUnit::DataUnitType type;
        do {
            type = DataUnit::DataUnitType(bitReader.read(8));
            if (!bitReader.isGood()) {
                return nullptr;
            }
            switch (type) {
                case DataUnit::DataUnitType::RAW_REFERENCE: {
                    UTILS_DIE("DataUnitFactory RAW_REFERENCE not supported yet!");
                }
                case DataUnit::DataUnitType::PARAMETER_SET: {
                    auto p = util::make_unique<ParameterSet>(bitReader);
                    parameters.insert(std::make_pair(p->getID(), std::move(p)));
                    break;
                }
                case DataUnit::DataUnitType::ACCESS_UNIT: {
                    auto ret = util::make_unique<AccessUnit>(parameters, bitReader);
                    std::cout << "Decompressing AU " << ret->getID() << "..." << std::endl;
                    return ret;
                }
                default: {
                    UTILS_DIE("DataUnitFactory invalid DataUnitType!");
                }
            }
        } while (type != DataUnit::DataUnitType::ACCESS_UNIT);
        return std::unique_ptr<AccessUnit>();
    }
}  // namespace mpegg_p2
}  // namespace genie