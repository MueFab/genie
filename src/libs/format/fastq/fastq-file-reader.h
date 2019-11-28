#ifndef UTIL_FASTQ_FILE_READER_H_
#define UTIL_FASTQ_FILE_READER_H_

#include <string>
#include <vector>
#include "fastq-record.h"
#include "util/file-reader.h"
#include "util/perf-stats.h"

namespace format {
    namespace fastq {

    class FastqFileReader : public util::FileReader {
        public:
            FastqFileReader(const std::string &path/*, FastqStats *_stats*/);

            ~FastqFileReader();

            size_t readRecords(const size_t numRecords, std::vector<FastqRecord> *const records);
        };

    }  // namespace util
}

#endif  // UTIL_FASTQ_FILE_READER_H_
