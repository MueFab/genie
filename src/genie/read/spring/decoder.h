/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_DECODER_H_
#define SRC_GENIE_READ_SPRING_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include <vector>
#include "filesystem/filesystem.hpp"
#include "genie/core/read-decoder.h"
#include "genie/read/spring/params.h"
#include "genie/read/spring/util.h"
#include "genie/util/ordered-section.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

/**
 * @brief
 */
struct Record {
    std::string name;  //!< @brief
    std::string seq;   //!< @brief
    std::string qv;    //!< @brief
};

/**
 * @brief
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
                    std::array<std::vector<Record>, 2>& unmatched_records,
                    std::vector<std::string>& names, std::vector<std::string>& qvs);

/**
 * @brief
 */
class Decoder : public genie::core::ReadDecoder {
   private:
    compression_params cp{};        //!<
    bool combine_pairs;             //!<
    size_t num_thr;                 //!<
    util::OrderedLock lock;         //!<
    std::ofstream fout_unmatched1;  //!<
    std::ofstream fout_unmatched2;  //!<
    std::ofstream fout_unmatched_readnames_1;  //!<
    std::ofstream fout_unmatched_readnames_2;  //!<

    std::string file_unmatched_fastq1;  //!<
    std::string file_unmatched_fastq2;  //!<
    std::string file_unmatched_readnames_1; //!<
    std::string file_unmatched_readnames_2; //!<
    uint32_t unmatched_record_index[2];   //!<
    std::string basedir;                //!<

   public:
    /**
     * @brief
     * @param working_dir
     * @param comb_p
     * @param paired_end
     */
    explicit Decoder(const std::string& working_dir, bool comb_p, bool paired_end);

    /**
     * @brief
     * @param t
     * @param id
     */
    void flowIn(genie::core::AccessUnit&& t, const util::Section& id) override;

    /**
     * @brief
     * @param i
     * @param r
     */
    static void readRec(std::ifstream& i, Record& r);

    /**
     * @brief
     * @param chunk
     * @param r
     * @param pos
     */
    void add(core::record::Chunk& chunk, core::record::Record&& r, uint64_t& pos);

    /**
     * @brief
     * @param pos
     */
    void flushIn(uint64_t& pos) override;

    /**
     * @brief
     * @param id
     */
    void skipIn(const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_DECODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
