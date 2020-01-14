#ifndef GENIE_FORMAT_EXPORTER_H
#define GENIE_FORMAT_EXPORTER_H

#include <util/drain.h>
#include <memory>
#include "block-payload.h"

namespace genie {
class FormatExporter : public util::Drain<genie::mpegg_rec::MpeggChunk> {
   public:
};
}  // namespace genie

#endif  // GENIE_FORMAT_EXPORTER_H
