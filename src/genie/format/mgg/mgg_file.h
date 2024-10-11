/**
 * @file
 * @brief Defines the `MggFile` class for handling MPEG-G container files.
 *
 * The `MggFile` class represents the top-level structure for managing MPEG-G container files.
 * It allows reading, modifying, and writing MPEG-G files containing various MPEG-G data boxes.
 * Each `MggFile` instance encapsulates a collection of `Box` objects, providing interfaces for
 * accessing and manipulating them.
 *
 * @copyright
 * This file is part of GENIE.
 * See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGG_MGG_FILE_H_
#define SRC_GENIE_FORMAT_MGG_MGG_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include "genie/format/mgg/box.h"
#include "genie/format/mgg/dataset_group.h"
#include "genie/format/mgg/file_header.h"
#include "genie/util/bit-reader.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

/**
 * @class MggFile
 * @brief Encapsulates an MPEG-G container file structure.
 *
 * The `MggFile` class provides functionalities to parse, manage, and write MPEG-G container files.
 * It holds a collection of MPEG-G boxes, such as `DatasetGroup` and `FileHeader`, and facilitates
 * reading from and writing to bitstreams. It also offers utilities to add and retrieve boxes,
 * as well as debug-printing functionalities.
 */
class MggFile {
 private:
    std::vector<std::unique_ptr<Box>> boxes;  //!< @brief Stores all MPEG-G boxes in the file.
    std::istream* file;                       //!< @brief Pointer to the file input stream.
    std::optional<util::BitReader> reader;    //!< @brief Bit reader for reading binary data from the input stream.

 public:
    /**
     * @brief Constructs an `MggFile` object and initializes it for reading.
     *
     * This constructor takes a file stream pointer and sets up the internal `BitReader` for
     * reading binary data. It is designed to parse the MPEG-G file structure from the input
     * stream, filling the internal box vector as it reads.
     *
     * @param _file Pointer to the input file stream.
     */
    explicit MggFile(std::istream* _file);

    /**
     * @brief Default constructor for an empty `MggFile`.
     *
     * This constructor creates an empty `MggFile` object without any boxes. It is useful
     * for creating MPEG-G containers programmatically.
     */
    MggFile();

    /**
     * @brief Retrieves the list of boxes in the MPEG-G file.
     *
     * Provides access to the internal vector of unique pointers to MPEG-G boxes. This allows
     * external code to iterate over, modify, or query the boxes contained within this `MggFile`.
     *
     * @return Reference to the vector of boxes in the file.
     */
    std::vector<std::unique_ptr<Box>>& getBoxes();

    /**
     * @brief Adds a new box to the MPEG-G file.
     *
     * This method appends a newly constructed box to the internal box vector. The box must
     * be allocated using `std::make_unique` and ownership is transferred to the `MggFile` object.
     *
     * @param box A unique pointer to the box to be added.
     */
    void addBox(std::unique_ptr<Box> box);

    /**
     * @brief Writes the MPEG-G file to a bitstream.
     *
     * Serializes the MPEG-G container, including all its boxes, into the provided bitstream writer.
     * This method is typically used when exporting the `MggFile` to a binary format for storage or transmission.
     *
     * @param writer Reference to the `BitWriter` used for output.
     */
    void write(util::BitWriter& writer);

    /**
     * @brief Prints the structure of the MPEG-G file for debugging.
     *
     * Outputs a hierarchical representation of the MPEG-G boxes contained within the file.
     * This is useful for analyzing the contents of the MPEG-G file and understanding its structure.
     *
     * @param output Reference to the output stream to print to.
     * @param max_depth Maximum depth for recursive printing of nested boxes (default: 100).
     */
    void print_debug(std::ostream& output, uint8_t max_depth = 100) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGG_MGG_FILE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
