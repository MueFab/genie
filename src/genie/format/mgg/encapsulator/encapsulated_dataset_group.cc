/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/encapsulator/encapsulated_dataset_group.h"
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::encapsulator {

// ---------------------------------------------------------------------------------------------------------------------

void EncapsulatedDatasetGroup::patchID(const uint8_t id) {
    if (group_meta != std::nullopt) {
        group_meta->patchID(id);
    }
    if (group_protection != std::nullopt) {
        group_protection->patchID(id);
    }
    for (auto& r : references) {
        r.patchID(id);
    }
    for (auto& r : reference_meta) {
        r.patchID(id);
    }
    for (const auto& r : datasets) {
        for (auto& d : r->datasets) {
            d.patchID(id, d.getHeader().getDatasetID());
        }
    }
    if (labelList != std::nullopt) {
        labelList->patchID(id);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void EncapsulatedDatasetGroup::mergeMetadata(const core::MPEGMinorVersion version) {
    std::string meta;
    std::string protection;

    for (const auto& d : datasets) {
        if (d->meta.getDataGroup() == std::nullopt) {
            continue;
        }
        if (protection != d->meta.getDataGroup()->getProtection()) {
            UTILS_DIE_IF(!protection.empty() && !d->meta.getDataGroup()->getProtection().empty(),
                         "Contradicting config");
            if (protection.empty()) {
                protection = std::move(d->meta.getDataGroup()->getProtection());
            }
        }
        if (meta != d->meta.getDataGroup()->getInformation()) {
            UTILS_DIE_IF(!meta.empty() && !d->meta.getDataGroup()->getInformation().empty(), "Contradicting config");
            if (meta.empty()) {
                meta = std::move(d->meta.getDataGroup()->getInformation());
            }
        }
    }

    if (!meta.empty()) {
        group_meta = DatasetGroupMetadata(0, std::move(meta), version);
    }

    if (!protection.empty()) {
        group_protection = DatasetGroupProtection(0, std::move(protection), version);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void EncapsulatedDatasetGroup::mergeReferences(const core::MPEGMinorVersion version) {
    for (const auto& d : datasets) {
        if (d->meta.getReference() == std::nullopt) {
            continue;
        }
        ReferenceMetadata ref_meta(0, 0, std::move(d->meta.getReference()->getInformation()));
        Reference ref(0, 0, std::move(*d->meta.getReference()), version);
        bool found = false;
        for (size_t i = 0; i < references.size(); ++i) {
            if (references[i] == ref) {
                if (reference_meta[i].getReferenceMetadataValue().empty()) {
                    reference_meta[i] = ReferenceMetadata(0, static_cast<uint8_t>(i), ref_meta.decapsulate());
                } else {
                    UTILS_DIE_IF(
                        !(ref_meta.getReferenceMetadataValue() == reference_meta[i].getReferenceMetadataValue()),
                        "Reference Meta mismatch");
                }

                for (auto& d2 : d->datasets) {
                    d2.patchRefID(ref.getReferenceID(), static_cast<uint8_t>(i));
                }
                found = true;
                break;
            }
        }
        if (!found) {
            for (auto& d2 : d->datasets) {
                d2.patchRefID(ref.getReferenceID(), static_cast<uint8_t>(references.size()));
            }
            ref.patchRefID(ref.getReferenceID(), static_cast<uint8_t>(references.size()));
            ref_meta.patchRefID(ref_meta.getReferenceID(), static_cast<uint8_t>(references.size()));
            references.emplace_back(std::move(ref));
            reference_meta.emplace_back(std::move(ref_meta));
        }
    }

    for (auto it = reference_meta.begin(); it != reference_meta.end();) {
        if (it->getReferenceMetadataValue().empty()) {
            it = reference_meta.erase(it);
        } else {
            ++it;
        }
    }

    for (const auto& d : datasets) {
        for (auto& d2 : d->datasets) {
            if (d2.getHeader().getDatasetType() != core::parameter::DataUnit::DatasetType::ALIGNED) {
                continue;
            }

            const auto ref_id = d2.getHeader().getReferenceOptions().getReferenceID();
            for (auto& reference : references) {
                if (ref_id == reference.getReferenceID()) {
                    for (auto& s : reference.getSequences()) {
                        d2.getHeader().addRefSequence(ref_id, s.getID(), 0, 0);
                    }
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void EncapsulatedDatasetGroup::mergeLabels() {
    std::map<std::string, std::vector<LabelDataset>> labels;
    for (const auto& d : datasets) {
        for (auto& l : d->meta.getLabels()) {
            for (auto& s : d->datasets) {
                labels[l.getID()].emplace_back(s.getHeader().getDatasetID(), l);
            }
        }
    }

    for (auto& s : labels) {
        Label label(s.first);
        for (auto& l : s.second) {
            label.addDataset(std::move(l));
        }
        if (labelList == std::nullopt) {
            labelList = LabelList(0);
        }
        labelList->addLabel(std::move(label));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

EncapsulatedDatasetGroup::EncapsulatedDatasetGroup(const std::vector<std::string>& input_files,
                                                   core::MPEGMinorVersion version) {
    datasets.reserve(input_files.size());
    for (const auto& i : input_files) {
        datasets.emplace_back(std::make_unique<EncapsulatedDataset>(i, version));
    }
    size_t index = 0;
    for (const auto& d : datasets) {
        for (auto& d2 : d->datasets) {
            d2.patchID(0, static_cast<uint16_t>(index++));
        }
    }

    mergeMetadata(version);

    mergeReferences(version);

    mergeLabels();
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroup EncapsulatedDatasetGroup::assemble(const core::MPEGMinorVersion version) {
    DatasetGroup ret(0, 0, version);
    for (auto& r : references) {
        ret.addReference(std::move(r));
    }
    for (auto& r : reference_meta) {
        ret.addReferenceMeta(std::move(r));
    }
    if (labelList != std::nullopt) {
        ret.setLabels(std::move(*labelList));
    }
    if (group_meta != std::nullopt) {
        ret.setMetadata(std::move(*group_meta));
    }
    if (group_protection != std::nullopt) {
        ret.setProtection(std::move(*group_protection));
    }

    for (const auto& d : datasets) {
        for (auto& d2 : d->datasets) {
            ret.addDataset(std::move(d2));
        }
    }

    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::encapsulator

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
