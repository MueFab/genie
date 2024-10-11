/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Defines the `Location` class for managing reference locations in MPEG-G files.
 *
 * The `Location` class serves as an abstract base class for representing different types of reference locations
 * within MPEG-G files. It provides a common interface for handling both internal and external references to
 * sequences, enabling flexible and self-contained referencing within MPEG-G containers.
 *
 * Derived classes include:
 * - `Internal`: Represents references to sequences within the same MPEG-G file.
 * - `External::MPEG`: Represents references to sequences stored in other MPEG-G files.
 * - `External::Raw`: Represents references to sequences stored in raw format.
 * - `External::Fasta`: Represents references to sequences stored in FASTA format.
 *
 * This class also supports serialization and deserialization from a bitstream, allowing seamless integration
 * with other components of the MPEG-G library.
 */

#ifndef SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_H_
#define SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>
#include "genie/core/constants.h"
#include "genie/core/meta/external-ref/fasta.h"
#include "genie/core/meta/external-ref/mpeg.h"
#include "genie/core/meta/external-ref/raw.h"
#include "genie/core/meta/internal-ref.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::reference {

/**
 * @brief Abstract base class for managing reference locations in MPEG-G files.
 *
 * The `Location` class represents different types of reference locations used in MPEG-G files. It provides
 * the foundation for handling both internal and external references, allowing MPEG-G containers to
 * refer to sequences either within the same file or in external sources (e.g., other MPEG-G files, raw files, or FASTA files).
 *
 * Derived classes implement specific handling and data structures for each type of reference.
 */
class Location {
 private:
    uint8_t reserved;        //!< @brief Reserved byte for future use.
    bool external_ref_flag;  //!< @brief Indicates whether the reference is external.

 public:
    /**
     * @brief Virtual destructor to ensure proper cleanup of derived classes.
     */
    virtual ~Location() = default;

    /**
     * @brief Constructs a `Location` object with specified parameters.
     *
     * @param _reserved Reserved byte for additional options or metadata.
     * @param _external_ref_flag Flag indicating if the reference is external.
     */
    explicit Location(uint8_t _reserved, bool _external_ref_flag);

    /**
     * @brief Constructs a `Location` object by reading from a bitstream.
     *
     * This constructor initializes a `Location` object using the data read from the provided bitstream.
     *
     * @param reader The `BitReader` used to read the reference data from the bitstream.
     */
    explicit Location(genie::util::BitReader& reader);

    /**
     * @brief Checks if the reference is external.
     * @return True if the reference is external, false otherwise.
     */
    [[nodiscard]] bool isExternal() const;

    /**
     * @brief Factory method to create a `Location` object based on the bitstream data.
     *
     * This method reads data from the provided bitstream and creates an appropriate `Location` object
     * based on the reference type and version specified.
     *
     * @param reader The `BitReader` to read the reference location data from.
     * @param seq_count Number of sequences in the reference.
     * @param _version MPEG-G version for compatibility.
     * @return A unique pointer to the created `Location` object.
     */
    static std::unique_ptr<Location> factory(genie::util::BitReader& reader, size_t seq_count,
                                             genie::core::MPEGMinorVersion _version);

    /**
     * @brief Factory method to create a `Location` object from a `RefBase` object.
     *
     * This method takes a `RefBase` object and constructs a corresponding `Location` object based on the
     * reference type contained within the `RefBase`.
     *
     * @param base The `RefBase` object to convert.
     * @param _version MPEG-G version for compatibility.
     * @return A unique pointer to the created `Location` object.
     */
    static std::unique_ptr<Location> referenceLocationFactory(std::unique_ptr<genie::core::meta::RefBase> base,
                                                              genie::core::MPEGMinorVersion _version);

    /**
     * @brief Serializes the `Location` object into a bitstream.
     *
     * This method writes the `Location`'s attributes and type information into a bitstream for storage or
     * transmission.
     *
     * @param writer The `BitWriter` to write the serialized data to.
     */
    virtual void write(genie::util::BitWriter& writer);

    /**
     * @brief Converts the `Location` into a `RefBase` object.
     *
     * This method decapsulates the reference location, converting it into a `RefBase` object that can be
     * used by higher-level MPEG-G metadata structures.
     *
     * @return A unique pointer to the decapsulated `RefBase` object.
     */
    virtual std::unique_ptr<genie::core::meta::RefBase> decapsulate() = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_REFERENCE_LOCATION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
