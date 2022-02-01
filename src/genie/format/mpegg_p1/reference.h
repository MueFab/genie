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
    explicit Reference(util::BitReader& reader,
                       genie::core::MPEGMinorVersion _version = genie::core::MPEGMinorVersion::V2000);

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
     * @return
     */
    uint64_t getSize() const override;

    /**
     * @brief
     * @param writer
     */
    void write(genie::util::BitWriter& writer) const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_REFERENCE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
