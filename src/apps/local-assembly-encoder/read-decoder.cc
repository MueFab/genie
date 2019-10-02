#include "read-decoder.h"

namespace lae {
    void LocalAssemblyReadDecoder::decodeVariants(std::string *read, std::string *cigar) {
        while (!pop(&container->mmpos_0)) {
            uint64_t pos = pop(&container->mmpos_1);
            uint64_t type = pop(&container->mmtype_0);
            switch (type) {
                case 0:
                    (*read)[pos] = pop(&container->mmtype_1);
                    break;
                case 1:
                    read->insert(pos, 1, (char) pop(&container->mmtype_2));
                    cigar->insert(pos, 1, 'I');
                    break;
                case 2:
                    read->erase(pos, 1);
                    cigar->erase(cigar->length() - 1, 1);
                    cigar->insert(pos, 1, 'D');
                    break;
            }
        }
    }

    uint64_t LocalAssemblyReadDecoder::pop(std::vector<uint64_t> *vec) {
        uint64_t ret = vec->front();
        vec->erase(vec->begin());
        return ret;
    }

    uint32_t LocalAssemblyReadDecoder::lengthOfNextRead() {
        return container->rlen_0.front();
    }

    uint32_t LocalAssemblyReadDecoder::offsetOfNextRead() {
        return container->pos_0.front();
    }

    void LocalAssemblyReadDecoder::decodeRead(const std::string& ref, util::SamRecord* s) {
        pop(&container->rlen_0);
        posCounter += pop(&container->pos_0);
        s->pos = posCounter;
        s->seq = ref;
        s->cigar = std::string(s->seq.length(), 'M');
        decodeVariants(&s->seq, &s->cigar);
        s->cigar = compressCigar(s->cigar);
        for(auto &c : s->seq) {
            if(c == 0){
                c = pop(&container->clips_0);
            }
        }
    }

    std::string LocalAssemblyReadDecoder::compressCigar(const std::string &cigar) {
        char currentChar = 0;
        size_t counter = 0;
        std::string ret;
        for (char c : cigar) {
            if (c != currentChar) {
                if (currentChar != 0) {
                    ret += std::to_string(counter);
                    ret += currentChar;
                }
                currentChar = c;
                counter = 1;
            } else {
                ++counter;
            }
        }
        ret += std::to_string(counter);
        ret += currentChar;
        return ret;
    }

    LocalAssemblyReadDecoder::LocalAssemblyReadDecoder(
            std::unique_ptr<StreamContainer> _container
    ) : posCounter(0),
        container(std::move(_container)) {

    }

}