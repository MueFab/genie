#include "read-encoder.h"
#include "exceptions.h"

namespace lae {
    LocalAssemblyReadEncoder::LocalAssemblyReadEncoder() : container(new StreamContainer()), pos(0){

    }

    uint64_t LocalAssemblyReadEncoder::pop(std::vector<uint64_t> *vec) {
        uint64_t ret = vec->front();
        vec->erase(vec->begin());
        return ret;
    }

    void LocalAssemblyReadEncoder::addRead(const util::SamRecord& rec, const std::string& ref) {
        container->pos_0.push_back(rec.pos - pos);
        pos = rec.pos;

        codeVariants(rec.seq, rec.cigar, ref);
        container->rlen_0.push_back(ref.length());

        container->mmpos_0.push_back(1);
    }
    void
    LocalAssemblyReadEncoder::codeVariants(const std::string &read, const std::string &cigar, const std::string& ref) {
        size_t count = 0;
        size_t read_pos = 0;
        size_t ref_offset = 0;
        size_t num_of_deletions = 0;

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
                                container->clips_0.push_back(read[read_pos]);
                            } else {
                                container->mmpos_0.push_back(0);
                                container->mmpos_1.push_back(read_pos);
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
                        container->mmpos_1.push_back(read_pos);
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
                        container->mmpos_1.push_back(read_pos);
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