//
// Created by fabian on 28.11.19.
//

#ifndef GENIE_SPLIT_ALIGNMENT_H
#define GENIE_SPLIT_ALIGNMENT_H


class SplitAlignment {
public:
    enum class SplitAlignmentType : uint8_t {
        SAME_REC = 0,
        OTHER_REC = 1,
        UNPAIRED = 2
    };

    explicit SplitAlignment(SplitAlignmentType _split_alignment) : split_alignment(_split_alignment) {

    }

    virtual ~SplitAlignment() = default;

private:
    SplitAlignmentType split_alignment : 8;

};


#endif //GENIE_SPLIT_ALIGNMENT_H
