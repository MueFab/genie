#ifndef GENIE_FORMAT_IMPORTER_H
#define GENIE_FORMAT_IMPORTER_H

#include <util/original-source.h>
#include <util/source.h>
#include <vector>
#include "mpegg_rec/mpegg-record.h"

namespace genie {

class FormatImporter : public util::Source<mpegg_rec::MpeggChunk>, public util::OriginalSource {};

}  // namespace genie

#endif  // GENIE_FORMAT_IMPORTER_H
