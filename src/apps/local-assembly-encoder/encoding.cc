#include "encoding.h"

#include <map>
#include <util/log.h>
#include <util/sam-file-reader.h>
#include <util/sam-record.h>
#include "exceptions.h"
#include "read-encoder.h"
#include "read-decoder.h"
#include "reference-encoder.h"
#include "reference-encoder-plainc.h"
namespace lae {

    static uint32_t abs_pos = 0;
    static LOCAL_ASSEMBLY_STATE* state;

    void add(const util::SamRecord& rec,  LocalAssemblyReferenceEncoder* le, LocalAssemblyReadEncoder* lre) {
        std::string ref = le->getReference(rec.pos, rec.cigar);
        le->addRead(rec);
        lre->addRead(rec, ref);
        le->printWindow();
        std::cout << "ref: " << std::endl;

        for(size_t i = 0; i < rec.pos - le->getWindowBorder(); ++i) {
            std::cout << " ";
        }

        std::cout << ref << std::endl;
        std::cout << std::endl;
    }

    void addPair(const util::SamRecord& rec1, const util::SamRecord& rec2,  LocalAssemblyReferenceEncoder* le, LocalAssemblyReadEncoder* lre) {
        std::string ref1 = le->getReference(rec1.pos, rec1.cigar);
        std::string ref2 = le->getReference(rec2.pos, rec2.cigar);
        le->addRead(rec1);
        le->addRead(rec2);
        lre->addPair(rec1, ref1, rec2, ref2);
        le->printWindow();

        std::cout << "pair!" << std::endl;
        std::cout << "ref1: " << std::endl;
        for(size_t i = 0; i < rec1.pos - le->getWindowBorder(); ++i) {
            std::cout << " ";
        }
        std::cout << ref1 << std::endl;

        std::cout << "ref2: " << std::endl;
        for(size_t i = 0; i < rec2.pos - le->getWindowBorder(); ++i) {
            std::cout << " ";
        }
        std::cout << ref2 << std::endl;
        std::cout << std::endl;
    }

    void decode(LocalAssemblyReferenceEncoder* letmp, LocalAssemblyReadDecoder* lrd, util::SamRecord *s) {
        abs_pos += lrd->offsetOfNextRead();

        char* refbuf;
        local_assembly_state_get_ref(state, abs_pos, lrd->lengthOfNextRead(), &refbuf);
        std::string ref = std::string(refbuf, refbuf + lrd->lengthOfNextRead());
        free(refbuf);

        lrd->decodeRead(ref, s);

        local_assembly_state_add_read(state, s->seq.c_str(), s->cigar.c_str(), s->pos);

        std::cout << "R: " << s->seq << std::endl;
        std::cout << "C: " << s->cigar << std::endl;
        std::cout << "P: " << s->pos << std::endl << std::endl;
    }

    void decodePair(LocalAssemblyReferenceEncoder* letmp, LocalAssemblyReadDecoder* lrd, util::SamRecord *s, util::SamRecord *s2) {
        abs_pos += lrd->offsetOfNextRead();

        char* refbuf;
        local_assembly_state_get_ref(state, abs_pos, lrd->lengthOfNextRead(), &refbuf);
        std::string ref1 = std::string(refbuf, refbuf + lrd->lengthOfNextRead());
        free(refbuf);

        abs_pos += lrd->offsetOfSecondNextRead();
        local_assembly_state_get_ref(state, abs_pos, lrd->lengthOfSecondNextRead(), &refbuf);
        std::string ref2 = std::string(refbuf, refbuf + lrd->lengthOfSecondNextRead());
        free(refbuf);

        uint32_t delta;
        bool first;
        lrd->decodePair(ref1, s, ref2, s2, &delta, &first);
        local_assembly_state_add_read(state, s->seq.c_str(), s->cigar.c_str(), s->pos);
        local_assembly_state_add_read(state, s2->seq.c_str(), s2->cigar.c_str(), s2->pos);
        std::cout << "Decoded pair! First1: " << first << " Delta: " << delta << std::endl;
        std::cout << "R: " << s->seq << std::endl;
        std::cout << "C: " << s->cigar << std::endl;
        std::cout << "P: " << s->pos << std::endl << std::endl;
        std::cout << "R: " << s2->seq << std::endl;
        std::cout << "C: " << s2->cigar << std::endl;
        std::cout << "P: " << s2->pos << std::endl << std::endl;
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

    //    decode(&le, &lrd, &s);
    //    decode(&le, &lrd, &s);
    //    decode(&le, &lrd, &s);
    //    decode(&le, &lrd, &s);
    }

    void encode(const ProgramOptions &programOptions) {
        LOG_DEBUG << "Encoding";
        LOG_INFO << "Input file: " << programOptions.inputFilePath;

        util::SamFileReader samFileReader(programOptions.inputFilePath);

        test();

        LocalAssemblyReadEncoder lre;
        LocalAssemblyReferenceEncoder le(1000);
        size_t blockSize = 10000;
        bool single = false;
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
                        addPair(samRecord, *it, &le, &lre);
                        foundMate = true;
                        samRecordsCopy.erase(it);
                        break;
                    }
                }
                if (!foundMate) {
                    LOG_TRACE << "Did not find mate";
                    single = true;
                    add(samRecord, &le, &lre);
                }

                // pos
                decodedPos.push_back(samRecord.pos - prevPos);
                prevPos = samRecord.pos;

                // Break if everything was processed
                if (samRecordsCopy.empty()) {
                    break;
                }
            }

            LocalAssemblyReadDecoder lrd(lre.pollStreams());
            le = LocalAssemblyReferenceEncoder(1000);

            util::SamRecord s;
            util::SamRecord s2;
            local_assembly_state_create(&state);
            local_assembly_state_init(state, 1000, 5);
            abs_pos = 0;
            if(single) {
                for (const auto &samRecord : samRecords) {
                    decode(&le, &lrd, &s);
                }
            } else {
                for (int i = 0; i < samRecords.size()/2; ++i) {
                    decodePair(&le, &lrd, &s, &s2);
                }
            }

            local_assembly_state_destroy(state);

            // Break if less than blockSize records were read from the SAM file
            if (samRecords.size() < blockSize) {
                break;
            }
        }
    }

}  // namespace lae
