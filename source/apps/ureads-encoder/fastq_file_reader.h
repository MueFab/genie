#ifndef GENIE_FASTQ_FILE_READER_H_
#define GENIE_FASTQ_FILE_READER_H_


#include <string>
#include <vector>

#include "fastq_record.h"
#include "file_reader.h"


namespace genie {


class FastqFileReader : public FileReader {
 public:
    explicit FastqFileReader(const std::string& path);
    ~FastqFileReader() override;
    size_t readRecords(size_t numRecords, std::vector<FastqRecord>* records);
};


}  // namespace genie


#endif  // GENIE_FASTQ_FILE_READER_H_
