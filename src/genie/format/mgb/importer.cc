/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgb/importer.h"
#include <string>
#include <utility>
#include "genie/format/mgb/access_unit.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgb {

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(std::istream& _file, core::ReferenceManager* manager, core::RefDecoder* refd, bool refOnly)
    : core::ReferenceSource(manager),
      reader(_file),
      factory(manager, this, refOnly),
      ref_manager(manager),
      decoder(refd) {}

// ---------------------------------------------------------------------------------------------------------------------

bool Importer::pump(uint64_t& id, std::mutex&) {
    util::Watch watch;
    std::optional<mgb::AccessUnit> unit;
    util::Section sec{};
    {
        std::unique_lock<std::mutex> lock_guard(lock);
        unit = factory.read(reader);
        if (!unit) {
            return false;
        }
        sec.start = id;
        sec.length = unit->getHeader().getReadCount();
        id += unit->getHeader().getReadCount();
    }

    flowOut(convertAU(std::move(unit.value())), sec);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string Importer::getRef(bool raw, size_t f_pos, size_t start, size_t end) {
    AccessUnit au(0, 0, core::record::ClassType::NONE, 0, core::parameter::DataUnit::DatasetType::REFERENCE, 0, false,
                  core::AlphabetID::ACGTN);
    std::string ret;
    {
        std::lock_guard<std::mutex> f_lock(this->lock);
        size_t oldPos = reader.getPos();

        if (raw) {
            reader.setPos(f_pos + start);
            ret.resize(end - start);
            reader.readBypass(&ret[0], ret.length());
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
    auto paramset = factory.getParams(unit.getHeader().getParameterID());
    core::AccessUnit set(std::move(paramset), unit.getHeader().getReadCount());

    for (auto& b : unit.getBlocks()) {
        set.set(core::GenDesc(b.getDescriptorID()), b.movePayload());
    }
    if (unit.getHeader().getClass() != core::record::ClassType::CLASS_U) {
        set.setReference(unit.getHeader().getAlignmentInfo().getRefID());
        set.setMinPos(unit.getHeader().getAlignmentInfo().getStartPos());
        set.setMaxPos(unit.getHeader().getAlignmentInfo().getEndPos());
        ref_manager->validateRefID(set.getReference());
    } else {
        set.setReference(0);
        set.setMinPos(0);
        set.setMaxPos(0);
    }
    set.setNumReads(unit.getHeader().getReadCount());
    set.setClassType(unit.getHeader().getClass());
    if (unit.getHeader().getClass() != core::record::ClassType::CLASS_U && !set.getParameters().isComputedReference()) {
        auto seqs = ref_manager->getSequences();
        auto cur_seq = ref_manager->ID2Ref(unit.getHeader().getAlignmentInfo().getRefID());
        if (std::find(seqs.begin(), seqs.end(), cur_seq) != seqs.end()) {
            set.setReference(ref_manager->load(cur_seq, unit.getHeader().getAlignmentInfo().getStartPos(),
                                               unit.getHeader().getAlignmentInfo().getEndPos() + 1),
                             {});
        }
    }
    return set;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgb

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
