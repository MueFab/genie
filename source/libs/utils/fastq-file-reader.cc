#include "fastq-file-reader.h"

#include <stdexcept>
#include <string>

namespace utils {

FastqFileReader::FastqFileReader(const std::string& path) : FileReader(path) {}

FastqFileReader::~FastqFileReader() = default;

size_t FastqFileReader::readRecords(const size_t numRecords, std::vector<FastqRecord>* const fastqRecords) {
    fastqRecords->clear();

    while (true) {
        // Try to read 4 lines, i.e. an entire FASTQ record
        FastqRecord fastqRecord;
        std::string line;

        // Try to read the title line
        readLine(&line);
        if (line.empty()) {
            return fastqRecords->size();
        }
        fastqRecord.title = line;

        readLine(&line);
        if (line.empty()) {
            throw std::runtime_error{"Truncated FASTQ record"};
        }
        fastqRecord.sequence = line;

        readLine(&line);
        if (line.empty()) {
            throw std::runtime_error{"Truncated FASTQ record"};
        }
        fastqRecord.optional = line;

        readLine(&line);
        if (line.empty()) {
            throw std::runtime_error{"Truncated FASTQ record"};
        }
        fastqRecord.qualityScores = line;

        fastqRecords->push_back(fastqRecord);

        if (fastqRecords->size() == numRecords) {
            return fastqRecords->size();
        }
    }
}

}  // namespace utils
