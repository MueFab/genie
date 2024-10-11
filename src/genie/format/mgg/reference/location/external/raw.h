/**
 * @file
 * @brief Defines the `Raw` class for managing external raw reference locations in MPEG-G files.
 *
 * The `Raw` class is a specialized type of `External` reference location that represents raw sequences
 * in external files. It handles the serialization and deserialization of raw reference data and manages
 * the sequence-level checksums for validating the integrity of these references. This class is used in the
 * context of MPEG-G reference metadata to refer to raw sequences stored externally.
 *
 * @copyright
 * This file is part of GENIE. See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_RAW_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_RAW_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/format/mgg/reference/location/external.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference::location::external {

/**
 * @brief Represents an external raw reference location in an MPEG-G file.
 *
 * The `Raw` class extends the `External` location type to support references to raw sequence data
 * stored outside of the MPEG-G file. It manages URI references and sequence-level checksums for
 * raw sequences, ensuring data integrity and compatibility. This class is used to represent raw
 * sequence references that are not in standard formats like FASTA or MPEG-G.
 */
class Raw : public External {
 private:
    std::vector<std::string> seq_checksums;  //!< @brief Checksums for individual sequences in the external raw dataset.

 public:
    /**
     * @brief Decapsulates the `Raw` reference into a core metadata object.
     *
     * This method extracts the internal state of the `Raw` object and converts it into a more generic
     * core metadata representation, which can be used in various contexts within the MPEG-G standard.
     *
     * @return A unique pointer to the decapsulated `RefBase` object.
     */
    std::unique_ptr<genie::core::meta::RefBase> decapsulate() override;

    /**
     * @brief Constructs a `Raw` reference with the specified parameters.
     *
     * @param _reserved Reserved byte value.
     * @param _uri The URI pointing to the external raw reference.
     * @param algo The checksum algorithm used for validating the sequence data.
     */
    Raw(uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo);

    /**
     * @brief Constructs a `Raw` reference by reading from a bitstream.
     *
     * This constructor initializes the `Raw` object by reading its state from a `BitReader`. The sequence count is
     * used to determine the number of checksums to read.
     *
     * @param reader The `BitReader` to read the data from.
     * @param seq_count The number of sequences expected in the raw reference.
     */
    Raw(genie::util::BitReader& reader, size_t seq_count);

    /**
     * @brief Constructs a `Raw` reference with additional parameters.
     *
     * @param reader The `BitReader` to read the data from.
     * @param _reserved Reserved byte value.
     * @param _uri The URI pointing to the external raw reference.
     * @param algo The checksum algorithm used for validating the sequence data.
     * @param seq_count The number of sequences expected in the raw reference.
     */
    Raw(genie::util::BitReader& reader, uint8_t _reserved, std::string _uri, ChecksumAlgorithm algo, size_t seq_count);

    /**
     * @brief Gets the checksums for the sequences in the raw reference.
     * @return A reference to the vector of sequence checksums.
     */
    [[nodiscard]] const std::vector<std::string>& getSeqChecksums() const;

    /**
     * @brief Adds a new checksum for a sequence in the raw reference.
     * @param checksum The checksum value to add.
     */
    void addSeqChecksum(std::string checksum);

    /**
     * @brief Serializes the `Raw` object to a bitstream.
     * @param writer The `BitWriter` to serialize the data to.
     */
    void write(genie::util::BitWriter& writer) override;

    /**
     * @brief Adds a checksum to the `Raw` reference.
     *
     * This method is part of the `External` interface and is used to add a checksum to the `Raw` object.
     *
     * @param checksum The checksum value to add.
     */
    void addChecksum(std::string checksum) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference::location::external

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_EXTERNAL_RAW_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
