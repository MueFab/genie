/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "reader.h"
#include <map>
#include <genie/util/exceptions.h>
#include <hdf5_hl.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

Reader::Reader(std::istream& _stream, Constraint _constraint, uint64_t _constraint_val):
    stream(_stream), header(stream), rec_saved(false), constraint(_constraint), constraint_val(_constraint_val), num_records(0){}

// ---------------------------------------------------------------------------------------------------------------------

const header::Header& Reader::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::isConstrainReached() const {
    switch (constraint){
        case Constraint::BY_NUM_RECORDS :
            return num_records > constraint_val;
        // TODO: Constraint BY_SIZE
        case Constraint::BY_SIZE :
            UTILS_DIE(" Not yet implemented : Constraint BY_SIZE");
        case Constraint::NONE:
            return false;
        default:
            UTILS_DIE("Invalid value of constraint / constraint mode not yet implemented");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Reader::read() {
    std::string string;
    while (!isConstrainReached() && std::getline(stream, string)){
        Record record(string);
        // TODO: Store values for other constraints such as BY_SIZE
        num_records++;

        UTILS_DIE_IF(!header.isReferenceExists(record.getRname()), "No reference in header");

        const auto& qname = record.getQname();

        auto entry = data.find(qname);
        if (entry == data.end()) {
            data.emplace(qname, std::vector<std::vector<Record>>());

            for (uint8_t i = 0; i < uint8_t(Index::TOTAL_TYPES); i++) {
                data[qname].emplace_back(std::vector<Record>());
            }
        }

        Reader::Index idx;
        // Paired-end Read
        if (record.checkFlag(Record::FlagPos::MULTI_SEGMENT_TEMPLATE)){
            if (record.checkFlag(Record::FlagPos::FIRST_SEGMENT)){
                if (record.isPrimaryLine()){
                    idx = Index::PAIR_FIRST_PRIMARY;
                } else {
                    idx = Index::PAIR_FIRST_NONPRIMARY;
                }
            } else if (record.checkFlag(Record::FlagPos::LAST_SEGMENT)){
                if (record.isPrimaryLine()){
                    idx = Index::PAIR_LAST_PRIMARY;
                } else {
                    idx = Index::PAIR_LAST_NONPRIMARY;
                }
            } else {
                idx = Index::UNKNOWN;
            }
        } else {
            if (record.checkFlag(Record::FlagPos::SEGMENT_UNMAPPED)) {
                idx = Index::SINGLE_UNMAPPED;
            } else {
                idx = Index::SINGLE_MAPPED;
            }
        }

        data[qname][uint8_t(idx)].push_back(std::move(record));
    }

    for (auto iter = data.begin(); iter != data.end(); iter++){

        // Use If-Case with UTILS_DIE for debugging purpose
        // TODO @Yeremia : Change to UTILS_DIE_IF
        if (iter->second[uint8_t(Index::UNKNOWN)].size() > 0){
            UTILS_DIE("Uncategorized SAM record found");
        }

        auto isSingleEnd = (iter->second[uint8_t(Index::SINGLE_MAPPED)].size() + iter->second[uint8_t(Index::SINGLE_UNMAPPED)].size()) > 0;
        auto isPairedEnd1 = iter->second[uint8_t(Index::PAIR_FIRST_PRIMARY)].size() + iter->second[uint8_t(Index::PAIR_FIRST_NONPRIMARY)].size() > 0;
        auto isPairedEnd2 = iter->second[uint8_t(Index::PAIR_LAST_PRIMARY)].size() + iter->second[uint8_t(Index::PAIR_LAST_NONPRIMARY)].size() > 0;

        // Check if template is either single or paired-end using XNOR Operation
        // Use If-Case with UTILS_DIE for debugging purpose
        // TODO @Yeremia : Change to UTILS_DIE_IF
        if (!((isSingleEnd || (isPairedEnd1 || isPairedEnd2)) && !(isSingleEnd && (isPairedEnd1 || isPairedEnd2)))){
            UTILS_DIE("Template with both single- and paired-end reads found");
        }


    }

//    std::string string;
//    vec.clear();
//    if (rec_saved) {
//        rec_saved = false;
//        vec.emplace_back(std::move(save));
//    }
//    while (num && std::getline(stream, string)) {
//        vec.emplace_back(string);
//        if (vec.front().getRname() != vec.back().getRname()) {
//            save = std::move(vec.back());
//            vec.pop_back();
//            rec_saved = true;
//            return;
//        }
//        num--;
//    }
    

//    vec.clear();


}

// ---------------------------------------------------------------------------------------------------------------------

bool Reader::isEnd() { return stream.eof(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------