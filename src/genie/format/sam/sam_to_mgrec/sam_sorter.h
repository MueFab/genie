//
// Created by sophie on 04.12.24.
//

#ifndef SAM_SORTER_H
#define SAM_SORTER_H

#define BUFFER_SIZE 1000000

#include <queue>
#include <string>

#include "sam_reader.h"
#include "sam_record.h"

namespace genie::format::sam::sam_to_mgrec {

class SamSorter {

    /* One record has type SamRecord,
     * One query has a collection of multiple SamRecords
     * The queries should be saved in a buffer of size PHASE2_BUFFER_SIZE
     * When size is reached, return the buffer
     */

    std::string sam_file_path;
    genie::format::sam::sam_to_mgrec::SamReader sam_reader;
    // should be sorted by p_next
    std::priority_queue<std::vector<sam_to_mgrec::SamRecord>> open_reads;
    // should be sorted by position of primary alignment
    std::vector<std::vector<sam_to_mgrec::SamRecord>> completed_reads;
    //!< @brief mapping position of the next ready query
    int cur_alignment_position;



    public:
    SamSorter(std::string& file_path);

    /**
     *
     * @return vector of queries with size BUFFER_SIZE
     */
    void addSamRead(std::vector<std::vector<sam_to_mgrec::SamRecord>>& queries, uint32_t num_threads);

    bool isReadComplete(std::vector<sam_to_mgrec::SamRecord>& read);




};

}  // namespace genie::format::sam::sam_to_mgrec

#endif //SAM_SORTER_H
