#include "read-encoder.h"
#include "exceptions.h"

namespace lae {
    LocalAssemblyReadEncoder::LocalAssemblyReadEncoder() : container(new StreamContainer()), pos(0), readCounter(0){

    }

    void LocalAssemblyReadEncoder::addRead(const util::SamRecord& rec, const std::string& ref) {
        container->pos_0.push_back(rec.pos - pos);
        pos = rec.pos;

        codeVariants(rec.seq, rec.cigar, ref);
        container->rlen_0.push_back(ref.length());

        container->mmpos_0.push_back(1);
        container->rcomp_0.push_back((rec.flag >> 4) & 0x1);
        container->flags_0.push_back((rec.flag >> 10) & 0x1);
        container->flags_1.push_back((rec.flag >> 9) & 0x1);
        container->flags_2.push_back((rec.flag >> 1) & 0x1);

        container->mscore_0.push_back(rec.mapq);

        container->rtype_0.push_back(4);

        readCounter++;
    }

    void LocalAssemblyReadEncoder::addPair(const util::SamRecord& rec1, const std::string& ref1, const util::SamRecord& rec2, const std::string& ref2) {
        addRead(rec1, ref1);
        addRead(rec2, ref2);
        container->pair_0.push_back(0);

        uint32_t delta = std::max(rec1.pos, rec2.pos) - std::min(rec1.pos, rec2.pos);
        bool first1 = rec1.pos < rec2.pos;

        container->pair_1.push_back((delta << 1) | first1);
    }
    void
    LocalAssemblyReadEncoder::codeVariants(const std::string &read, const std::string &cigar, const std::string& ref) {
        size_t count = 0;
        size_t read_pos = 0;
        size_t ref_offset = 0;
        size_t num_of_deletions = 0;

        size_t lastMisMatch = 0;

        bool foundClip = false;

        for (size_t cigar_pos = 0; cigar_pos < cigar.length(); ++cigar_pos) {
            if (cigar[cigar_pos] >= '0' && cigar[cigar_pos] <= '9') {
                count *= 10;
                count += cigar[cigar_pos] - '0';
                continue;
            }
            switch (cigar[cigar_pos]) {
                case 'M':
                case '=':
                case 'X':
                    for (size_t i = 0; i < count; ++i) {
                        if (read_pos >= read.length()) {
                            GENIE_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
                        }
                        if (read[read_pos] != ref[ref_offset]) {
                            if(ref[ref_offset] == 0) {
                                if(!foundClip){
                                    foundClip = true;
                                    container->clips_0.push_back(readCounter);
                                }
                                container->clips_2.push_back(read[read_pos]);
                            } else {
                                container->mmpos_0.push_back(0);
                                container->mmpos_1.push_back(read_pos - lastMisMatch);
                                lastMisMatch = read_pos + 1;
                                container->mmtype_0.push_back(0);
                                container->mmtype_1.push_back(read[read_pos]);
                            }
                        }
                        read_pos++;
                        ref_offset++;
                    }
                    break;

                case 'I':
                    for (size_t i = 0; i < count; ++i) {
                        container->mmpos_0.push_back(0);
                        container->mmpos_1.push_back(read_pos - lastMisMatch);
                        lastMisMatch = read_pos + 1;
                        container->mmtype_0.push_back(1);
                        container->mmtype_2.push_back(read[read_pos]);
                        read_pos++;
                    }
                    break;
                case 'S':
                case 'P':
                    read_pos += count;
                    // softclip
                    break;

                case 'N':
                case 'D':
                    for (size_t i = 0; i < count; ++i) {
                        container->mmpos_0.push_back(0);
                        container->mmpos_1.push_back(read_pos - lastMisMatch);
                        lastMisMatch = read_pos;
                        container->mmtype_0.push_back(2);
                        num_of_deletions++;
                        ref_offset++;
                    }
                    break;
                case 'H':

                    ref_offset += count;
                    // Hardclip
                    break;
                default:
                    GENIE_THROW_RUNTIME_EXCEPTION("Unknown CIGAR character");
            }
            count = 0;
        }

        if(foundClip) {
            container->clips_1.push_back(1);
            container->clips_1.push_back(8); // Terminator
            container->clips_2.push_back(5); // Terminator
        }

        if (read_pos != read.length()) {
            GENIE_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
        }
    }

    std::unique_ptr<StreamContainer> LocalAssemblyReadEncoder::pollStreams() {
        auto tmp = std::move(container);
        container = std::unique_ptr<StreamContainer>(new StreamContainer());
        return tmp;
    }
}