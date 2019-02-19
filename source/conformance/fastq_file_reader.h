#ifndef GENIE_FASTQFILEREADER_H_
#define GENIE_FASTQFILEREADER_H_


#include <string>
#include <vector>

#include "conformance/file_reader.h"
#include "conformance/fastq_record.h"


namespace genie {


class FastqFileReader : public FileReader {
 public:
    FastqFileReader(
        const std::string& path);

    ~FastqFileReader(void);

    size_t readRecords(
        const size_t numRecords,
        std::vector<FastqRecord> * const records);
};


}  // namespace genie


#endif  // GENIE_FASTQFILEREADER_H_
