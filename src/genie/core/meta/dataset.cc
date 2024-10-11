/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/dataset.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "blockheader/disabled.h"
#include "blockheader/enabled.h"

#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::meta {

// ---------------------------------------------------------------------------------------------------------------------

Dataset::Dataset(const uint32_t _version, std::unique_ptr<BlockHeader> _headerCfg, std::string meta,
                 std::string protection)
    : version(_version),
      headerCfg(std::move(_headerCfg)),
      DT_metadata_value(std::move(meta)),
      DT_protection_value(std::move(protection)) {}

// ---------------------------------------------------------------------------------------------------------------------

Dataset::Dataset(const nlohmann::json& json)
    : version(json["version"]),
      DT_metadata_value(json["DT_metadata_value"]),
      DT_protection_value(json["DT_protection_value"]) {
    // Optional: dataset group metadata
    if (json.contains("dataset_group")) {
        dataset_group = DatasetGroup(json["dataset_group"]);
    }
    // Optional: reference metadata (aligned data only)
    if (json.contains("reference")) {
        reference = Reference(json["reference"]);
    }
    // Optional: labels
    if (json.contains("label_list")) {
        for (const auto& l : json["label_list"]) {
            label_list.emplace_back(l);
        }
    }
    // Presence of MIT and CC_mode flags signals blockheader_flag == true
    if (json.contains("MIT_flag") && json.contains("CC_mode_flag") && !json.contains("ordered_blocks_flag")) {
        headerCfg = std::make_unique<blockheader::Enabled>(json);
    } else if (!json.contains("MIT_flag") && !json.contains("CC_mode_flag") && json.contains("ordered_blocks_flag")) {
        headerCfg = std::make_unique<blockheader::Disabled>(json);
    } else {
        UTILS_DIE("Could not infer block header mode.");
    }
    // Optional: Access unit specific metadata
    if (json.contains("access_units")) {
        for (const auto& au : json["access_units"]) {
            access_units.emplace_back(au);
        }
    }
    // Optional: descriptor stream metadata
    if (json.contains("descriptor_streams")) {
        for (const auto& ds : json["descriptor_streams"]) {
            descriptor_streams.emplace_back(ds);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json Dataset::toJson() const {
    nlohmann::json ret;
    if (dataset_group) {
        ret["dataset_group"] = dataset_group->toJson();
    }
    if (reference) {
        ret["reference"] = reference->toJson();
    }
    for (const auto& l : label_list) {
        ret["label_list"].push_back(l.toJson());
    }
    ret["version"] = version;
    auto tmp = headerCfg->toJson();
    for (const auto& i : tmp.items()) {
        ret[i.key()] = i.value();
    }
    ret["DT_metadata_value"] = DT_metadata_value;
    ret["DT_protection_value"] = DT_protection_value;
    for (const auto& au : access_units) {
        ret["access_units"].push_back(au.toJson());
    }
    for (const auto& ds : descriptor_streams) {
        ret["descriptor_streams"].push_back(ds.toJson());
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::optional<DatasetGroup>& Dataset::getDataGroup() const { return dataset_group; }

// ---------------------------------------------------------------------------------------------------------------------

const std::optional<Reference>& Dataset::getReference() const { return reference; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Label>& Dataset::getLabels() const { return label_list; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Dataset::getVersion() const { return version; }

// ---------------------------------------------------------------------------------------------------------------------

const BlockHeader& Dataset::getHeader() const { return *headerCfg; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Dataset::getInformation() const { return DT_metadata_value; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Dataset::getProtection() const { return DT_protection_value; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<AccessUnit>& Dataset::getAUs() const { return access_units; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<DescriptorStream>& Dataset::getDSs() const { return descriptor_streams; }

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::setDataGroup(DatasetGroup dg) { dataset_group = std::move(dg); }

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::setReference(Reference ref) { reference = std::move(ref); }

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::addLabel(Label l) { label_list.emplace_back(std::move(l)); }

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::addAccessUnit(AccessUnit au) { access_units.emplace_back(std::move(au)); }

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::addDescriptorStream(DescriptorStream ds) { descriptor_streams.emplace_back(std::move(ds)); }

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::setMetadata(std::string meta) { DT_metadata_value = std::move(meta); }

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::setProtection(std::string prot) { DT_protection_value = std::move(prot); }

// ---------------------------------------------------------------------------------------------------------------------

Dataset::Dataset() {
    version = 0;
    headerCfg = std::make_unique<blockheader::Enabled>(false, false);
}

// ---------------------------------------------------------------------------------------------------------------------

void Dataset::setHeader(std::unique_ptr<BlockHeader> _hdr) { headerCfg = std::move(_hdr); }

// ---------------------------------------------------------------------------------------------------------------------

std::optional<DatasetGroup>& Dataset::getDataGroup() { return dataset_group; }

// ---------------------------------------------------------------------------------------------------------------------

std::optional<Reference>& Dataset::getReference() { return reference; }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<Label>& Dataset::getLabels() { return label_list; }

// ---------------------------------------------------------------------------------------------------------------------

std::string& Dataset::getInformation() { return DT_metadata_value; }

// ---------------------------------------------------------------------------------------------------------------------

std::string& Dataset::getProtection() { return DT_protection_value; }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<AccessUnit>& Dataset::getAUs() { return access_units; }

// ---------------------------------------------------------------------------------------------------------------------

std::vector<DescriptorStream>& Dataset::getDSs() { return descriptor_streams; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::meta

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
