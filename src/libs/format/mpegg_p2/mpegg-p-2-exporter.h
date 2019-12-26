#ifndef GENIE_MPEGG_P_2_EXPORTER_H
#define GENIE_MPEGG_P_2_EXPORTER_H

#include <util/drain.h>

#include <memory>
#include <vector>
#include <coding/mpegg-raw-au.h>

#include <format/block-payload.h>
#include <util/ordered-lock.h>
#include <util/ordered-section.h>
#include "access_unit.h"

class MpeggP2Exporter : public Drain<std::unique_ptr<BlockPayloadSet>>{
private:
    util::BitWriter writer;
    OrderedLock lock;
public:


    explicit MpeggP2Exporter(std::ostream *_file) : writer(_file) {

    }
    void flowIn(std::unique_ptr<BlockPayloadSet> t, size_t id) override {
        OrderedSection section(&lock, id);
        t->getParameters()->write(&writer);

        format::mpegg_p2::AccessUnit au(id, 0, format::mpegg_rec::MpeggRecord::ClassType::CLASS_I, t->getRecordNum(), format::mpegg_p2::DataUnit::DatasetType::ALIGNED, 32, 32, 0);
        for (size_t descriptor = 0; descriptor < getDescriptors().size(); ++descriptor) {
            if(!t->getPayload(GenomicDescriptor(descriptor))) {
                continue;
            }
            au.addBlock(util::make_unique<format::Block>(descriptor, t->movePayload(descriptor)));
        }
        au.write(&writer);
    }
    void dryIn() override {
    }
};


#endif //GENIE_MPEGG_P_2_EXPORTER_H
