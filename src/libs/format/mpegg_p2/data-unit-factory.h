#ifndef GENIE_DATA_UNIT_FACTORY_H
#define GENIE_DATA_UNIT_FACTORY_H

#include <map>
#include "clutter.h"
#include "parameter_set.h"
namespace format {
namespace mpegg_p2 {
class DataUnitFactory {
   private:
    std::map<size_t, std::unique_ptr<ParameterSet>> parameters;

   public:
    const ParameterSet& getParams(size_t id) const {
        return *parameters.at(id).get();
    }
    std::unique_ptr<AccessUnit> read(util::BitReader &bitReader) {
        DataUnit::DataUnitType type;
        do {
            type = DataUnit::DataUnitType(bitReader.read(8));
            if(!bitReader.isGood()) {
                return nullptr;
            }
            switch (type) {
                case DataUnit::DataUnitType::RAW_REFERENCE: {
                    UTILS_DIE("DataUnitFactory RAW_REFERENCE not supported yet!");
                }
                case DataUnit::DataUnitType::PARAMETER_SET: {
                    fprintf(stdout, "DataUnitFactory PARAMETER_SET detected!\n");
                    auto p =  util::make_unique<ParameterSet>(bitReader);
                    parameters.insert(std::make_pair(p->getID(), std::move(p)));
                    break;
                }
                case DataUnit::DataUnitType::ACCESS_UNIT: {
                    fprintf(stdout, "DataUnitFactory ACCESS_UNIT detected!\n");
                    return util::make_unique<AccessUnit>(parameters, bitReader);
                }
                default: {
                    UTILS_DIE("DataUnitFactory invalid DataUnitType!");
                }
            }
        } while (type != DataUnit::DataUnitType::ACCESS_UNIT);
        return std::unique_ptr<AccessUnit>();
    }
};
}  // namespace mpegg_p2
}  // namespace format

#endif  // GENIE_DATA_UNIT_FACTORY_H
