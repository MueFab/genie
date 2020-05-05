/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_READER_H
#define GENIE_READER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <istream>
#include "header/header.h"
#include "record.h"

#include <map>
#include <list>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

class Reader {
   private:
    std::istream& stream;
    header::Header header;
    Record save;
    bool rec_saved;

   public:
    explicit Reader(std::istream& _stream);

    const header::Header& getHeader() const;

    bool read(Record& rec);
    void readRecords(size_t num, std::vector<Record>& vec);

    bool isEnd();
};

// ---------------------------------------------------------------------------------------------------------------------

class ReadTemplateGroup{
   private:
    std::list<std::pair<std::string, size_t>> window;
    std::map<std::string, ReadTemplate> data;
    size_t counter;

    void addEntry(const std::string& qname);
    void updateEntry(const std::string& qname);
    void removeEntry(std::string& qname);

   public:
    explicit ReadTemplateGroup();

    void addRecord(Record&& rec);
    void addRecords(std::list<Record>& recs);
    void addRecords(std::vector<Record>& recs);

    void resetCounter();

    bool getTemplate(ReadTemplate& t);
    bool getTemplate(ReadTemplate& t, const size_t& threshold);
    void getTemplates(std::list<ReadTemplate>& ts, const size_t& threshold);
};

// ---------------------------------------------------------------------------------------------------------------------
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_READER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------