/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/capsulator/main.h"
#include <fstream>
#include <iostream>
#include "apps/genie/capsulator/program-options.h"
#include "format/mgb/mgb_file.h"
#include "genie/format/mgb/raw_reference.h"
#include "genie/format/mpegg_p1/mgg_file.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace capsulator {

// ---------------------------------------------------------------------------------------------------------------------

enum class ErrorCode : uint8_t { success = 0, failure = 1 };
enum class DataUnitType : uint8_t { RAW_REF = 0, PAR_SET = 1, ACC_UNT = 2 };

// ---------------------------------------------------------------------------------------------------------------------

struct EncapsulatedDataset {
    std::ifstream reader;
    genie::format::mgb::MgbFile mgb_file;
    genie::core::meta::Dataset meta;

    std::vector<genie::format::mpegg_p1::Dataset> datasets;

    explicit EncapsulatedDataset(const std::string& input_file, genie::core::MPEGMinorVersion version)
        : reader(input_file), mgb_file(&reader) {
        for (uint8_t multiple_alignment = 0; multiple_alignment < 2; ++multiple_alignment) {
            for (uint8_t pos40 = 0; pos40 < 2; ++pos40) {
                for (uint8_t data_type = 0; data_type < uint8_t(genie::core::parameter::DataUnit::DatasetType::COUNT);
                     ++data_type) {
                    for (uint8_t alphabet_type = 0; alphabet_type < uint8_t(genie::core::AlphabetID::COUNT);
                         ++alphabet_type) {
                        auto param_ids =
                            mgb_file.collect_param_ids(bool(multiple_alignment), bool(pos40),
                                                       genie::core::parameter::DataUnit::DatasetType(data_type),
                                                       genie::core::AlphabetID(alphabet_type));
                        if (param_ids.empty()) {
                            continue;
                        }
                        auto dataset = genie::format::mpegg_p1::Dataset(mgb_file, meta, version, param_ids);
                        datasets.emplace_back(std::move(dataset));
                    }
                }
            }
        }
    }
};

struct EncapsulatedDatasetGroup {
    boost::optional<genie::format::mpegg_p1::DatasetGroupMetadata> group_meta;
    boost::optional<genie::format::mpegg_p1::DatasetGroupProtection> group_protection;
    std::vector<genie::format::mpegg_p1::Reference> references;
    std::vector<std::unique_ptr<EncapsulatedDataset>> datasets;

    void mergeMetadata(genie::core::MPEGMinorVersion version) {
        std::string meta;
        std::string protection;

        for (auto& d : datasets) {
            if (d->meta.getDataGroup() == boost::none) {
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
                UTILS_DIE_IF(!meta.empty() && !d->meta.getDataGroup()->getInformation().empty(),
                             "Contradicting config");
                if (meta.empty()) {
                    meta = std::move(d->meta.getDataGroup()->getInformation());
                }
            }
        }

        if (!meta.empty()) {
            group_meta = genie::format::mpegg_p1::DatasetGroupMetadata(0, std::move(meta), version);
        }

        if (!protection.empty()) {
            group_protection = genie::format::mpegg_p1::DatasetGroupProtection(0, std::move(protection), version);
        }
    }

    void mergeReferences(genie::core::MPEGMinorVersion version) {
        for (auto& d : datasets) {
            if(d->meta.getReference() == boost::none) {
                continue;
            }
            genie::format::mpegg_p1::Reference ref(0, 0, std::move(*d->meta.getReference()), version);
            bool found = false;
            for(size_t i = 0; i < references.size(); ++i) {
                if (references[i] == ref) {
                    for(auto& d2 : d->datasets) {
                        d2.patchRefID(ref.getReferenceID(), i);
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                for(auto& d2 : d->datasets) {
                    d2.patchRefID(ref.getReferenceID(), references.size());
                }
                ref.patchRefID(ref.getReferenceID(), references.size());
                references.emplace_back(std::move(ref));
            }
        }
    }

    EncapsulatedDatasetGroup(const std::vector<std::string>& input_files, genie::core::MPEGMinorVersion version) {
        datasets.reserve(input_files.size());
        for (const auto& i : input_files) {
            datasets.emplace_back(genie::util::make_unique<EncapsulatedDataset>(i, version));
        }
        size_t index = 0;
        for (auto& d : datasets) {
            for (auto& d2 : d->datasets) {
                d2.patchID(0, index++);
            }
        }

        mergeMetadata(version);

        mergeReferences(version);
    }
};


ErrorCode encapsulate(ProgramOptions& options) {
    auto version = genie::core::MPEGMinorVersion::V2000;

    //   mgb_file.remove_class(genie::core::record::ClassType::CLASS_N);
    //   mgb_file.remove_class(genie::core::record::ClassType::CLASS_M);
    //   mgb_file.select_mapping_range(0, 54111088, 101380838);
    //   mgb_file.sort_by_class();

    // mgb_file.print_debug(std::cout, 100);

    genie::format::mpegg_p1::MggFile mpegg_file;
    mpegg_file.addBox(genie::util::make_unique<genie::format::mpegg_p1::FileHeader>(version));
    auto group = genie::util::make_unique<genie::format::mpegg_p1::DatasetGroup>(0, 0, version);

    mpegg_file.addBox(std::move(group));

    std::ofstream outstream(options.outputFile);
    genie::util::BitWriter writer(&outstream);

    mpegg_file.print_debug(std::cout, 100);

    mpegg_file.write(writer);

    return ErrorCode::success;
}

// ---------------------------------------------------------------------------------------------------------------------

boost::optional<genie::core::meta::DatasetGroup> decapsulate_dataset_group(genie::format::mpegg_p1::DatasetGroup* grp) {
    boost::optional<genie::core::meta::DatasetGroup> meta_group;
    if (grp->hasMetadata()) {
        if (meta_group == boost::none) {
            meta_group =
                genie::core::meta::DatasetGroup(grp->getHeader().getID(), grp->getHeader().getVersion(), "", "");
        }
        meta_group->setMetadata(grp->getMetadata().decapsulate());
    }

    if (grp->hasProtection()) {
        if (meta_group == boost::none) {
            meta_group =
                genie::core::meta::DatasetGroup(grp->getHeader().getID(), grp->getHeader().getVersion(), "", "");
        }
        meta_group->setProtection(grp->getProtection().decapsulate());
    }
    return meta_group;
}

std::map<uint8_t, genie::core::meta::Reference> decapsulate_references(genie::format::mpegg_p1::DatasetGroup* grp) {
    std::map<uint8_t, std::string> ref_metadata;

    for (auto& m : grp->getReferenceMetadata()) {
        ref_metadata.emplace(std::make_pair(m.getReferenceID(), m.decapsulate()));
    }

    std::map<uint8_t, genie::core::meta::Reference> references;

    for (auto& m : grp->getReferences()) {
        auto it = ref_metadata.find(m.getReferenceID());
        std::string meta;
        if (it != ref_metadata.end()) {
            meta = std::move(it->second);
        }
        references.emplace(std::make_pair(m.getReferenceID(), m.decapsulate(std::move(meta))));
    }
    return references;
}

ErrorCode decapsulate(ProgramOptions& options) {
    std::ifstream reader(options.inputFile);

    std::string global_output_prefix = options.outputFile.substr(0, options.outputFile.find_last_of('.'));

    genie::format::mpegg_p1::MggFile mpegg_file(&reader);
    for (auto& box : mpegg_file.getBoxes()) {
        auto* grp = dynamic_cast<genie::format::mpegg_p1::DatasetGroup*>(box.get());
        if (!grp) {
            continue;
        }

        auto meta_group = decapsulate_dataset_group(grp);

        auto meta_references = decapsulate_references(grp);

        for (auto& dt : grp->getDatasets()) {
            std::string local_output_prefix = global_output_prefix + "." + std::to_string(grp->getHeader().getID()) +
                                              "." + std::to_string(dt.getHeader().getDatasetID());
            genie::format::mgb::MgbFile mgb_file;
            genie::core::meta::Dataset meta;

            meta.setHeader(dt.getHeader().decapsulate());

            if (!dt.getHeader().getReferenceOptions().getSeqIDs().empty()) {
                auto it = meta_references.find(dt.getHeader().getReferenceOptions().getReferenceID());
                if (it != meta_references.end()) {
                    meta.setReference(it->second);
                }
            }

            if (meta_group != boost::none) {
                meta.setDataGroup(*meta_group);
            }
            if (grp->hasLabelList()) {
                auto labels = grp->getLabelList().decapsulate(dt.getHeader().getDatasetID());
                for (auto& l : labels) {
                    meta.addLabel(std::move(l));
                }
            }
            for (auto& p : dt.getParameterSets()) {
                mgb_file.addUnit(genie::util::make_unique<genie::core::parameter::ParameterSet>(p.descapsulate()));
            }
            for (auto& au : dt.getAccessUnits()) {
                genie::core::meta::AccessUnit meta_au(au.getHeader().getHeader().getID());
                bool has_meta = false;
                if (au.hasInformation()) {
                    has_meta = true;
                    meta_au.setInformation(au.getInformation().decapsulate());
                }
                if (au.hasProtection()) {
                    has_meta = true;
                    meta_au.setProtection(au.getProtection().decapsulate());
                }
                mgb_file.addUnit(genie::util::make_unique<genie::format::mgb::AccessUnit>(au.decapsulate()));
                if (has_meta) {
                    meta.addAccessUnit(std::move(meta_au));
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
                    meta.addDescriptorStream(genie::core::meta::DescriptorStream(
                        size_t(ds.getHeader().getDescriptorID()), ds.getProtection().decapsulate()));
                }
            }

            auto meta_json = meta.toJson().dump(4);
            std::cout << meta_json.size() << std::endl;
            std::ofstream json_file(local_output_prefix + ".mgb.json");
            json_file.write(meta_json.data(), meta_json.length());

            std::ofstream mgb_output_file(local_output_prefix + ".mgb");
            genie::util::BitWriter mgb_output_writer(&mgb_output_file);
            mgb_file.write(mgb_output_writer);
        }
    }

    mpegg_file.print_debug(std::cout, 2);

    return ErrorCode::success;
}

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    ProgramOptions pOpts(argc, argv);

    genieapp::capsulator::ErrorCode ret = genieapp::capsulator::ErrorCode::success;
    /// Encapsulate
    if (pOpts.inputFile.substr(pOpts.inputFile.length() - 3) == "mgb" &&
        pOpts.outputFile.substr(pOpts.outputFile.length() - 3) == "mgg") {
        ret = encapsulate(pOpts);
        /// Decapsulate
    } else if (pOpts.inputFile.substr(pOpts.inputFile.length() - 3) == "mgg" &&
               pOpts.outputFile.substr(pOpts.outputFile.length() - 3) == "mgb") {
        ret = decapsulate(pOpts);
    }
    if (ret != genieapp::capsulator::ErrorCode::success) {
        UTILS_DIE("ERROR");
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace capsulator
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
