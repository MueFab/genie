#ifndef GENIE_MPEGG_P_2_EXPORTER_H
#define GENIE_MPEGG_P_2_EXPORTER_H

#include <util/drain.h>

#include <backbone/mpegg-raw-au.h>
#include <memory>
#include <vector>

#include <backbone/block-payload.h>
#include <util/ordered-lock.h>
#include <util/ordered-section.h>
#include "access_unit.h"
namespace genie {
namespace mpegg_p2 {
class MpeggP2Exporter : public util::Drain<genie::BlockPayloadSet> {
   private:
    util::BitWriter writer;
    util::OrderedLock lock;

   public:
    explicit MpeggP2Exporter(std::ostream* _file);
    void flowIn(genie::BlockPayloadSet&& t, size_t id) override;
    void dryIn() override;
};
}  // namespace mpegg_p2
}  // namespace genie

#endif  // GENIE_MPEGG_P_2_EXPORTER_H
