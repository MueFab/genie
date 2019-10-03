#include "full-local-assembly-decoder.h"

#include <iostream>

namespace lae {
    FullLocalAssemblyDecoder::FullLocalAssemblyDecoder(
            std::unique_ptr<StreamContainer>
            container,
            uint32_t _cr_buf_size,
            bool _debug) : abs_pos(0),
                           state(nullptr),
                           lrd(std::move(container)),
                           debug(_debug) {
        const uint32_t CAPACITY = 16;
        local_assembly_state_create(&state);
        local_assembly_state_init(state, _cr_buf_size, CAPACITY);
    }

    FullLocalAssemblyDecoder::~FullLocalAssemblyDecoder() {
        local_assembly_state_destroy(state);
    }

    void FullLocalAssemblyDecoder::decode(util::SamRecord *s) {
        abs_pos += lrd.offsetOfNextRead();

        char *refbuf;
        local_assembly_state_get_ref(state, abs_pos, lrd.lengthOfNextRead(), &refbuf);
        std::string ref = std::string(refbuf, refbuf + lrd.lengthOfNextRead());
        free(refbuf);

        lrd.decodeRead(ref, s);

        local_assembly_state_add_read(state, s->seq.c_str(), s->cigar.c_str(), s->pos);

        if (debug) {
            std::cout << "R: " << s->seq << std::endl;
            std::cout << "C: " << s->cigar << std::endl;
            std::cout << "P: " << s->pos << std::endl << std::endl;
        }
    }

    void FullLocalAssemblyDecoder::decodePair(util::SamRecord *s,
                                              util::SamRecord *s2) {
        abs_pos += lrd.offsetOfNextRead();

        char *refbuf;
        local_assembly_state_get_ref(state, abs_pos, lrd.lengthOfNextRead(), &refbuf);
        std::string ref1 = std::string(refbuf, refbuf + lrd.lengthOfNextRead());
        free(refbuf);

        abs_pos += lrd.offsetOfSecondNextRead();
        local_assembly_state_get_ref(state, abs_pos, lrd.lengthOfSecondNextRead(), &refbuf);
        std::string ref2 = std::string(refbuf, refbuf + lrd.lengthOfSecondNextRead());
        free(refbuf);

        uint32_t delta;
        bool first;
        lrd.decodePair(ref1, s, ref2, s2, &delta, &first);
        local_assembly_state_add_read(state, s->seq.c_str(), s->cigar.c_str(), s->pos);
        local_assembly_state_add_read(state, s2->seq.c_str(), s2->cigar.c_str(), s2->pos);
        if (debug) {
            std::cout << "Decoded pair! First1: " << first << " Delta: " << delta << std::endl;
            std::cout << "R: " << s->seq << std::endl;
            std::cout << "C: " << s->cigar << std::endl;
            std::cout << "P: " << s->pos << std::endl << std::endl;
            std::cout << "R: " << s2->seq << std::endl;
            std::cout << "C: " << s2->cigar << std::endl;
            std::cout << "P: " << s2->pos << std::endl << std::endl;
        }
    }
}