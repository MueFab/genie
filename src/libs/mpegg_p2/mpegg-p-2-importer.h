#ifndef GENIE_MPEGG_P_2_IMPORTER_H
#define GENIE_MPEGG_P_2_IMPORTER_H

#include "backbone/block-payload.h"
#include "data-unit-factory.h"
#include "util/bitreader.h"
#include "util/ordered-lock.h"
#include "util/original-source.h"
#include "util/source.h"

#include <util/ordered-section.h>
#include <map>
namespace genie {
namespace mpegg_p2 {
class MpeggP2Importer : public util::Source<genie::BlockPayloadSet>, public util::OriginalSource {
   private:
    util::BitReader reader;
    std::map<size_t, mpegg_p2::ParameterSet> parameterSets;
    util::OrderedLock lock;
    mpegg_p2::DataUnitFactory factory;

   public:
    explicit MpeggP2Importer(std::istream& _file);

    bool pump(size_t id) override;

    /**
     * @brief Signal end of data.
     */
    void dryIn() override;
};
}  // namespace mpegg_p2
}  // namespace genie

#endif  // GENIE_MPEGG_P_2_IMPORTER_H
