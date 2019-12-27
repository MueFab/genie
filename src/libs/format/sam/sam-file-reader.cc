#include "sam-file-reader.h"
#include <string>
#include "util/log.h"
#include "sam-record.h"
#include "util/string-helpers.h"

namespace format {
    namespace sam {

        static void parseLine(const std::string &line, std::vector<std::string> *const fields) {
            fields->clear();
            fields->push_back("");

            int fieldCount = 0;

            for (const auto &c : line) {
                if (c == '\t' && fieldCount <= 10) {
                    fieldCount++;
                    fields->push_back("");
                }
                (*fields)[fieldCount] += c;
            }

            for (auto &field : *fields) {
                util::trim(field);
            }
        }

        SamFileReader::SamFileReader(std::istream* _in)  : in(_in)
        {
            readHeader();
        }

        SamFileReader::~SamFileReader() = default;

        size_t SamFileReader::readRecords(const size_t numRecords, std::list<SamRecord> *const records) {
            for (size_t i = 0; i < numRecords; i++) {
                // Read a line
                std::string line;
                std::getline(*in, line);
                if (line.empty()) {
                    break;
                }

                // Parse line and construct samRecord
                std::vector<std::string> fields;
                parseLine(line, &fields);
                SamRecord samRecord(fields);

                //        std::cout << samRecord.str(); //Not sure if this line is needed or just debug
                records->push_back(samRecord);
            }

            return records->size();
        }

        void SamFileReader::readHeader() {
            // Set file pointer to the beginning of the file
            in->seekg(0, std::ios::seekdir::_S_beg);

            size_t fpos = 0;

            while (true) {
                // Remember the file pointer position
                fpos = in->tellg();

                // Read a line
                std::string line;
                std::getline(*in, line);

                // Add the line contents to the header
                if (line[0] == '@') {
                    header += line;
                    header += "\n";
                } else {
                    break;
                }
            }

            // Rewind to the beginning of the the alignment section
            in->seekg(fpos);

            if (header.empty()) {
                LOG_WARNING << "SAM header not present";
            }
        }

    }
}  // namespace util