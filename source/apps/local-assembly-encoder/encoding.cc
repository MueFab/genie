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

            std::vector<int64_t> decodedPos;
            int64_t prevPos = 0;

            for (const auto samRecord : samRecords) {
                samRecords.erase(samRecords.begin());

                // LOG_TRACE << "Encoding SAM record";

                // LOG_TRACE << "Searching mate";
                // LOG_TRACE << "RNAME:POS = " << samRecord.rname << ":" << samRecord.pos;
                // LOG_TRACE << "RNEXT:PNEXT = " << samRecord.rnext << ":" << samRecord.pnext;

                bool foundMate = false;
                for (const auto &mateSamRecord : samRecords) {
                    if ((mateSamRecord.rname == samRecord.rname) && (mateSamRecord.pos == samRecord.pos)) {
                        LOG_TRACE << "Found mate";
                        foundMate = true;
                    }
                }
                if (!foundMate) {
                    LOG_TRACE << "Did not find mate";
                }

                // pos
                decodedPos.push_back(samRecord.pos - prevPos);
                prevPos = samRecord.pos;
            }

            // Break if less than blockSize records were read from the SAM file
            if (samRecords.size() < blockSize) {
                break;
            }
        }
    }

}  // namespace lae
