#include "fasta-file-reader.h"

#include <string>

namespace fio {

    FastaFileReader::FastaFileReader(const std::string &path) : FileReader(path) {}

    FastaFileReader::~FastaFileReader() = default;

    void FastaFileReader::parse(std::vector<FastaRecord> *const fastaRecords) {
        // Reset file pointer to the beginning of the file
        size_t fpos = tell();
        seekFromSet(0);

        std::string currentHeader;
        std::string currentSequence;

        while (true) {
            // Read a line
            std::string line;
            readLine(&line);
            if (line.empty()) {
                break;
            }

            // Process line
            if (line[0] == '>') {
                // Store the previous FASTA record, if there is one
                if (!currentSequence.empty()) {
                    // We have a sequence, check if we have a header
                    if (currentHeader.empty()) {
                        throw std::runtime_error{"Found FASTA sequence, but no header"};
                    }

                    FastaRecord currentFastaRecord(currentHeader, currentSequence);
                    fastaRecords->push_back(currentFastaRecord);

                    currentHeader = "";
                    currentSequence = "";
                }

                // Store the header and trim it: remove everything after the first space
                currentHeader = line;
                currentHeader = currentHeader.substr(0, currentHeader.find_first_of(" "));
            } else {
                currentSequence += line;
            }
        }

        FastaRecord currentFastaRecord(currentHeader, currentSequence);
        fastaRecords->push_back(currentFastaRecord);

        seekFromSet(fpos);
    }


}  // namespace fio
