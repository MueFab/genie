//
// Created by fabian on 28.11.19.
//

#ifndef GENIE_META_ALIGNMENT_H
#define GENIE_META_ALIGNMENT_H


class MetaAlignment {
    uint16_t seq_ID : 16;
    uint8_t as_depth : 8;
public:
    MetaAlignment(
            uint16_t _seq_ID,
            uint8_t _as_depth
    ) : seq_ID(_seq_ID),
        as_depth(_as_depth) {
    }

    virtual void write(util::BitWriter *write) const {

    }

    explicit MetaAlignment(util::BitReader *reader) {

    }

    uint16_t getSeqID() const {
        return seq_ID;
    }
    uint8_t getAsDepth () const {
        return as_depth;
    }
};


#endif //GENIE_META_ALIGNMENT_H
