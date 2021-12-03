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
#include "genie/util/watch.h"

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
    size_t parameter_id = parameter_stash.size();
    data.getParameters().setID((uint8_t)parameter_id);
    data.getParameters().setParentID((uint8_t)parameter_id);
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
        if (data.getParameters() == p) {
            found = true;
            parameter_id = p.getID();
        }
    }

    if (!found) {
        std::cerr << "Writing PS " << uint32_t(data.getParameters().getID()) << "..." << std::endl;
        data.getParameters().write(writer);
        parameter_stash.push_back(data.getParameters());
    }

    auto datasetType = data.getClassType() != core::record::ClassType::CLASS_U
                           ? core::parameter::DataUnit::DatasetType::ALIGNED
                           : (data.isReferenceOnly() ? core::parameter::DataUnit::DatasetType::REFERENCE
                                                     : core::parameter::DataUnit::DatasetType::NON_ALIGNED);

    mgb::AccessUnit au((uint32_t)id_ctr, (uint8_t)parameter_id, data.getClassType(), (uint32_t)data.getNumReads(),
                       datasetType, 32, 32, 0);
    if (data.isReferenceOnly()) {
        au.setRefCfg(RefCfg(data.getReference(), data.getReferenceExcerpt().getGlobalStart(),
                            data.getReferenceExcerpt().getGlobalEnd() - 1, 32));
    }
    if (au.getClass() != core::record::ClassType::CLASS_U) {
        au.setAuTypeCfg(
            AuTypeCfg(data.getReference(), data.getMinPos(), data.getMaxPos(), data.getParameters().getPosSize()));
    }
    for (uint8_t descriptor = 0; descriptor < (uint8_t)core::getDescriptors().size(); ++descriptor) {
        if (data.get(core::GenDesc(descriptor)).isEmpty()) {
            continue;
        }
        au.addBlock(Block(descriptor, std::move(data.get(core::GenDesc(descriptor)))));
    }
    std::string lut[] = {"NONE", "P", "N", "M", "I", "HM", "U"};
    std::cerr << "Writing AU " << au.getID() << ": class " << lut[(int)au.getClass()];
    if (au.getClass() != genie::core::record::ClassType::CLASS_U) {
        std::cerr << ", Position [" << au.getAlignmentInfo().getRefID() << "-" << au.getAlignmentInfo().getStartPos()
                  << ":" << au.getAlignmentInfo().getEndPos() << "]";
    }
    std::cerr << ", " << au.getReadCount() << " records";
    auto& tmp_ps = parameter_stash[au.getParameterID()];

    if (au.getClass() == genie::core::record::ClassType::CLASS_U) {
        if (!tmp_ps.isComputedReference()) {
            std::cerr << " (Low Latency)";
        } else {
            if (tmp_ps.getComputedRef().getAlgorithm() ==
                core::parameter::ComputedRef::Algorithm::GLOBAL_ASSEMBLY) {
                std::cerr << " (Global Assembly)";
            } else {
                UTILS_DIE(
                    "Computed ref not supported: " +
                    std::to_string(static_cast<int>(
                        tmp_ps.getComputedRef().getAlgorithm())));
            }
        }
    } else {
        if (!tmp_ps.isComputedReference()) {
            std::cerr << " (Reference)";
        } else {
            if (tmp_ps.getComputedRef().getAlgorithm() ==
                core::parameter::ComputedRef::Algorithm::LOCAL_ASSEMBLY) {
                std::cerr << " (Local Assembly)";
            } else {
                UTILS_DIE(
                    "Computed ref not supported: " +
                    std::to_string(static_cast<int>(
                        tmp_ps.getComputedRef().getAlgorithm())));
            }
        }
    }
    std::cerr << "..." << std::endl;


    au.write(writer);
    id_ctr++;
    getStats().addDouble("time-mgb-export", watch.check());
}

// ---------------------------------------------------------------------------------------------------------------------

void Exporter::skipIn(const genie::util::Section& id) { util::OrderedSection sec(&lock, id); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
