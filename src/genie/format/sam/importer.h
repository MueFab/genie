/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_SAM_IMPORTER_H
#define GENIE_SAM_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/format-importer.h>
#include <genie/core/record/record.h>
#include <genie/core/stats/perf-stats.h>
#include <genie/util/ordered-lock.h>
#include <list>
#include <map>
#include "reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace sam {

// ---------------------------------------------------------------------------------------------------------------------

class Importer : public core::FormatImporter {
   private:
    size_t blockSize;
    Reader samReader;
    genie::core::stats::SamStats *stats;  //!< @brief Stats collector (null => don't collect)

   public:
    Importer(size_t _blockSize, std::istream &_file, genie::core::stats::SamStats *_stats = nullptr);

    static std::tuple<bool, uint8_t> convertFlags2Mpeg(uint16_t flags);

    static char convertCigar2ECigarChar(char token);

    static int stepSequence(char token);

    static std::string convertCigar2ECigar(const std::string &cigar, const std::string &seq);

    std::map<std::string, size_t> refs;
    size_t ref_counter;

    static core::record::Record convert(uint16_t ref, sam::Record &&_r1, sam::Record *_r2);
    bool pumpRetrieve(genie::core::Classifier *_classifier) override {
        core::record::Chunk chunk;
        std::vector<sam::Record> s;
        std::list<sam::Record> samRecords;
        uint16_t local_ref_num = 0;
        {
            // TODO: util::OrderedSection section(&lock, id);
            samReader.read(blockSize, s, stats);
            if (s.size() == 0) {
                return false;
            }
            auto it = refs.find(s.front().getRname());
            if (it == refs.end()) {
                local_ref_num = ref_counter;
                refs.insert(std::make_pair(s.front().getRname(), ref_counter++));
            } else {
                local_ref_num = it->second;
            }
        }
        std::copy(s.begin(), s.end(), std::back_inserter(samRecords));

        std::cout << "Read " << samRecords.size() << " SAM record(s) " << std::endl;
        size_t skipped = 0;
        while (!samRecords.empty()) {
            sam::Record samRecord = std::move(samRecords.front());
            samRecords.erase(samRecords.begin());
            // Search for mate
            const std::string &rnameSearchString =
                samRecord.getRnext() == "=" ? samRecord.getRname() : samRecord.getRnext();
            auto mate = samRecords.begin();
            //   mate = samRecords.end();  // Disable pairs for now TODO: implement
            if (samRecord.getPnext() == samRecord.getPos() && samRecord.getRname() == rnameSearchString) {
                mate = samRecords.end();
            }
            for (; mate != samRecords.end(); ++mate) {
                if (mate->getRname() == rnameSearchString && mate->getPos() == samRecord.getPnext()) {
                    // LOG_TRACE << "Found mate";
                    break;
                }
            }
            if (mate == samRecords.end()) {
                // LOG_TRACE << "Did not find mate";
                if ((samRecord.getFlags() & (1u << uint16_t(Record::FlagPos::SEGMENT_UNMAPPED))) ||
                    samRecord.getCigar() == "*" || samRecord.getPos() == 0 || samRecord.getRname() == "*") {
                    skipped++;
                } else {
                    chunk.emplace_back(convert(local_ref_num, std::move(samRecord), nullptr));
                }
            } else {
                // TODO: note the filtering of unaligned reads above. Move this to the encoder
                chunk.emplace_back(convert(local_ref_num, std::move(samRecord), &*mate));
                samRecords.erase(mate);
            }
        }
        if (skipped) {
            std::cerr << "Skipped " << skipped << " unmapped reads! Those are currently not supported." << std::endl;
        }
        if (!chunk.empty()) {
            _classifier->add(std::move(chunk));
        }

        // Break if less than blockSize records were read from the SAM file
        return !samReader.isEnd();
    }

    void dryIn() override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_IMPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
