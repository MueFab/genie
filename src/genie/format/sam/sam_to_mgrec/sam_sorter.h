//
// Created by sophie on 04.12.24.
//

#ifndef SAM_SORTER_H
#define SAM_SORTER_H

#include <list>
#include <queue>

#include "sam_reader.h"
#include "sam_record.h"

namespace genie::format::sam::sam_to_mgrec {

struct CmpOpen {
    bool operator()(const std::vector<sam_to_mgrec::SamRecord>& a, const std::vector<sam_to_mgrec::SamRecord>& b) const;
};

struct CmpCompete {
    bool operator()(const std::vector<sam_to_mgrec::SamRecord>& a, const std::vector<sam_to_mgrec::SamRecord>& b) const;
};

class SamSorter {

    //!< @brief queries that can be returned at any moment
    std::vector<std::vector<sam_to_mgrec::SamRecord>> queries;
    //!< @brief automatically sorted by p_next because priority queue
    std::priority_queue<std::vector<sam_to_mgrec::SamRecord>, std::vector<std::vector<sam_to_mgrec::SamRecord>>, CmpOpen> open_queries;
    //!< @brief automatically sorted by position of primary alignment because priority queue
    std::priority_queue<std::vector<sam_to_mgrec::SamRecord>, std::vector<std::vector<sam_to_mgrec::SamRecord>>, CmpCompete> completed_queries;
    //!< @brief mapping position of the next ready query
    uint64_t cur_alignment_position = 0;
    //!< @brief all positions of primary alignments
    std::list<int> primary_alignment_positions;
    //!< @brief maximal allowed distance of two mates
    uint32_t max_distance;

    /** this functions checks what the position of the record that needs to be next is.
     * If there is no record we are waiting on this number is set to maximum uint64 value.
     ***/
    void set_new_alignment_pos();

    static bool isReadComplete(std::vector<sam_to_mgrec::SamRecord> read);

    /** This function takes cur_query and checks, if it's okay to append it to queries.
     * If we are still waiting for a record with smaller position, then cur_query is appended to completed_queries.
     ***/
    void try_appending_to_queries(std::vector<SamRecord> cur_query);

    public:
    explicit SamSorter(uint32_t max_waiting_distance);


    /** Function for sorting one Record.
     ***/
    void addSamRead(const sam_to_mgrec::SamRecord&);

    /** Function for getting already sorted queries.
     * The queries data structure is also cleared afterwards.
     ***/
    std::vector<std::vector<sam_to_mgrec::SamRecord>> getQueries();


    /** Function to signal end of data
     * clears all member variables (open_queries and complete_queries)
     * and puts remaining records to queries **/
    void endFile();

};

}  // namespace genie::format::sam::sam_to_mgrec

#endif //SAM_SORTER_H
