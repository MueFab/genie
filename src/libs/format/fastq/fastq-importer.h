/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FASTQ_IMPORTER_H
#define GENIE_FASTQ_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <format/mpegg_rec/alignment-container.h>
#include <format/mpegg_rec/external-alignment.h>
#include <format/mpegg_rec/meta-alignment.h>
#include <format/mpegg_rec/mpegg-record.h>
#include <format/mpegg_rec/segment.h>
#include <util/exceptions.h>
#include <util/make_unique.h>
#include <util/source.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace fastq {

/**
 * Reads fastq files and converts them into MPEGG-Record format
 */
class FastqImporter : public Source<std::unique_ptr<format::mpegg_rec::MpeggChunk>> {
   private:
    static constexpr size_t LINES_PER_RECORD = 4;  //!< How many lines in a fastq files correspondent to one record
    size_t blockSize;                              //!< How many records to read in one pump() run
    std::vector<std::istream *> file_list;         //!< Input streams
    size_t record_counter;                         //!< ID of next data chunk

    enum Lines { ID = 0, SEQUENCE = 1, RESERVED = 2, QUALITY = 3 };  //!< FASTQ format lines
    enum Files { FIRST = 0, SECOND = 1 };                            //!< File shortcuts

    /**
     *
     * @param _file_list
     * @return
     */
    static std::vector<std::array<std::string, LINES_PER_RECORD>> readData(
        const std::vector<std::istream *> &_file_list);

    /**
     *
     * @param data
     */
    static void sanityCheck(const std::array<std::string, LINES_PER_RECORD> &data);

    /**
     *
     * @param data
     * @return
     */
    static std::unique_ptr<format::mpegg_rec::MpeggRecord> buildRecord(
        const std::vector<std::array<std::string, LINES_PER_RECORD>> &data);

   public:
    /**
     *
     * @param _blockSize
     * @param _file_1
     */
    FastqImporter(size_t _blockSize, std::istream *_file_1);

    /**
     *
     * @param _blockSize
     * @param _file_1
     * @param _file_2
     */
    FastqImporter(size_t _blockSize, std::istream *_file_1, std::istream *_file_2);

    /**
     *
     * @return
     */
    bool pump() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fastq
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FASTQ_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
