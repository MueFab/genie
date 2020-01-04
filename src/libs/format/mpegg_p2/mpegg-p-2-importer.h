#ifndef GENIE_MPEGG_P_2_IMPORTER_H
#define GENIE_MPEGG_P_2_IMPORTER_H

#include "data-unit-factory.h"
#include "format/block-payload.h"
#include "util/bitreader.h"
#include "util/ordered-lock.h"
#include "util/original-source.h"
#include "util/source.h"

#include <util/ordered-section.h>
#include <map>

class MpeggP2Importer : public Source<BlockPayloadSet>, public OriginalSource{
   private:
    util::BitReader reader;
    std::map<size_t, format::mpegg_p2::ParameterSet> parameterSets;
    OrderedLock lock;
    format::mpegg_p2::DataUnitFactory factory;
   public:
    explicit MpeggP2Importer(std::istream &_file) : reader(_file){
    }

    bool pump(size_t id) override {
        std::unique_ptr<format::mpegg_p2::AccessUnit> unit;
        {
            OrderedSection section(&lock, id);
            unit = factory.read(reader);
        }
        if(!unit) {
            return false;
        }
        BlockPayloadSet set(factory.getParams(unit->getParameterID()), id);

        for(auto& b : unit->getBlocks()) {
            set.setPayload(GenDesc(b.getDescriptorID()), b.movePayload());
        }
        set.setReference(unit->getAlignmentInfo().getRefID());
        set.setMinPos(unit->getAlignmentInfo().getStartPos());
        set.setMaxPos(unit->getAlignmentInfo().getEndPos());
        set.setRecordNum(unit->getReadCount());
        unit.reset();
        flowOut(std::move(set), id);
        return true;
    }

    /**
     * @brief Signal end of data.
     */
    void dryIn() override {
        dryOut();
    }
};

#endif  // GENIE_MPEGG_P_2_IMPORTER_H
