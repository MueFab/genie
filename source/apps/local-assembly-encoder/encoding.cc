#include "encoding.h"

#include <fio/sam-file-reader.h>
#include <fio/sam-record.h>
#include <utils/log.h>

namespace lae {

    void encode(const ProgramOptions &programOptions) {
        LOG_DEBUG << "Encoding";
        LOG_INFO << "Input file: " << programOptions.inputFilePath;

        fio::SamFileReader samFileReader(programOptions.inputFilePath);

        std::vector<fio::SamRecord> samRecords;

        size_t blockSize = 10000;

        while (true) {
            samFileReader.readRecords(blockSize, &samRecords);
            LOG_TRACE << "Read " << samRecords.size() << " SAM record(s)";
            break;
        }
    }

}  // namespace lae
