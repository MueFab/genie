#include "read-encoder.h"
#include "exceptions.h"

namespace lae {
    LocalAssemblyReadEncoder::LocalAssemblyReadEncoder(
            std::string *_ref
    ) : ref(_ref),
        posCounter(0),
        container(new StreamContainer()){

    }

    uint64_t LocalAssemblyReadEncoder::pop(std::vector<uint64_t> *vec) {
        uint64_t ret = vec->front();
        vec->erase(vec->begin());
        return ret;
    }

    void LocalAssemblyReadEncoder::addRead(const std::string &read_raw, const std::string &cigar, uint32_t pos_offset) {
        container->pos_0.push_back(pos_offset);
        posCounter += pos_offset;

        auto len = codeVariants(read_raw, cigar, posCounter) - posCounter;
        container->rlen_0.push_back(len);

        container->mmpos_0.push_back(1);
    }
    uint64_t
    LocalAssemblyReadEncoder::codeVariants(const std::string &read, const std::string &cigar, uint64_t ref_offset) {
        size_t count = 0;
        size_t read_pos = 0;

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
                        if (read[read_pos] != (*ref)[ref_offset]) {
                            container->mmpos_0.push_back(0);
                            container->mmpos_1.push_back(read_pos);
                            container->mmtype_0.push_back(0);
                            container->mmtype_1.push_back(read[read_pos]);
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
        return ref_offset;
    }

    std::unique_ptr<StreamContainer> LocalAssemblyReadEncoder::pollStreams() {
        posCounter = 0;
        auto tmp = std::move(container);
        container = std::unique_ptr<StreamContainer>(new StreamContainer());
        return tmp;
    }
}