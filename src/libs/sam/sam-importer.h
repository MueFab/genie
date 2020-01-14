#ifndef GENIE_SAM_IMPORTER_H
#define GENIE_SAM_IMPORTER_H

#include <backbone/format-importer.h>
#include <backbone/mpegg_rec/mpegg-record.h>
#include <util/log.h>
#include <util/make_unique.h>
#include <util/ordered-lock.h>
#include <util/ordered-section.h>
#include <util/original-source.h>
#include <util/source.h>
#include <list>
#include "sam-reader.h"
#include "sam-record.h"

namespace genie {
namespace sam {
class SamImporter : public genie::FormatImporter {
   private:
    size_t blockSize;
    SamReader samReader;
    util::OrderedLock lock;  //!< @brief Lock to ensure in order execution

   public:
    SamImporter(size_t _blockSize, std::istream &_file);

    static std::tuple<bool, uint8_t> convertFlags2Mpeg(uint16_t flags);

    static char convertCigar2ECigarChar(char token);

    static int stepSequence(char token);

    static std::string convertCigar2ECigar(const std::string &cigar, const std::string &seq);

    std::map<std::string, size_t> refs;
    size_t ref_counter;

    static genie::mpegg_rec::MpeggRecord convert(uint16_t ref, sam::SamRecord &&_r1, sam::SamRecord *_r2);

    bool pump(size_t id) override;

    void dryIn() override;
};
}  // namespace sam
}  // namespace genie
#endif  // GENIE_SAM_IMPORTER_H