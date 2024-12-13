//
// Created by sophie on 04.12.24.
//

#ifndef SAM_SORTER_H
#define SAM_SORTER_H

#define BUFFER_SIZE 1000000
#define MAX_BUFFER_SIZE 1000

#include <list>
#include <mutex>
#include <queue>
#include <string>

#include "sam_reader.h"
#include "sam_record.h"

namespace genie::format::sam::sam_to_mgrec {

struct CmpOpen {
    bool operator()(std::vector<sam_to_mgrec::SamRecord>& a, std::vector<sam_to_mgrec::SamRecord>& b) const;
};

struct CmpCompete {
    bool operator()(std::vector<sam_to_mgrec::SamRecord>& a, std::vector<sam_to_mgrec::SamRecord>& b) const;
};

class SamSorter {

    /* One record has type SamRecord,
     * One query has a collection of multiple SamRecords
     * The queries should be saved in a buffer of size PHASE2_BUFFER_SIZE
     * When size is reached, return the buffer
     */

    std::string sam_file_path;
    genie::format::sam::sam_to_mgrec::SamReader sam_reader;
    //!< @brief automatically sorted by p_next because priority queue
    std::priority_queue<std::vector<sam_to_mgrec::SamRecord>, std::vector<std::vector<sam_to_mgrec::SamRecord>>, CmpOpen> open_queries;
    //!< @brief automatically sorted by position of primary alignment because priority queue
    std::priority_queue<std::vector<sam_to_mgrec::SamRecord>, std::vector<std::vector<sam_to_mgrec::SamRecord>>, CmpCompete> completed_queries;
    //!< @brief mapping position of the next ready query
    uint64_t cur_alignment_position = 0;
    //!< @brief all positions of primary alignments
    std::list<int> primary_alignment_positions;


    public:
    SamSorter(std::string& file_path);

    void set_new_alignment_pos();

    static bool isReadComplete(std::vector<sam_to_mgrec::SamRecord> read);

    void try_appending_to_queries(std::vector<SamRecord> cur_query, std::vector<std::vector<sam_to_mgrec::SamRecord>>& queries);

    void addSamRead(std::vector<std::vector<sam_to_mgrec::SamRecord>>& queries, uint32_t num_threads);

    void addSamRead_thread(std::mutex& lock, std::vector<std::vector<sam_to_mgrec::SamRecord>>& queries);

};

}  // namespace genie::format::sam::sam_to_mgrec

#endif //SAM_SORTER_H
