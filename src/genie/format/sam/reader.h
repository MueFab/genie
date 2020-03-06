/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_READER_H
#define GENIE_READER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <istream>
#include <map>
#include "header/header.h"
#include "record.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

class Reader {
   public:

    enum class Index : uint8_t {
        SINGLE_MAPPED = 0,
        SINGLE_UNMAPPED = 1,
        PAIR_FIRST_PRIMARY = 2,
        PAIR_LAST_PRIMARY = 3,
        PAIR_FIRST_NONPRIMARY = 4,
        PAIR_LAST_NONPRIMARY = 5,
        UNKNOWN = 6,
        TOTAL_TYPES = 7,
    };

    enum class Constraint : uint8_t {
        NONE = 0,
        BY_NUM_RECORDS = 1,
        BY_SIZE = 2
    };

   private:
    std::istream& stream;
    header::Header header;
//    Record save;
    bool rec_saved;

    Constraint constraint;
    uint64_t constraint_val;
    uint64_t num_records;

   public:
    // TODO: Set as private, temporary public for debugging purpose
    std::map<std::string, std::vector<std::vector<Record>>> data;

    explicit Reader(std::istream& _stream, Constraint _constraint = Constraint::NONE, uint64_t _constraint_val = UINT64_MAX);

    const header::Header& getHeader() const;

    bool isConstrainReached() const; //

    void read();

    bool isEnd();

    // TODO: Discusssion - Implement isConstrainReached as pointer to avoid if-case / switch evaluation
//    bool (&isConstrainReached)();
//    bool noConstrain() const;
//    bool isLimitNumRecordReached() const;
//    bool isLimitSizeReached() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_READER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------