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
    std::map<std::string, uint16_t> refs;
    bool with_index;

    std::map<std::string, std::vector<size_t>> index;

   public:
    /**
     * @brief Initialize Reader given istream, allows index to group reads that belongs to the same template
     *
     * @param _stream: Input stream of sam records
     * @param _with_index: 
     */
    explicit Reader(std::istream& _stream, bool _with_index=true);

    /**
     *
     * @return
     */
    const header::Header& getHeader() const;

    /**
     * Return references found in sam file
     *
     * @return map contains RNAME and ref_id pair
     */
    std::map<std::string, uint16_t>& getRefs();

    /**
     * Add qname and file position pair to cache for indexing purpose
     *
     * @param qname: QNAME of sam record
     * @param pos: Position of sam record in the sam file
     */
    void addCacheEntry(std::string& qname, size_t &pos);

    /**
     * Read sam records
     *
     * @param lines
     * @return true, if there is any remaining sam records to be read
     */
    bool read(std::list<std::string>& lines);

    /**
     * Return the current status of Reader
     *
     * @return true, if istream is good
     */
    bool good();
};

// ---------------------------------------------------------------------------------------------------------------------

class ReadTemplateGroup{
   private:
    std::list<std::pair<std::string, size_t>> window;
    std::map<std::string, ReadTemplate> data;
    size_t counter;
    bool enable_window;

    /**
     *
     * @param qname
     */
    void addEntry(const std::string& qname);

    /**
     *
     * @param qname
     */
    void updateEntry(const std::string& qname);

    /**
     *
     * @param qname
     */
    void removeEntry(const std::string& qname);

   public:
    /**
     *
     * @param _enable_window
     */
    explicit ReadTemplateGroup(bool _enable_window=false);

    /**
     *
     * @param rec
     */
    void addRecord(Record&& rec);

    /**
     *
     * @param recs
     */
    void addRecords(std::list<Record>& recs);

    /**
     *
     * @param recs
     */
    void addRecords(std::vector<Record>& recs);

    /**
     *
     * @param lines
     */
    void addRecords(std::list<std::string>& lines);

    /**
     *
     */
    void resetCounter();

    /**
     *
     * @param t
     * @param threshold
     * @return
     */
    bool getTemplate(ReadTemplate& t, const size_t& threshold=0);

    /**
     *
     * @param ts
     * @param threshold
     */
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