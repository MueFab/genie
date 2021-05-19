#ifndef GENIE_SAM_SORTER_H
#define GENIE_SAM_SORTER_H

#include <fstream>
#include <genie/util/bitwriter.h>
#include <genie/util/bitreader.h>
#include <genie/core/record/record.h>

namespace sam_transcoder {

class SubfileWriter {
   private:
    uint64_t curr_mgrec_pos;
    std::ofstream writer;
    genie::util::BitWriter bitwriter;

   public:
    explicit SubfileWriter(const std::string& fpath);

    bool writeUnmappedRecord(genie::core::record::Record& rec);

    bool writeRecord(genie::core::record::Record& rec);

    void close();
};

class SubfileReader {
   private:
    uint64_t curr_mgrec_pos;
    std::ifstream reader;
    genie::util::BitReader bitreader;
    genie::core::record::Record rec;

   public:
    explicit SubfileReader(const std::string& fpath);

    ~SubfileReader();

    bool readRecord();
    void writeRecord(genie::util::BitWriter& bitwriter);

    uint64_t getPos() const;
};

uint64_t getMinPos(const genie::core::record::Record& r);

bool compare(const genie::core::record::Record &r1, const genie::core::record::Record &r2);

}

#endif  // GENIE_SAM_SORTER_H
