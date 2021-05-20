#ifndef GENIE_SAM_SORTER_H
#define GENIE_SAM_SORTER_H

#include <fstream>
#include <genie/util/bitwriter.h>
#include <genie/util/bitreader.h>
#include <genie/core/record/record.h>

namespace sam_transcoder {

class PerRIDWriter{
   private:

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

    genie::core::record::Record&& moveRecord();

    const genie::core::record::Record& getRecord() const;

    void writeRecord(genie::util::BitWriter& bitwriter);

    uint64_t getPos() const;

    bool good();
    void close();
};

uint64_t getMinPos(const genie::core::record::Record& r);

bool compare(const genie::core::record::Record &r1, const genie::core::record::Record &r2);

}

#endif  // GENIE_SAM_SORTER_H
