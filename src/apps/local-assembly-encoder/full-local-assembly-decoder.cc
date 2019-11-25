#include "full-local-assembly-decoder.h"

#include <iostream>

/*
namespace lae {
    FullLocalAssemblyDecoder::FullLocalAssemblyDecoder(
            std::unique_ptr<StreamContainer>
            container,
            uint32_t cr_buf_max_size,
            bool _debug) : mapping_pos(0),
                           state(nullptr),
                           lrd(std::move(container)),
                           debug(_debug) {
        init(cr_buf_max_size);
    }

    FullLocalAssemblyDecoder::~FullLocalAssemblyDecoder() {
        local_assembly_state_destroy(state);
    }

    // !!! This is the function that we need to mimic in the MPEG-G reference software. !!!
    void FullLocalAssemblyDecoder::init(const uint32_t cr_buf_max_size) {
        const uint32_t CAPACITY = 16; // Starting capacity for memory management
        local_assembly_state_create(&state);
        local_assembly_state_init(state, cr_buf_max_size, CAPACITY);
    }

    // !!! This is the function that we need to mimic in the MPEG-G reference software. !!!
    void FullLocalAssemblyDecoder::decode(util::SamRecord *mpegg_record) {
        // We first need to decode the (absolute) mapping position from pos and the read length from rlen
        mapping_pos += lrd.offsetOfNextRead();
        uint32_t read_len = lrd.lengthOfNextRead();

        char *refBuf;
        local_assembly_state_get_ref(state, mapping_pos, read_len, &refBuf);

        // Usually, when decoding the first read, refBuf returned by local_assembly_state_get_ref() is empty. Empty
        // in our implementation means, that it contains an arbitrary number of '\0' bytes. This however does not
        // matter because refBuf is not needed for descriptor decoding in this case.

        std::string ref_sequence = std::string(refBuf, refBuf + read_len); // ref_sequence[0]
        free(refBuf);

        lrd.decodeRead(ref_sequence, mpegg_record);

        // seq=sequence[0], cigar=ecigar_string[0][0], pos=mapping_pos[0]
        local_assembly_state_add_read(state, mpegg_record->seq.c_str(), mpegg_record->cigar.c_str(), mpegg_record->pos);

        if (debug) {
            std::cout << "sequence[0]         : " << mpegg_record->seq << std::endl;
            std::cout << "ecigar_string[0][0] : " << mpegg_record->cigar << std::endl;
            std::cout << "mapping_pos[0]      : " << mpegg_record->pos << std::endl << std::endl;
        }
    }

    // !!! This is the function that we need to mimic in the MPEG-G reference software. !!!
    void FullLocalAssemblyDecoder::decodePair(util::SamRecord *mpegg_record_segment_1,
                                              util::SamRecord *mpegg_record_segment_2) {
        mapping_pos += lrd.offsetOfNextRead();
        uint32_t read_len = lrd.lengthOfNextRead();

        char *refBuf;

        local_assembly_state_get_ref(state, mapping_pos, read_len, &refBuf);
        std::string ref1 = std::string(refBuf, refBuf + read_len);
        free(refBuf);

        mapping_pos += lrd.offsetOfSecondNextRead();
        read_len = lrd.lengthOfSecondNextRead();

        local_assembly_state_get_ref(state, mapping_pos, read_len, &refBuf);
        std::string ref2 = std::string(refBuf, refBuf + read_len);
        free(refBuf);

        // TODO: ref1 and ref2 need to be concatenated to form ref_sequence
        lrd.decodePair(ref1, mpegg_record_segment_1, ref2, mpegg_record_segment_2);

        local_assembly_state_add_read(state, mpegg_record_segment_1->seq.c_str(), mpegg_record_segment_1->cigar.c_str(), mpegg_record_segment_1->pos);
        local_assembly_state_add_read(state, mpegg_record_segment_2->seq.c_str(), mpegg_record_segment_2->cigar.c_str(), mpegg_record_segment_2->pos);

        if (debug) {
            std::cout << "sequence[0]                  : " << mpegg_record_segment_1->seq << std::endl;
            std::cout << "ecigar_string[0][0]          : " << mpegg_record_segment_1->cigar << std::endl;
            std::cout << "mapping_pos[0]               : " << mpegg_record_segment_1->pos << std::endl << std::endl;
            std::cout << "sequence[1]                  : " << mpegg_record_segment_2->seq << std::endl;
            std::cout << "ecigar_string[0][1]          : " << mpegg_record_segment_2->cigar << std::endl;
            std::cout << "mapping_pos[0] + delta[0][1] : " << mpegg_record_segment_2->pos << std::endl << std::endl;
        }
    }
} */