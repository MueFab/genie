/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/format/mpegg_p1/file_header.h"
#include "reference_location.h"
#include "genie/format/mpegg_p1/util.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 */
class Reference : public GenInfo {
 public:
    enum class ReferenceType : uint8_t { MPEGG_REF = 0, RAW_REF = 1, FASTA_REF = 2 };  //!< @brief

    class ReferenceSeq {
     private:
        std::string name;
        uint32_t sequence_length;
        uint16_t sequence_id;
        genie::core::MPEGMinorVersion version;

     public:
        ReferenceSeq(std::string _name, uint32_t length, uint16_t id, genie::core::MPEGMinorVersion _version)
            : name(std::move(_name)), sequence_length(length), sequence_id(id), version(_version) {}

        const std::string& getName() const { return name; }

        uint32_t getLength() const { return sequence_length; }

        uint16_t getID() const { return sequence_id; }

        ReferenceSeq(genie::util::BitReader& reader, genie::core::MPEGMinorVersion _version) : version(_version) {
            reader.readBypass_null_terminated(name);
            if (version != genie::core::MPEGMinorVersion::V1900) {
                sequence_length = reader.readBypassBE<uint32_t>();
                sequence_id = reader.readBypassBE<uint16_t>();
            }
        }

        void write(genie::util::BitWriter& writer) const {
            writer.writeBypass(name.data(), name.length());
            writer.writeBypassBE<uint8_t>('\0');
            if (version != genie::core::MPEGMinorVersion::V1900) {
                writer.writeBypassBE(sequence_length);
                writer.writeBypassBE(sequence_id);
            }
        }
    };

    class ReferenceVersion {
     private:
        uint16_t major;  //!< @brief
        uint16_t minor;  //!< @brief
        uint16_t patch;  //!< @brief
     public:
        ReferenceVersion(uint16_t _major, uint16_t _minor, uint16_t _patch)
            : major(_major), minor(_minor), patch(_patch) {}

        explicit ReferenceVersion(genie::util::BitReader& reader) {
            major = reader.readBypassBE<uint16_t>();
            minor = reader.readBypassBE<uint16_t>();
            patch = reader.readBypassBE<uint16_t>();
        }

        void write(genie::util::BitWriter& writer) const {
            writer.writeBypassBE(major);
            writer.writeBypassBE(minor);
            writer.writeBypassBE(patch);
        }

        uint16_t getMajor() const { return major; }

        uint16_t getMinor() const { return minor; }

        uint16_t getPatch() const { return patch; }
    };

 private:
    /** ------------------------------------------------------------------------------------------------------------
     * Reference - ISO 23092-1 Section 6.5.1.3 table 10
     * Reference Metadata - ISO 23092-1 Section 6.5.1.4 table 13 - metadata specified in ISO/IEC 23092-3
     * ------------------------------------------------------------------------------------------------------------- */
    uint8_t dataset_group_ID;    //!< @brief
    uint8_t reference_ID;        //!< @brief
    std::string reference_name;  //!< @brief
    ReferenceVersion ref_version;

    std::vector<ReferenceSeq> sequences;  //!< @brief seq_count is length of vector

    // Contains external_ref_flag and corresponding data
    std::unique_ptr<ReferenceLocation> reference_location;  //!< @brief

    genie::core::MPEGMinorVersion version;

 public:
    explicit Reference(util::BitReader& reader,
                       genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000)
        : ref_version(0, 0, 0) {
        version = _version;
        dataset_group_ID = reader.readBypassBE<uint8_t>();
        reference_ID = reader.readBypassBE<uint8_t>();
        reader.readBypass_null_terminated(reference_name);
        ref_version = ReferenceVersion(reader);
        auto seq_count = reader.readBypassBE<uint16_t>();
        for (size_t i = 0; i < seq_count; ++i) {
            sequences.emplace_back(reader, version);
        }

        reference_location = ReferenceLocation::referenceLocationFactory(reader, seq_count, _version);
    }

    Reference(uint8_t group_id, uint8_t ref_id, std::string ref_name, ReferenceVersion _ref_version,
              std::unique_ptr<ReferenceLocation> location, genie::core::MPEGMinorVersion _version)
        : dataset_group_ID(group_id),
          reference_ID(ref_id),
          reference_name(std::move(ref_name)),
          ref_version(_ref_version),
          reference_location(std::move(location)),
          version(_version) {}

    uint8_t getDatasetGroupID() const { return dataset_group_ID; }

    uint8_t getReferenceID() const { return reference_ID; }

    const std::string& getReferenceName() const { return reference_name; }

    const ReferenceVersion& getRefVersion() const { return ref_version; }

    const std::vector<ReferenceSeq>& getSequences() const { return sequences; }

    const std::string& getKey() const override {
        static const std::string key = "rfgn";
        return key;
    }

    void addSequence(ReferenceSeq seq, std::string checksum) {
        sequences.emplace_back(std::move(seq));
        if (reference_location->isExternal()) {
            dynamic_cast<ExternalReferenceLocation&>(*reference_location).addChecksum(std::move(checksum));
        }
    }

    const ReferenceLocation& getLocation() const { return *reference_location; }

    uint64_t getSize() const override {
        std::stringstream stream;
        genie::util::BitWriter writer(&stream);
        write(writer);
        return stream.str().length();
    }

    void write(genie::util::BitWriter& writer) const override {
        GenInfo::write(writer);
        writer.writeBypassBE(dataset_group_ID);
        writer.writeBypassBE(reference_ID);
        writer.writeBypass(reference_name.data(), reference_name.length());
        ref_version.write(writer);
        writer.writeBypassBE<uint16_t>(sequences.size());
        for (const auto& s : sequences) {
            s.write(writer);
        }
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
