/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/meta/reference.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/meta/external-ref/fasta.h"
#include "genie/core/meta/external-ref/mpeg.h"
#include "genie/core/meta/external-ref/raw.h"
#include "genie/core/meta/internal-ref.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace meta {

// ---------------------------------------------------------------------------------------------------------------------

Reference::Reference(std::string name, uint32_t major, uint32_t minor, uint32_t patch, std::unique_ptr<RefBase> base,
                     std::string metadata)
    : reference_name(std::move(name)),
      reference_major_version(major),
      reference_minor_version(minor),
      reference_patch_version(patch),
      ref(std::move(base)),
      reference_metadata_value(std::move(metadata)) {}

// ---------------------------------------------------------------------------------------------------------------------

Reference::Reference(const nlohmann::json& json)
    : reference_name(json["reference_name"]),
      reference_major_version(json["reference_major_version"]),
      reference_minor_version(json["reference_minor_version"]),
      reference_patch_version(json["reference_patch_version"]),
      reference_metadata_value(json["reference_metadata_value"]) {
    for (const auto& s : json["seqs"]) {
        seqs.emplace_back(s);
    }
    if (json.contains("external_ref")) {
        auto type = static_cast<ExternalRef::ReferenceType>(json["external_ref"]["reference_type"]);
        switch (type) {
            case ExternalRef::ReferenceType::FASTA_REF:
                ref = genie::util::make_unique<external_ref::Fasta>(json["external_ref"]);
                break;
            case ExternalRef::ReferenceType::MPEGG_REF:
                ref = genie::util::make_unique<external_ref::MPEG>(json["external_ref"]);
                break;
            case ExternalRef::ReferenceType::RAW_REF:
                ref = genie::util::make_unique<external_ref::Raw>(json["external_ref"]);
                break;
            default:
                UTILS_DIE("Unknown external reference type.");
        }

    } else {
        ref = genie::util::make_unique<InternalRef>(json["internal_ref"]);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

nlohmann::json Reference::toJson() const {
    nlohmann::json ret;
    ret["reference_name"] = reference_name;
    ret["reference_major_version"] = reference_major_version;
    ret["reference_minor_version"] = reference_minor_version;
    ret["reference_patch_version"] = reference_patch_version;
    ret[ref->getKeyName()] = ref->toJson();
    ret["reference_metadata_value"] = reference_metadata_value;
    for (const auto& s : seqs) {
        ret["seqs"].push_back(s.toJson());
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Reference::getName() const { return reference_name; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Reference::getMajorVersion() const { return reference_major_version; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Reference::getMinorVersion() const { return reference_minor_version; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Reference::getPatchVersion() const { return reference_patch_version; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<Sequence>& Reference::getSequences() const { return seqs; }

// ---------------------------------------------------------------------------------------------------------------------

void Reference::addSequence(Sequence s) { seqs.emplace_back(std::move(s)); }

// ---------------------------------------------------------------------------------------------------------------------

const RefBase& Reference::getBase() const { return *ref; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Reference::getInformation() const { return reference_metadata_value; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace meta
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
