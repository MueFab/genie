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
    bool with_cache;

    std::map<std::string, std::vector<size_t>> cache;

   public:
    explicit Reader(std::istream& _stream, bool with_cache=true);

    const header::Header& getHeader() const;

    void addCacheEntry(std::string& qname, size_t &pos);

    bool read(std::list<std::string> lines);

    bool good();
};

// ---------------------------------------------------------------------------------------------------------------------

class ReadTemplateGroup{
   private:
    std::list<std::pair<std::string, size_t>> window;
    std::map<std::string, ReadTemplate> data;
    size_t counter;
    bool enable_window;

    void addEntry(const std::string& qname);
    void updateEntry(const std::string& qname);
    void removeEntry(const std::string& qname);

   public:
    explicit ReadTemplateGroup(bool _enable_window=false);

    void addRecord(Record&& rec);
    void addRecords(std::list<Record>& recs);
    void addRecords(std::vector<Record>& recs);
    void addRecords(std::list<std::string>& lines);

    void resetCounter();

    bool getTemplate(ReadTemplate& t, const size_t& threshold=0);
    void getTemplates(std::list<ReadTemplate>& ts, const size_t& threshold=0);
};

// ---------------------------------------------------------------------------------------------------------------------
}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_READER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------