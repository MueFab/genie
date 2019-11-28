//
// Created by fabian on 28.11.19.
//

#ifndef GENIE_ALIGNMENT_H
#define GENIE_ALIGNMENT_H


class Alignment {
    uint32_t ecigar_len : 24;
    std::unique_ptr<std::string> ecigar_string;
    uint8_t reverse_comp : 8;

    // for (as=0; as < as_depth; as++)
    std::vector<int32_t> mapping_score;

public:
    Alignment(std::unique_ptr<std::string> _ecigar_string,
              uint8_t _reverse_comp
    ) : ecigar_len(_ecigar_string->length()),
        ecigar_string(std::move(_ecigar_string)),
        reverse_comp(_reverse_comp),
        mapping_score() {

    }

    size_t getAsDepth() const {
        return mapping_score.size();
    }

    void addMappingScore(int32_t score) {
        mapping_score.push_back(score);
    }
};


#endif //GENIE_ALIGNMENT_H
