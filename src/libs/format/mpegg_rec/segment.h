//
// Created by fabian on 28.11.19.
//

#ifndef GENIE_SEGMENT_H
#define GENIE_SEGMENT_H


class Segment {
    std::unique_ptr<std::string> sequence;
    // for (qs=0; qs < qv_depth; qs++)
    std::vector<std::unique_ptr<std::string>> quality_values; // or c(1), as specified in subclause 9.2.15
public:
    explicit Segment(std::unique_ptr<std::string> _sequence) : sequence(std::move(_sequence)), quality_values() {

    }

    explicit Segment(uint32_t length, util::BitReader* reader) {

    }

    size_t getQvDepth() const {
        return quality_values.size();
    }

    size_t getLength() const {
        return sequence->length();
    }

    void addQualityValues(std::unique_ptr<std::string> qv) {
        if (qv->length() != sequence->length()) {
            UTILS_DIE("QV and sequence lengths do not match");
        }
        quality_values.push_back(std::move(qv));
    }
    virtual void write(util::BitWriter *write) const {

    }

};


#endif //GENIE_SEGMENT_H
