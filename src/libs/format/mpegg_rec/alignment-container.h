//
// Created by fabian on 28.11.19.
//

#ifndef GENIE_ALIGNMENT_CONTAINER_H
#define GENIE_ALIGNMENT_CONTAINER_H


class AlignmentContainer {
    uint64_t mapping_pos : 40;
    std::unique_ptr<Alignment> alignment;

    // if (class_ID != Class_HM)
    // for (tSeg=1; tSeg < number_of_template_segments; tSeg++)
    std::vector<std::unique_ptr<SplitAlignment>> splitAlignmentInfo;

public:
    virtual void write(util::BitWriter *writer) const {

    }

    explicit AlignmentContainer(util::BitReader *reader) {

    }

    uint32_t getAsDepth() const {
        return alignment->getAsDepth();
    }
};


#endif //GENIE_ALIGNMENT_CONTAINER_H
