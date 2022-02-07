/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/meta/reference.h"
#include "genie/format/mpegg_p1/gen_info.h"
#include "genie/format/mpegg_p1/reference_location.h"
#include "genie/util/bitreader.h"

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
        std::string name;                       //!< @brief
        uint32_t sequence_length;               //!< @brief
        uint16_t sequence_id;                   //!< @brief
        genie::core::MPEGMinorVersion version;  //!< @brief

     public:
        /**
         * @brief
         * @param other
         * @return
         */
        bool operator==(const ReferenceSeq& other) const;

        /**
         * @brief
         * @param _name
         * @param length
         * @param id
         * @param _version
         */
        ReferenceSeq(std::string _name, uint32_t length, uint16_t id, genie::core::MPEGMinorVersion _version);

        ReferenceSeq(genie::core::meta::Sequence s, genie::core::MPEGMinorVersion _version)
            : name(std::move(s.getName())), sequence_length(s.getLength()), sequence_id(s.getID()), version(_version) {}

        /**
         * @brief
         * @return
         */
        const std::string& getName() const;

        /**
         * @brief
         * @return
         */
        uint32_t getLength() const;

        /**
         * @brief
         * @return
         */
        uint16_t getID() const;

        /**
         * @brief
         * @param reader
         * @param _version
         */
        ReferenceSeq(genie::util::BitReader& reader, genie::core::MPEGMinorVersion _version);

        /**
         * @brief
         * @param writer
         */
        void write(genie::util::BitWriter& writer) const;
    };

    /**
     * @brief
     */
    class ReferenceVersion {
     private:
        uint16_t major;  //!< @brief
        uint16_t minor;  //!< @brief
        uint16_t patch;  //!< @brief

     public:
        /**
         * @brief
         * @param other
         * @return
         */
        bool operator==(const ReferenceVersion& other) const;

        /**
         * @brief
         * @param _major
         * @param _minor
         * @param _patch
         */
        ReferenceVersion(uint16_t _major, uint16_t _minor, uint16_t _patch);

        /**
         * @brief
         * @param reader
         */
        explicit ReferenceVersion(genie::util::BitReader& reader);

        /**
         * @brief
         * @param writer
         */
        void write(genie::util::BitWriter& writer) const;

        /**
         * @brief
         * @return
         */
        uint16_t getMajor() const;

        /**
         * @brief
         * @return
         */
        uint16_t getMinor() const;

        /**
         * @brief
         * @return
         */
        uint16_t getPatch() const;
    };

 private:
    uint8_t dataset_group_ID;      //!< @brief
    uint8_t reference_ID;          //!< @brief
    std::string reference_name;    //!< @brief
    ReferenceVersion ref_version;  //!< @brief

    std::vector<ReferenceSeq> sequences;  //!< @brief seq_count is length of vector

    std::unique_ptr<ReferenceLocation> reference_location;  //!< @brief

    genie::core::MPEGMinorVersion version;  //!< @brief

 public:
    void patchID(uint8_t groupID) { dataset_group_ID = groupID; }

    void patchRefID(uint8_t _old, uint8_t _new) {
        if (reference_ID == _old) {
            reference_ID = _new;
        }
    }

    genie::core::meta::Reference decapsulate(std::string meta) {
        std::unique_ptr<genie::core::meta::RefBase> location = reference_location->decapsulate();
        genie::core::meta::Reference ret(std::move(reference_name), ref_version.getMajor(), ref_version.getMinor(),
                                         ref_version.getPatch(), std::move(location), std::move(meta));
        return ret;
    }

    Reference(uint8_t _dataset_group_id, uint8_t _reference_ID, genie::core::meta::Reference ref,
              genie::core::MPEGMinorVersion _version)
        : dataset_group_ID(_dataset_group_id),
          reference_ID(_reference_ID),
          reference_name(std::move(ref.getName())),
          ref_version(ref.getMajorVersion(), ref.getMinorVersion(), ref.getPatchVersion()),
          version(_version) {
        for(auto& r : ref.getSequences()) {
            sequences.emplace_back(std::move(r), version);
        }

    }

    /**
     * @brief
     * @param info
     * @return
     */
    bool operator==(const GenInfo& info) const override;

    /**
     * @brief
     * @param reader
     * @param _version
     */
    explicit Reference(util::BitReader& reader, genie::core::MPEGMinorVersion _version);

    /**
     * @brief
     * @param group_id
     * @param ref_id
     * @param ref_name
     * @param _ref_version
     * @param location
     * @param _version
     */
    Reference(uint8_t group_id, uint8_t ref_id, std::string ref_name, ReferenceVersion _ref_version,
              std::unique_ptr<ReferenceLocation> location, genie::core::MPEGMinorVersion _version);

    /**
     * @brief
     * @return
     */
    uint8_t getDatasetGroupID() const;

    /**
     * @brief
     * @return
     */
    uint8_t getReferenceID() const;

    /**
     * @brief
     * @return
     */
    const std::string& getReferenceName() const;

    /**
     * @brief
     * @return
     */
    const ReferenceVersion& getRefVersion() const;

    /**
     * @brief
     * @return
     */
    const std::vector<ReferenceSeq>& getSequences() const;

    /**
     * @brief
     * @return
     */
    const std::string& getKey() const override;

    /**
     * @brief
     * @param seq
     * @param checksum
     */
    void addSequence(ReferenceSeq seq, std::string checksum);

    /**
     * @brief
     * @return
     */
    const ReferenceLocation& getLocation() const;

    /**
     * @brief
     * @param writer
     */
    void box_write(genie::util::BitWriter& writer) const override;

    void print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const override {
        print_offset(output, depth, max_depth, "* Reference");
        print_offset(output, depth + 1, max_depth, "Dataset group ID: " + std::to_string((int)(dataset_group_ID)));
        print_offset(output, depth + 1, max_depth, "Reference ID: " + std::to_string((int)(reference_ID)));
        print_offset(output, depth + 1, max_depth, "Reference name: " + reference_name);
        print_offset(output, depth + 1, max_depth,
                     "Reference major version: " + std::to_string(ref_version.getMajor()));
        print_offset(output, depth + 1, max_depth,
                     "Reference minor version: " + std::to_string(ref_version.getMinor()));
        print_offset(output, depth + 1, max_depth,
                     "Reference patch version: " + std::to_string(ref_version.getPatch()));
        for (const auto& r : sequences) {
            std::string s = "Reference sequence: " + r.getName();
            if (version != core::MPEGMinorVersion::V1900) {
                s += " (ID: " + std::to_string(r.getID()) + "; length: " + std::to_string(r.getLength()) + ")";
            }
            print_offset(output, depth + 1, max_depth, s);
        }
        std::string location;
        if (reference_location->isExternal()) {
            location = "External at " + dynamic_cast<const ExternalReferenceLocation&>(*reference_location).getURI();
        } else {
            const auto& i = dynamic_cast<const InternalReferenceLocation&>(*reference_location);
            location = "Internal at (Dataset Group " + std::to_string(int(i.getDatasetGroupID())) + ", Dataset " +
                       std::to_string(int(i.getDatasetID())) + ")";
        }
        print_offset(output, depth + 1, max_depth, "Reference location: " + location);
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
