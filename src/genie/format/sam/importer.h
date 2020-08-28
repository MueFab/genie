/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SAM_IMPORTER_H
#define GENIE_SAM_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/format-importer.h>
#include <genie/core/record/record.h>
#include <genie/util/ordered-lock.h>
#include <map>
#include "reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

typedef std::list<Record> SamRecords;
typedef std::list<std::list<Record>> SamRecords2D;

class Importer : public core::FormatImporter {
   private:
    size_t blockSize;
    Reader samReader;
    ReadTemplateGroup rtg;
    util::OrderedLock lock;  //!< @brief Lock to ensure in order execution
    uint64_t last_position{0};

    /**
     *
     * @brief Convert 2 SAM records to SameRec MPEG-G record
     *
     * @param sam_r1: First SAM record
     * @param sam_r2: Second SAM record
     * @param refs: Map, contains Reference ID associated to each Reference Name
     * @return SameRec MPEG-G record
     */
    static std::unique_ptr<core::record::Record> convertSam2SameRec(Record& sam_r1, Record& sam_r2,
                                                                    std::map<std::string, size_t>& refs);

    /**
     *
     * @brief Add alignment to the existing SameRec MPEG-G record
     *
     * @param rec: SameRec MPEG-G record
     * @param sam_r1: First SAM record
     * @param sam_r2: Second SAM record
     * @param refs: Map, contains Reference ID associated to each Reference Name
     */
    static void addAlignmentToSameRec(std::unique_ptr<core::record::Record>& rec, Record& sam_r1, Record& sam_r2,
                                      std::map<std::string, size_t>& refs);

    /**
     *
     * @brief Convert SAM Records that belongs to the same template to SameRec MPEG-G records
     *
     * @param template_chunk: : List of MPEG-G records
     * @param sam_recs_2d: List of List of sam records (alignments).
     * @param refs: Map, contains Reference ID associated to each Reference Name
     */
    static void convertPairedEndNoSplit(core::record::Chunk& template_chunk, SamRecords2D& sam_recs_2d, std::map<std::string, size_t>& refs);

    /**
     *
     * @brief Convert a SAM Record to SplitRec MPEG-G record
     *
     * @param sam_r1: SAM record
     * @param sam_r2_ptr: pointer to the pair of sam_r1, NULL_POINTER if no pair found
     * @param refs: Map, contains Reference ID associated to each Reference Name
     * @return SplitRec MPEG-G record
     */
    static std::unique_ptr<core::record::Record> convertSam2SplitRec(Record& sam_r1, Record* sam_r2_ptr,
                                              std::map<std::string, size_t>& refs);

    /**
     *
     * @brief Add alignment to the existing SplitRec MPEG-G record
     *
     * @param rec: SplitRec MPEG-G record
     * @param sam_r1: SAM record that has the same RefID as the one stored in rec
     * @param sam_r2_ptr: pointer to the pair of sam_r1, NULL_POINTER if no pair found
     * @param refs: Map, contains Reference ID associated to each Reference Name
     */
    static void addAlignmentToSplitRec(std::unique_ptr<core::record::Record>& rec, Record& sam_r1, Record* sam_r2_ptr,
                                       std::map<std::string, size_t>& refs);


    /**
     *
     * @brief Convert SAM Records that belongs to the same template to SplitRec MPEG-G records
     *
     * @param template_chunk:
     * @param sam_recs_2d
     * @param refs
     */
    static void convertPairedEndSplitPair(core::record::Chunk& template_chunk, SamRecords2D& sam_recs_2d, std::map<std::string, size_t>& refs);

   public:
    /**
     *
     * @param _blockSize
     * @param _file
     */
    Importer(size_t _blockSize, std::istream &_file);

    /**
     *
     * @brief Convert SAM flags to MPEG-G record flags
     *
     * @param flags: SAM flags
     * @return
     */
    static std::tuple<bool, uint8_t> convertFlags2Mpeg(uint16_t flags);

    /**
     *
     * @param token
     * @return
     */
    static char convertCigar2ECigarChar(char token);

    /**
     *
     * @param token
     * @return
     */
    static int stepSequence(char token);

    /**
     *
     * @brief Convert CIGAR string to ECIGAR string
     *
     * @param cigar
     * @param seq
     * @return
     */
    static std::string convertCigar2ECigar(const std::string &cigar, const std::string &seq);

    /**
     *
     * @param id
     * @return
     */
   // bool pump(uint64_t& id, std::mutex& lock) override;

    bool pumpRetrieve(core::Classifier* _classifier) override;

    /**
     *
     */
    void flushIn(uint64_t& pos) override;

    /**
     *
     * @brief Convert Unmapped Reads SAM records to MPEG-G records
     *
     * @param chunk: List of MPEG-G records
     * @param sam_recs: SAM record
     * @param refs: Map containing Reference ID associated to each Reference Name
     */
    static void convertUnmapped(core::record::Chunk& chunk, SamRecords& sam_recs, std::map<std::string, size_t>& refs);

    /**
     *
     * @brief Convert Single-end Reads SAM records to MPEG-G records
     *
     * @param chunk: List of MPEG-G records
     * @param sam_recs: SAM record
     * @param refs: Map, contains Reference ID associated to each Reference Name
     * @param unmapped_pair:  SAM record has
     * @param is_read_1_first
     */
    static void convertSingleEnd(core::record::Chunk& chunk, SamRecords& sam_recs, std::map<std::string, size_t>& refs,
                          bool unmapped_pair=false, bool is_read_1_first=true);

    /**
     * @brief Convert Paired-end Reads SAM records to MPEG-G records
     *
     * @param chunk: List of MPEG-G records
     * @param sam_recs_2d: List of List of sam records (alignments).
     * @param refs: Map, contains Reference ID associated to each Reference Name
     * @param force_split: Force creation of MPEG-G record for each read.
     */
    static void convertPairedEnd(core::record::Chunk& chunk, SamRecords2D& sam_recs_2d, std::map<std::string,
                                 size_t>& refs, bool force_split=false);

    /**
     * @brief Convert SAM records contained in ReadTemplate data structure to MPEG-G records
     *
     * @param chunk: List of MPEG-G records
     * @param rt: ReadTemplate object, contains SAM reads belongs to one template
     * @param refs: Map, contains Reference ID associated to each Reference Name
     * @param force_split: Force creation of MPEG-G record for each read.
     */
    static void convert(core::record::Chunk& chunk, ReadTemplate& rt, std::map<std::string, size_t>& refs,
                        bool force_split=false);

    /**
     * @brief Compare mapping position of primary alignment of r1 and r2
     *
     * @param r1
     * @param r2
     * @return true, if mapping pos of r1 is less than of r2
     */
    static bool compare(const core::record::Record& r1, const core::record::Record& r2);

    static uint64_t getMinPos(const core::record::Record& r);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------