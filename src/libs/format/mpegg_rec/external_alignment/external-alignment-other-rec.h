//
// Created by fabian on 28.11.19.
//

#ifndef GENIE_EXTERNAL_ALIGNMENT_OTHER_REC_H
#define GENIE_EXTERNAL_ALIGNMENT_OTHER_REC_H


class ExternalAlignmentOtherRec : public ExternalAlignment {
    uint64_t next_pos : 40;
    uint16_t next_seq_ID : 16;
public:
    ExternalAlignmentOtherRec(
            uint64_t _next_pos,
            uint16_t _next_seq_ID
    ) : ExternalAlignment(ExternalAlignment::MoreAlignmentInfoType::OTHER_REC),
        next_pos(_next_pos),
        next_seq_ID(_next_seq_ID) {

    }

    explicit ExternalAlignmentOtherRec(util::BitReader *reader) : ExternalAlignment(ExternalAlignment::MoreAlignmentInfoType::OTHER_REC){

    }

    void write(util::BitWriter *writer) const override {

    }
};


#endif //GENIE_EXTERNAL_ALIGNMENT_OTHER_REC_H
