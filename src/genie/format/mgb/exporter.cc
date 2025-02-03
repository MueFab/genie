/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgb/exporter.h"
#include <iostream>
#include <string>
#include <utility>
#include "genie/format/mgb/raw_reference.h"
#include "genie/util/stop_watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

Exporter::Exporter(std::ostream* _file) : writer(_file), id_ctr(0) {}

// ---------------------------------------------------------------------------------------------------------------------

void Exporter::flowIn(core::AccessUnit&& t, const util::Section& id) {
    util::Watch watch;
    core::AccessUnit data = std::move(t);
    util::OrderedSection section(&lock, id);
    getStats().add(data.getStats());
    auto parameter_id = static_cast<uint8_t>(parameter_stash.size());
    core::parameter::ParameterSet out_set(parameter_id, parameter_id, std::move(data.getParameters()));
    mgb::RawReference ref;
    for (const auto& p : data.getRefToWrite()) {
        auto string = *data.getReferenceExcerpt().getChunkAt(p.first);
        auto substr = string.substr(p.first % core::ReferenceManager::getChunkSize());
        substr = string.substr(0, p.second - p.first);
        mgb::RawReferenceSequence refseq(data.getReference(), p.first, std::move(substr));
        ref.addSequence(std::move(refseq));
    }
    if (!ref.isEmpty()) {
        for (auto& r : ref) {
            std::cerr << "Writing Ref " << r.getSeqID() << ":" << r.getStart() << "-" << r.getEnd() << "..."
                      << std::endl;
        }
        ref.write(writer);
        ref = mgb::RawReference();
    }

    if (data.getNumReads() == 0) {
        return;
    }

    bool found = false;
    for (const auto& p : parameter_stash) {
        if (out_set == p) {
            found = true;
            parameter_id = p.getID();
        }
    }

    if (!found) {
        std::cerr << "Writing PS " << uint32_t(out_set.getID()) << "..." << std::endl;
        out_set.write(writer);
        parameter_stash.push_back(out_set);
    }

    auto datasetType = data.getClassType() != core::record::ClassType::CLASS_U
                           ? core::parameter::DataUnit::DatasetType::ALIGNED
                           : (data.isReferenceOnly() ? core::parameter::DataUnit::DatasetType::REFERENCE
                                                     : core::parameter::DataUnit::DatasetType::NON_ALIGNED);

    mgb::AccessUnit au((uint32_t)id_ctr, parameter_id, data.getClassType(), (uint32_t)data.getNumReads(), datasetType,
                       32, false, core::AlphabetID::ACGTN);
    if (data.isReferenceOnly()) {
        au.getHeader().setRefCfg(RefCfg(data.getReference(), data.getReferenceExcerpt().getGlobalStart(),
                                        data.getReferenceExcerpt().getGlobalEnd() - 1, 32));
    }
    if (au.getHeader().getClass() != core::record::ClassType::CLASS_U) {
        au.getHeader().setAuTypeCfg(
            AuTypeCfg(data.getReference(), data.getMinPos(), data.getMaxPos(), data.getParameters().getPosSize()));
    }
    for (uint8_t descriptor = 0; descriptor < (uint8_t)core::getDescriptors().size(); ++descriptor) {
        if (data.get(core::GenDesc(descriptor)).isEmpty()) {
            continue;
        }
        au.addBlock(Block(descriptor, std::move(data.get(core::GenDesc(descriptor)))));
    }

    au.debugPrint(parameter_stash[au.getHeader().getParameterID()].getEncodingSet());

    au.write(writer);
    id_ctr++;
    getStats().addDouble("time-mgb-export", watch.check());
}

// ---------------------------------------------------------------------------------------------------------------------

void Exporter::SkipIn(const Section& id) { util::OrderedSection sec(&lock, id); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
