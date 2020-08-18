/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SPRING_DECOMPRESS_H_
#define SPRING_DECOMPRESS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/read-decoder.h>
#include <genie/util/ordered-section.h>
#include <genie/util/watch.h>
#include <filesystem/filesystem.hpp>
#include <map>
#include <string>
#include <vector>
#include "params.h"
#include "util.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

/**
 *
 */
struct Record {
    std::string name;  //!<
    std::string seq;   //!<
    std::string qv;    //!<
};

/**
 *
 * @param au
 * @param paired_end
 * @param combine_pairs
 * @param matched_records
 * @param unmatched_records
 * @param mate_au_id
 * @param mate_record_index
 * @param names
 * @param qvs
 */
void decode_streams(core::AccessUnit& au, bool paired_end, bool combine_pairs,
                    std::array<std::vector<Record>, 2>& matched_records,
                    std::array<std::vector<Record>, 2>& unmatched_records, std::vector<uint32_t>& mate_au_id,
                    std::vector<uint32_t>& mate_record_index, std::vector<std::string>& names,
                    std::vector<std::string>& qvs);

/**
 *
 */
class Decoder : public genie::core::ReadDecoder {
   private:
    compression_params cp{};        //!<
    bool combine_pairs;             //!<
    util::OrderedLock lock;         //!<
    std::ofstream fout_unmatched1;  //!<
    std::ofstream fout_unmatched2;  //!<

    std::string file_unmatched_fastq1;  //!<
    std::string file_unmatched_fastq2;  //!<
    std::string basedir;                //!<

    std::vector<uint32_t> mate_au_id_concat, mate_record_index_concat;  //!<

   public:
    /**
     *
     * @param working_dir
     * @param comb_p
     * @param paired_end
     * @param threads
     */
    explicit Decoder(const std::string& working_dir, bool comb_p, bool paired_end);

    /**
     *
     * @param t
     * @param id
     */
    void flowIn(genie::core::AccessUnit&& t, const util::Section& id) override;

    /**
     *
     * @param i
     * @param r
     */
    static void readRec(std::ifstream& i, Record& r);

    /**
     *
     * @param chunk
     * @param r
     * @param pos
     */
    void add(core::record::Chunk& chunk, core::record::Record&& r, size_t& pos);

    /**
     *
     * @param pos
     */
    void flushIn(size_t& pos) override;

    /**
     *
     * @param id
     */
    void skipIn(const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SPRING_DECOMPRESS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------