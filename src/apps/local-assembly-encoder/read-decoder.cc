#include "read-decoder.h"

namespace lae {
    void LocalAssemblyReadDecoder::decodeVariants(std::string *read, std::string *cigar) {
        size_t totalPos = 0;
        while (!pop(&container->mmpos_0)) {
            totalPos += pop(&container->mmpos_1);
            uint64_t type = pop(&container->mmtype_0);
            switch (type) {
                case 0:
                    (*read)[totalPos] = pop(&container->mmtype_1);
                    totalPos += 1;
                    break;
                case 1:
                    read->insert(totalPos, 1, (char) pop(&container->mmtype_2));
                    cigar->insert(totalPos, 1, 'I');
                    totalPos += 1;
                    break;
                case 2:
                    read->erase(totalPos, 1);
                    cigar->erase(cigar->length() - 1, 1);
                    cigar->insert(totalPos, 1, 'D');
                    break;
            }
        }
    }

    uint64_t LocalAssemblyReadDecoder::pop(gabac::DataBlock *vec) {
        uint64_t ret = vec->get(0);
        auto tmp = gabac::DataBlock(((uint8_t *)vec->getData()) + vec->getWordSize() , vec->size() - 1, vec->getWordSize());
        vec->swap(&tmp);
        return ret;
    }

    uint32_t LocalAssemblyReadDecoder::lengthOfNextRead() {
        return container->rlen_0.get(0);
    }

    uint32_t LocalAssemblyReadDecoder::offsetOfNextRead() {
        return container->pos_0.get(0);
    }

    uint32_t LocalAssemblyReadDecoder::lengthOfSecondNextRead() {
        return container->rlen_0.get(1);
    }

    uint32_t LocalAssemblyReadDecoder::offsetOfSecondNextRead() {
        return container->pos_0.get(1);
    }

    void LocalAssemblyReadDecoder::decodeRead(const std::string& ref, util::SamRecord* s) {
        bool usedClip = false;

        pop(&container->rlen_0);
        posCounter += pop(&container->pos_0);
        s->pos = posCounter;
        s->seq = ref;
        s->cigar = std::string(s->seq.length(), 'M');
        decodeVariants(&s->seq, &s->cigar);
        s->cigar = compressCigar(s->cigar);
        for(auto &c : s->seq) {
            if(c == 0){
                usedClip = true;
                c = pop(&container->clips_2);
            }
        }
        if(usedClip) {
            pop(&container->clips_0);
            pop(&container->clips_1);
            pop(&container->clips_1);
            pop(&container->clips_2);
        }

        pop(&container->flags_0);
        pop(&container->flags_1);
        pop(&container->flags_2);
        pop(&container->mscore_0);
        pop(&container->rcomp_0);
        pop(&container->rtype_0);
    }

    void LocalAssemblyReadDecoder::decodePair(const std::string& ref1, util::SamRecord* s1, const std::string& ref2, util::SamRecord* s2) {
        this->decodeRead(ref1, s1);
        this->decodeRead(ref2, s2);
        pop(&container->pair_0);
        /*delta = */pop(&container->pair_1);
//        *first1 = *delta & 0x1;
//        *delta >>= 1;
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