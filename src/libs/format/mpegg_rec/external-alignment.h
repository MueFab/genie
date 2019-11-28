//
// Created by fabian on 28.11.19.
//

#ifndef GENIE_EXTERNAL_ALIGNMENT_H
#define GENIE_EXTERNAL_ALIGNMENT_H


class ExternalAlignment {
public:
    enum class MoreAlignmentInfoType : uint8_t {
        NONE,
        OTHER_REC
    };

    explicit ExternalAlignment(MoreAlignmentInfoType _moreAlignmentInfoType) : moreAlignmentInfoType(
            _moreAlignmentInfoType) {

    }

    virtual ~ExternalAlignment() = default;

    virtual void write(util::BitWriter *writer) const {

    }

    static std::unique_ptr<ExternalAlignment> factory(util::BitReader *reader);
protected:
    MoreAlignmentInfoType moreAlignmentInfoType;
};


#endif //GENIE_EXTERNAL_ALIGNMENT_H
