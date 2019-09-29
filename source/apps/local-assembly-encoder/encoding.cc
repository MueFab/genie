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

    void test() {
        LocalAssemblyReferenceEncoder le(26 * 3);
        le.addRead("ABCDEFGHIJKLMNOPQRSTUVWXYZ", "26M", 0);
        le.printWindow();
        std::cout << "ref: " << le.ref << std::endl;
        std::cout << std::endl;
        le.addRead("BCDEFGHIJKLMNOPQRSTUVWXYZ", "25M", 30);
        le.printWindow();
        std::cout << "ref: " << le.ref << std::endl;
        std::cout << std::endl;
        le.addRead("CDEFGHIJKLMNOPQRSTUVWXYZ", "24M", 1);
        le.printWindow();
        std::cout << "ref: " << le.ref << std::endl;
        std::cout << std::endl;
        le.addRead("DEFGHIJKLMNOPQRSTUVWXYZ", "23M", 1);
        le.printWindow();
        std::cout << "ref: " << le.ref << std::endl;
        std::cout << std::endl;

        std::string ref;
        le.finish(&ref);

        std::cout << "ref: " << ref << std::endl;

        LocalAssemblyReadEncoder lre(&ref);
        lre.addRead("AB123ZGHIJKLMNOPQRSTUVWXYZ", "2M3D3I21M", 0);
        lre.addRead("BCDEFGHIJKLMNOPQRSTUVWXYZ", "25M", 30);
        lre.addRead("EFGHIJKLMNOPQRSTUVWZZZ", "22M", 3);
        lre.addRead("DEFGHIJKLMNOPQRSTUVWXYZ", "23M", 1);

        LocalAssemblyReadDecoder lrd(&ref, lre.pollStreams());

        std::string r, q;
        uint64_t p;
        lrd.decodeRead(&r, &q, &p);
        std::cout << "R: " << r << std::endl;
        std::cout << "C: " << q << std::endl;
        std::cout << "P: " << p << std::endl << std::endl;
        lrd.decodeRead(&r, &q, &p);
        std::cout << "R: " << r << std::endl;
        std::cout << "C: " << q << std::endl;
        std::cout << "P: " << p << std::endl << std::endl;
        lrd.decodeRead(&r, &q, &p);
        std::cout << "R: " << r << std::endl;
        std::cout << "C: " << q << std::endl;
        std::cout << "P: " << p << std::endl << std::endl;
        lrd.decodeRead(&r, &q, &p);
        std::cout << "R: " << r << std::endl;
        std::cout << "C: " << q << std::endl;
        std::cout << "P: " << p << std::endl << std::endl;
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

            // Break if less than blockSize records were read from the SAM file
            if (samRecords.size() < blockSize) {
                break;
            }
        }
    }

}  // namespace lae
