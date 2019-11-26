#include "sam-file-reader.h"
#include <string>
#include "log.h"
#include "sam-record.h"
#include "string-helpers.h"

namespace util {

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

SamFileReader::SamFileReader(const std::string &path) : FileReader(path) { readHeader(); }

SamFileReader::~SamFileReader() = default;

size_t SamFileReader::readRecords(const size_t numRecords, std::list<SamRecord> *const records) {
    for (size_t i = 0; i < numRecords; i++) {
        // Read a line
        std::string line;
        readLine(&line);
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
    seekFromSet(0);

    size_t fpos = 0;

    while (true) {
        // Remember the file pointer position
        fpos = tell();

        // Read a line
        std::string line;
        readLine(&line);

        // Add the line contents to the header
        if (line[0] == '@') {
            header += line;
            header += "\n";
        } else {
            break;
        }
    }

    // Rewind to the beginning of the the alignment section
    seekFromSet(fpos);

    if (header.empty()) {
        LOG_WARNING << "SAM header not present";
    }
}

}  // namespace util
