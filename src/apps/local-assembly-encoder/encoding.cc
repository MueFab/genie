#include "encoding.h"

#include <map>
#include <util/log.h>
#include <util/sam-file-reader.h>
#include <util/sam-record.h>
#include "exceptions.h"
#include "read-encoder.h"
#include "read-decoder.h"
#include "reference-encoder.h"

namespace lae {

    void add(const util::SamRecord& rec,  LocalAssemblyReferenceEncoder* le, LocalAssemblyReadEncoder* lre) {
        std::string ref = le->getReference(rec.pos, rec.cigar);
        le->addRead(rec);
        lre->addRead(rec, ref);
        le->printWindow();
        std::cout << "ref: " << ref << std::endl;
        std::cout << std::endl;
    }

    void decode(LocalAssemblyReferenceEncoder* le, LocalAssemblyReadDecoder* lrd, util::SamRecord *s) {
        static uint32_t abs_pos = 0;
        abs_pos += lrd->offsetOfNextRead();
        std::string ref = le->getReference(abs_pos, lrd->lengthOfNextRead());
        lrd->decodeRead(ref, s);
        le->addRead(*s);
        std::cout << "R: " << s->seq << std::endl;
        std::cout << "C: " << s->cigar << std::endl;
        std::cout << "P: " << s->pos << std::endl << std::endl;
    }

    void test() {

        LocalAssemblyReadEncoder lre;
        LocalAssemblyReferenceEncoder le(26 * 3);
        util::SamRecord s;

        s.seq = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        s.cigar = "26M";
        s.pos = 0;
        add(s, &le, &lre);

        s.seq = "BCDEFGHIJKLMNOPQRSTUVWXYZ";
        s.cigar = "25M";
        s.pos = 30;
        add(s, &le, &lre);

        s.seq = "CDEFGHIJKLMNOPQRSTUV123WXYZA6";
        s.cigar = "20M10D3I3D5M5D1M";
        s.pos = 31;
        add(s, &le, &lre);

        s.seq = "DEFGHIJKLMNOPQ8STUVWXYZAB";
        s.cigar = "25M";
        s.pos = 32;
        add(s, &le, &lre);

        LocalAssemblyReadDecoder lrd(lre.pollStreams());
        le = LocalAssemblyReferenceEncoder(26 * 3);

        decode(&le, &lrd, &s);
        decode(&le, &lrd, &s);
        decode(&le, &lrd, &s);
        decode(&le, &lrd, &s);
    }

    void encode(const ProgramOptions &programOptions) {
        LOG_DEBUG << "Encoding";
        LOG_INFO << "Input file: " << programOptions.inputFilePath;

        util::SamFileReader samFileReader(programOptions.inputFilePath);

        test();

        size_t blockSize = 10000;
        while (true) {
            // Read a block of SAM records
            std::list<util::SamRecord> samRecords;
            samFileReader.readRecords(blockSize, &samRecords);
            std::list<util::SamRecord> samRecordsCopy(samRecords);
            LOG_TRACE << "Read " << samRecords.size() << " SAM record(s)";

            // Set up the decoded descriptor streams
            std::vector<int64_t> decodedPos;
            int64_t prevPos = 0;

            for (const auto &samRecord : samRecords) {
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
                        foundMate = true;
                        samRecordsCopy.erase(it);
                        break;
                    }
                }
                if (!foundMate) {
                    LOG_TRACE << "Did not find mate";
                }

                // pos
                decodedPos.push_back(samRecord.pos - prevPos);
                prevPos = samRecord.pos;

                // Break if everything was processed
                if (samRecordsCopy.empty()) {
                    break;
                }
            }

            LocalAssemblyReadEncoder lre;
            LocalAssemblyReferenceEncoder le(1000);

            for (const auto &samRecord : samRecords) {
                add(samRecord, &le, &lre);
            }

            // Break if less than blockSize records were read from the SAM file
            if (samRecords.size() < blockSize) {
                break;
            }
        }
    }

}  // namespace lae
