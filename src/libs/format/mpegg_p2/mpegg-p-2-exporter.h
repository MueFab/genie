#ifndef GENIE_MPEGG_P_2_EXPORTER_H
#define GENIE_MPEGG_P_2_EXPORTER_H

#include <util/drain.h>

#include <memory>

#include "block-payload.h"

class MpeggP2Exporter : public Drain<std::unique_ptr<BlockPayload>>{
public:
    virtual void flowIn(std::unique_ptr<BlockPayload> t, size_t id) override;
    virtual void dryIn() override;
};


#endif //GENIE_MPEGG_P_2_EXPORTER_H
