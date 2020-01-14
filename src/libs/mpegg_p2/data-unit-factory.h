#ifndef GENIE_DATA_UNIT_FACTORY_H
#define GENIE_DATA_UNIT_FACTORY_H

#include <map>
#include "parameter_set.h"
#include "access_unit.h"
namespace genie {
namespace mpegg_p2 {
class DataUnitFactory {
   private:
    std::map<size_t, std::unique_ptr<ParameterSet>> parameters;

   public:
    const ParameterSet& getParams(size_t id) const;
    std::unique_ptr<AccessUnit> read(util::BitReader& bitReader);
};
}  // namespace mpegg_p2
}  // namespace genie

#endif  // GENIE_DATA_UNIT_FACTORY_H
