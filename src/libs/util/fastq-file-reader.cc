#include "fastq-file-reader.h"
#include <stdexcept>
#include <string>

namespace util {

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

  /*      if (stats->enabled) {
            stats->num_recs++;
            stats->orig_id_sz += fastqRecord.title.size();
            stats->orig_seq_sz += fastqRecord.sequence.size();
            stats->orig_qual_sz += fastqRecord.qualityScores.size();
            stats->orig_total_sz += (fastqRecord.title.size() + fastqRecord.sequence.size()
              + fastqRecord.optional.size() + fastqRecord.qualityScores.size() + 4);
        } */

        if (fastqRecords->size() == numRecords) {
            return fastqRecords->size();
        }
    }
}

}  // namespace util
