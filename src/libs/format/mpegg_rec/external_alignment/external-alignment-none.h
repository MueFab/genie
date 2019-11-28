//
// Created by fabian on 28.11.19.
//

#ifndef GENIE_EXTERNAL_ALIGNMENT_NONE_H
#define GENIE_EXTERNAL_ALIGNMENT_NONE_H


class ExternalAlignmentNone : public ExternalAlignment {
public:
    ExternalAlignmentNone() : ExternalAlignment(ExternalAlignment::MoreAlignmentInfoType::NONE) {

    }

    void write(util::BitWriter *writer) const override {

    }

    explicit ExternalAlignmentNone(util::BitReader *reader) : ExternalAlignment(ExternalAlignment::MoreAlignmentInfoType::NONE){

    }

};


#endif //GENIE_EXTERNAL_ALIGNMENT_NONE_H
