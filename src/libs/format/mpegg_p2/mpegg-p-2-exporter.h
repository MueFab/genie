#ifndef GENIE_MPEGG_P_2_EXPORTER_H
#define GENIE_MPEGG_P_2_EXPORTER_H

#include <util/drain.h>

#include <coding/mpegg-raw-au.h>
#include <memory>
#include <vector>

#include <format/block-payload.h>
#include <util/ordered-lock.h>
#include <util/ordered-section.h>
#include "access_unit.h"

class MpeggP2Exporter : public Drain<BlockPayloadSet> {
   private:
    util::BitWriter writer;
    OrderedLock lock;

   public:
    explicit MpeggP2Exporter(std::ostream *_file) : writer(_file) {}
    void flowIn(BlockPayloadSet&& t, size_t id) override {
        BlockPayloadSet data = std::move(t);
        OrderedSection section(&lock, id);
        data.getParameters().write(writer);

        format::mpegg_p2::AccessUnit au(id, id, format::mpegg_rec::ClassType::CLASS_I, data.getRecordNum(),
                                        format::mpegg_p2::DataUnit::DatasetType::ALIGNED, 32, 32, 0);
        au.setAuTypeCfg(util::make_unique<format::AuTypeCfg>(data.getReference(), data.getMinPos(), data.getMaxPos(), data.getParameters().getPosSize()));
        for (size_t descriptor = 0; descriptor < getDescriptors().size(); ++descriptor) {
            if (data.getPayload(GenDesc(descriptor)).isEmpty()) {
                continue;
            }
            au.addBlock(format::Block(descriptor, data.movePayload(descriptor)));
        }
        au.write(writer);
    }
    void dryIn() override {}
};

#endif  // GENIE_MPEGG_P_2_EXPORTER_H
