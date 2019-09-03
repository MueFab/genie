#ifndef GENIE_TRANSFORM_SUBSEQ_PARAMETERS_H
#define GENIE_TRANSFORM_SUBSEQ_PARAMETERS_H

#include <memory>
#include <vector>

class BitWriter;

/**
 * ISO 23092-2 Section 8.3.4 table
 */
class Transform_subseq_parameters {
public:

    enum class Transform_ID_subseq : uint8_t {
        no_transform = 0,
        equality_coding = 1,
        match_coding = 2,
        rle_coding = 3,
        merge_coding = 4
    };


    virtual void write(BitWriter *writer) const;

    Transform_subseq_parameters();

    Transform_subseq_parameters(const Transform_ID_subseq &_transform_ID_subseq, uint16_t param);

    std::unique_ptr<Transform_subseq_parameters> clone() const;
private:
    Transform_ID_subseq transform_ID_subseq; //!< : 8; Line 2
    std::unique_ptr<uint16_t> match_coding_buffer_size; //!< : 16; Line 6
    std::unique_ptr<uint8_t> rle_coding_guard; //!< : 8; Line 9
    std::unique_ptr<uint8_t> merge_coding_subseq_count; //!< : 4; Line 12
    std::vector<uint8_t> merge_coding_shift_size; //!< : 5; Line 15
};

#endif //GENIE_TRANSFORM_SUBSEQ_PARAMETERS_H
