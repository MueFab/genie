/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/encapsulator/decapsulated_dataset_group.h"
#include <map>
#include <memory>
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::encapsulator {

// ---------------------------------------------------------------------------------------------------------------------

DecapsulatedDatasetGroup::DecapsulatedDatasetGroup(DecapsulatedDatasetGroup&& other) noexcept {
    id = other.id;
    meta_group = std::move(other.meta_group);
    meta_references = std::move(other.meta_references);
    data = std::move(other.data);
}

// ---------------------------------------------------------------------------------------------------------------------

DecapsulatedDatasetGroup::DecapsulatedDatasetGroup(DatasetGroup* grp) {
    id = grp->getHeader().getID();
    meta_group = decapsulate_dataset_group(grp);
    meta_references = decapsulate_references(grp);

    for (auto& dt : grp->getDatasets()) {
        data.emplace(dt.getHeader().getDatasetID(), decapsulate_dataset(dt, meta_group, meta_references, grp));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DecapsulatedDatasetGroup::getID() const { return id; }

// ---------------------------------------------------------------------------------------------------------------------

bool DecapsulatedDatasetGroup::hasMetaGroup() const { return meta_group != std::nullopt; }

// ---------------------------------------------------------------------------------------------------------------------

core::meta::DatasetGroup& DecapsulatedDatasetGroup::getMetaGroup() { return *meta_group; }

// ---------------------------------------------------------------------------------------------------------------------

std::map<uint8_t, core::meta::Reference>& DecapsulatedDatasetGroup::getMetaReferences() { return meta_references; }

// ---------------------------------------------------------------------------------------------------------------------

std::map<uint64_t, std::pair<mgb::MgbFile, core::meta::Dataset>>& DecapsulatedDatasetGroup::getData() { return data; }

// ---------------------------------------------------------------------------------------------------------------------

std::optional<core::meta::DatasetGroup> DecapsulatedDatasetGroup::decapsulate_dataset_group(DatasetGroup* grp) {
    std::optional<core::meta::DatasetGroup> meta_group;
    if (grp->hasMetadata()) {
        if (meta_group == std::nullopt) {
            meta_group = core::meta::DatasetGroup(grp->getHeader().getID(), grp->getHeader().getVersion(), "", "");
        }
        meta_group->setMetadata(grp->getMetadata().decapsulate());
    }

    if (grp->hasProtection()) {
        if (meta_group == std::nullopt) {
            meta_group = core::meta::DatasetGroup(grp->getHeader().getID(), grp->getHeader().getVersion(), "", "");
        }
        meta_group->setProtection(grp->getProtection().decapsulate());
    }
    return meta_group;
}

// ---------------------------------------------------------------------------------------------------------------------

std::map<uint8_t, core::meta::Reference> DecapsulatedDatasetGroup::decapsulate_references(DatasetGroup* grp) {
    std::map<uint8_t, std::string> ref_metadata;

    for (auto& m : grp->getReferenceMetadata()) {
        ref_metadata.emplace(m.getReferenceID(), m.decapsulate());
    }

    std::map<uint8_t, core::meta::Reference> references;

    for (auto& m : grp->getReferences()) {
        auto it = ref_metadata.find(m.getReferenceID());
        std::string meta;
        if (it != ref_metadata.end()) {
            meta = std::move(it->second);
        }
        references.emplace(m.getReferenceID(), m.decapsulate(std::move(meta)));
    }
    return references;
}

// ---------------------------------------------------------------------------------------------------------------------

std::pair<mgb::AccessUnit, std::optional<core::meta::AccessUnit>> DecapsulatedDatasetGroup::decapsulate_AU(
    AccessUnit& au) {
    core::meta::AccessUnit meta_au(au.getHeader().getHeader().getID());
    bool has_meta = false;
    if (au.hasInformation()) {
        has_meta = true;
        meta_au.setInformation(au.getInformation().decapsulate());
    }
    if (au.hasProtection()) {
        has_meta = true;
        meta_au.setProtection(au.getProtection().decapsulate());
    }
    std::pair<mgb::AccessUnit, std::optional<core::meta::AccessUnit>> ret(au.decapsulate(), std::nullopt);
    if (has_meta) {
        ret.second = std::move(meta_au);
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::pair<mgb::MgbFile, core::meta::Dataset> DecapsulatedDatasetGroup::decapsulate_dataset(
    Dataset& dt, std::optional<core::meta::DatasetGroup>& meta_group,
    std::map<uint8_t, core::meta::Reference>& meta_references, DatasetGroup* grp) {
    mgb::MgbFile mgb_file;
    core::meta::Dataset meta;

    meta.setHeader(dt.getHeader().decapsulate());

    if (!dt.getHeader().getReferenceOptions().getSeqIDs().empty()) {
        const auto it = meta_references.find(dt.getHeader().getReferenceOptions().getReferenceID());
        if (it != meta_references.end()) {
            meta.setReference(it->second);
        }
    }

    if (meta_group != std::nullopt) {
        meta.setDataGroup(*meta_group);
    }
    if (grp->hasLabelList()) {
        auto labels = grp->getLabelList().decapsulate(dt.getHeader().getDatasetID());
        for (auto& l : labels) {
            meta.addLabel(std::move(l));
        }
    }
    for (auto& p : dt.getParameterSets()) {
        mgb_file.addUnit(std::make_unique<core::parameter::ParameterSet>(p.descapsulate()));
    }
    for (auto& au : dt.getAccessUnits()) {
        auto decap_au = decapsulate_AU(au);
        mgb_file.addUnit(std::make_unique<mgb::AccessUnit>(std::move(decap_au.first)));
        if (decap_au.second != std::nullopt) {
            meta.addAccessUnit(std::move(*decap_au.second));
        }
    }

    if (dt.hasMetadata()) {
        meta.setMetadata(dt.getMetadata().decapsulate());
    }

    if (dt.hasProtection()) {
        meta.setProtection(dt.getProtection().decapsulate());
    }

    for (auto& ds : dt.getDescriptorStreams()) {
        if (ds.hasProtection()) {
            meta.addDescriptorStream(core::meta::DescriptorStream(static_cast<size_t>(ds.getHeader().getDescriptorID()),
                                                                  ds.getProtection().decapsulate()));
        }
    }

    return std::make_pair(std::move(mgb_file), std::move(meta));
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
