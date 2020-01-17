/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_IMPORTER_H
#define GENIE_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/record/record.h>
#include <genie/util/exceptions.h>
#include <genie/util/make-unique.h>
#include <genie/util/ordered-lock.h>
#include <genie/util/original-source.h>
#include <genie/util/source.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace fastq {

/**
 * @brief Module to reads fastq files and convert them into MPEGG-Record format
 */
class FastqImporter : public util::Source<std::unique_ptr<core::record::MpeggChunk>>, public util::OriginalSource {
   private:
    static constexpr size_t LINES_PER_RECORD = 4;  //!< @brief How many lines in a fastq file belong to one record
    size_t blockSize;                              //!< @brief How many records to read in one pump() run
    std::vector<std::istream *> file_list;         //!< @brief Input streams (paired files supported)
    size_t record_counter;                         //!< @brief ID of next data chunk
    util::OrderedLock lock;                        //!< @brief Lock to ensure in order execution

    enum Lines { ID = 0, SEQUENCE = 1, RESERVED = 2, QUALITY = 3 };  //!< @brief FASTQ format lines
    enum Files { FIRST = 0, SECOND = 1 };                            //!< @brief File shortcuts

    /**
     * @brief Read one chunk of fastq data
     * @param _file_list Where to read data from (2 streams in paired mode)
     * @return Data extracted from the fastq files, not converted yet
     */
    static std::vector<std::array<std::string, LINES_PER_RECORD>> readData(
        const std::vector<std::istream *> &_file_list);

    /**
     * @brief Check if read record data is actually valid for fastq files or if anything went wrong
     * @param data Data read previously
     */
    static void sanityCheck(const std::array<std::string, LINES_PER_RECORD> &data);

    /**
     * @brief Do the conversion to MPEG-G records
     * @param data Raw fastq data
     * @return Finished MPEG-G record
     */
    static core::record::Record buildRecord(std::vector<std::array<std::string, LINES_PER_RECORD>> data);

   public:
    /**
     * @brief Unpaired input
     * @param _blockSize How many records to extract per pump()
     * @param _file_1 Input file
     */
    FastqImporter(size_t _blockSize, std::istream *_file_1);

    /**
     * @brief Paired input
     * @param _blockSize How many records to extract per pump()
     * @param _file_1 Input file #1
     * @param _file_2 Input file #2
     */
    FastqImporter(size_t _blockSize, std::istream *_file_1, std::istream *_file_2);

    /**
     * @brief Process one block of data and propagate it to the next module in the chain
     * @param id Current block identifier
     * @return True if more data is available, false otherwise
     */
    bool pump(size_t id) override;

    /**
     * @brief Cleanup, end of data
     */
    void dryIn() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fastq
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
