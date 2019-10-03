#include "encoding.h"

#include <map>
#include <util/log.h>
#include <util/sam-file-reader.h>
#include <util/sam-record.h>
#include "exceptions.h"
#include "full-local-assembly-decoder.h"
#include "full-local-assembly-encoder.h"

namespace lae {
    void encode(const ProgramOptions &programOptions) {
        LOG_DEBUG << "Encoding";
        LOG_INFO << "Input file: " << programOptions.inputFilePath;

        util::SamFileReader samFileReader(programOptions.inputFilePath);

        size_t blockSize = 10000;
        bool singleEnd = false;
        const bool ENABLE_DECODING = true;
        const uint32_t SEQUENCE_BUFFER_SIZE = 1000;
        FullLocalAssemblyEncoder encoder(SEQUENCE_BUFFER_SIZE, true);
        uint32_t record_counter = 0;

        while (true) {
            // Read a block of SAM records
            std::list<util::SamRecord> samRecords;
            samFileReader.readRecords(blockSize, &samRecords);
            std::list<util::SamRecord> samRecordsCopy(samRecords);
            LOG_TRACE << "Read " << samRecords.size() << " SAM record(s)";
            for (const auto &samRecord : samRecords) {
                record_counter++;
                // Search for mate
                std::string rnameSearchString;
                if (samRecord.rnext == "=") {
                    rnameSearchString = samRecord.rname;
                } else {
                    rnameSearchString = samRecord.rnext;
                }
                samRecordsCopy.erase(samRecordsCopy.begin());  // delete current record from the search space
                bool foundMate = false;
                for (auto it = samRecordsCopy.begin(); it != samRecordsCopy.end(); ++it) {
                    if (it->rname == rnameSearchString && it->pos == samRecord.pnext) {
                        LOG_TRACE << "Found mate";
                        encoder.addPair(samRecord, *it);
                        foundMate = true;
                        samRecordsCopy.erase(it);
                        break;
                    }
                }
                if (!foundMate) {
                    LOG_TRACE << "Did not find mate";
                    singleEnd = true;
                    encoder.add(samRecord);
                }

                // Break if everything was processed
                if (samRecordsCopy.empty()) {
                    break;
                }
            }

            // Break if less than blockSize records were read from the SAM file
            if (samRecords.size() < blockSize) {
                break;
            }
        }

        if (ENABLE_DECODING) {
            FullLocalAssemblyDecoder decoder(encoder.pollStreams(), SEQUENCE_BUFFER_SIZE, true);
            util::SamRecord s;
            util::SamRecord s2;
            if (singleEnd) {
                for (uint32_t i = 0; i < record_counter; ++i) {
                    decoder.decode(&s);
                }
            } else {
                for (uint32_t i = 0; i < record_counter / 2; ++i) {
                    decoder.decodePair(&s, &s2);
                }
            }
        }

    }

}  // namespace lae
