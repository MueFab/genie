/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MGB_IMPORTER_H
#define GENIE_MGB_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/format-importer-compressed.h>
#include <genie/core/ref-decoder.h>
#include <genie/core/reference-source.h>
#include <genie/core/stats/perf-stats.h>
#include <genie/util/bitreader.h>
#include <genie/util/ordered-section.h>
#include <genie/util/original-source.h>
#include <genie/util/source.h>
#include <map>
#include "data-unit-factory.h"
#include "format-importer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

/**
 *
 */
class Importer : public core::FormatImporterCompressed, public core::ReferenceSource {
   private:
    util::BitReader reader;                //!<
    std::mutex lock;                       //!<
    genie::core::stats::PerfStats* stats;  //!<
    mgb::DataUnitFactory factory;          //!<
    core::ReferenceManager* ref_manager;
    core::RefDecoder* decoder;

    core::AccessUnit convertAU(mgb::AccessUnit&& au) {
        auto unit = std::move(au);
        auto paramset = factory.getParams(unit.getParameterID());
        core::AccessUnit set(std::move(paramset), unit.getReadCount());

        for (auto& b : unit.getBlocks()) {
            set.set(core::GenDesc(b.getDescriptorID()), b.movePayload());
        }
        if (unit.getClass() != core::record::ClassType::CLASS_U) {
            set.setReference(unit.getAlignmentInfo().getRefID());
            set.setMinPos(unit.getAlignmentInfo().getStartPos());
            set.setMaxPos(unit.getAlignmentInfo().getEndPos());
        } else {
            set.setReference(0);
            set.setMinPos(0);
            set.setMaxPos(0);
        }
        set.setNumReads(unit.getReadCount());
        set.setClassType(unit.getClass());
        if (unit.getClass() != core::record::ClassType::CLASS_U) {
            auto seqs = ref_manager->getSequences();
            auto cur_seq = ref_manager->ID2Ref(unit.getAlignmentInfo().getRefID());
            if (std::find(seqs.begin(), seqs.end(), cur_seq) != seqs.end()) {
                set.setReference(ref_manager->load(cur_seq, unit.getAlignmentInfo().getStartPos(),
                                                   unit.getAlignmentInfo().getEndPos() + 1),
                                 {});
            }
        }
        return set;
    }

   public:
    /**
     *
     * @param _file
     * @param _stats
     */
    explicit Importer(std::istream& _file, core::ReferenceManager* manager, core::RefDecoder* refd,
                      genie::core::stats::PerfStats* _stats = nullptr);

    /**
     *
     * @param id
     * @param lock
     * @return
     */
    bool pump(size_t& id, std::mutex& lock) override;

    std::string getRef(bool raw, size_t f_pos, size_t start, size_t end) {
        std::lock_guard<std::mutex> f_lock(this->lock);
        size_t oldPos = reader.getPos();
        std::string ret;

        reader.setPos(f_pos + start);
        if (raw) {
            ret.resize(end - start);
            reader.readBuffer(&ret[0], ret.length());
        } else {

            ret = decoder->decode(convertAU(AccessUnit(factory.getParams(), reader, false)));
        }

        reader.setPos(oldPos);
        return ret;
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
