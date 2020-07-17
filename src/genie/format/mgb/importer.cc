/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "importer.h"
#include <genie/util/watch.h>
#include "access_unit.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(std::istream& _file, core::ReferenceManager* manager, core::RefDecoder* refd, bool refOnly,
                   genie::core::stats::PerfStats* _stats)
    : core::ReferenceSource(manager),
      reader(_file),
      stats(_stats),
      factory(manager, this, refOnly),
      ref_manager(manager),
      decoder(refd) {}

// ---------------------------------------------------------------------------------------------------------------------

bool Importer::pump(size_t& id, std::mutex&) {
    util::Watch watch;
    boost::optional<mgb::AccessUnit> unit;
    util::Section sec{};
    {
        std::unique_lock<std::mutex> lock_guard(lock);
        unit = factory.read(reader);
        if (!unit) {
            return false;
        }
        sec.start = id;
        sec.length = unit->getReadCount();
        id += unit->getReadCount();
    }

    flowOut(convertAU(std::move(unit.get())), sec);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string Importer::getRef(bool raw, size_t f_pos, size_t start, size_t end) {
    AccessUnit au(0, 0, core::record::ClassType::NONE, 0, core::parameter::DataUnit::DatasetType::REFERENCE, 0, 0, 0);
    std::string ret;
    {
        std::lock_guard<std::mutex> f_lock(this->lock);
        size_t oldPos = reader.getPos();

        if (raw) {
            reader.setPos(f_pos + start);
            ret.resize(end - start);
            reader.readBuffer(&ret[0], ret.length());
        } else {
            reader.setPos(f_pos);
            au = AccessUnit(factory.getParams(), reader, false);
        }

        reader.setPos(oldPos);
    }

    if (!raw) {
        ret = decoder->decode(convertAU(std::move(au)));
    }

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit Importer::convertAU(mgb::AccessUnit&& au) {
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
    if (unit.getClass() != core::record::ClassType::CLASS_U && !set.getParameters().isComputedReference()) {
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

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
