#ifndef GENIE_FORMAT_EXPORTER_H
#define GENIE_FORMAT_EXPORTER_H

#include "block-payload.h"
#include <memory>

class FormatExporter : public Drain<std::unique_ptr<BlockPayload>>{
public:
};

#endif //GENIE_FORMAT_EXPORTER_H
