/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_READ_BASECODER_ENCODERSTUB_H_
#define SRC_GENIE_READ_BASECODER_ENCODERSTUB_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include <utility>
#include "genie/core/read_encoder.h"
#include "genie/read/basecoder/encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace basecoder {

/**
 * @brief Common stub for all aligned record encoders
 */
class EncoderStub : public core::ReadEncoder {
 protected:
    struct EncodingState {
        /**
         * @brief Initialize from chunk of records
         * @param data Records
         */
        explicit EncodingState(const core::record::Chunk& data);

        basecoder::Encoder readCoder;       //!< @brief Generating the descriptor streams
        bool pairedEnd;                     //!< @brief Current guess regarding pairing
        size_t readLength;                  //!< @brief Current guess regarding read length
        core::record::ClassType classType;  //!< @brief Current guess regarding class type
        uint64_t minPos;                    //!< @brief Starting position of chunk
        uint64_t maxPos;                    //!< @brief Ending position of chunk
        uint16_t ref;                       //!< @brief Reference sequence
        uint64_t readNum;                   //!< @brief Number of segments
        uint64_t lastReadPosition;          //!< @brief Last mapping position

        /**
         * @brief Virtual destructor
         */
        virtual ~EncodingState() = default;
    };

    /**
     * @brief Get the reference sequences used to encode a record
     * @param r Record
     * @param state Current state
     * @return Pair of references. If unpaired, second reference will be empty
     */
    virtual std::pair<std::string, std::string> getReferences(const core::record::Record& r, EncodingState& state) = 0;

    /**
     * @brief Create the specialized state for this encoder
     * @param data Chunk of data to initalize from
     * @return Allocated state
     */
    virtual std::unique_ptr<EncodingState> createState(const core::record::Chunk& data) const = 0;

    /**
     * @brief Encode the base sequences of all record in a chunk
     * @param data Chunk of records
     * @param state State to encode with
     */
    void encodeSeq(core::record::Chunk& data, EncodingState& state);

    /**
     * @brief Pack encoded data into a reference unit
     * @param id Multithreading sync ID
     * @param qv Encoded quality values
     * @param rname Encoded read names
     * @param state Encoding state
     * @return Assembled access unit with parameters
     */
    virtual core::AccessUnit pack(size_t id, core::QvEncoder::qv_coded qv, core::AccessUnit::Descriptor rname,
                                  EncodingState& state);

    /**
     * @brief Remove all descriptors redundant form the current class.
     * @param rawAU AU to modify
     */
    static void removeRedundantDescriptors(core::AccessUnit& rawAU);

    /**
     * @brief Encode all quality values in a chunk of data
     * @param qvcoder QV encoder
     * @param data Chunk of data
     * @return Encoded quality values
     */
    static core::QvEncoder::qv_coded encodeQVs(qv_selector* qvcoder, core::record::Chunk& data);

    /**
     * @brief Encode read names in a chunk of data
     * @param namecoder Name encoder
     * @param data Chunk to encode
     * @return Encoded read names
     */
    static core::AccessUnit::Descriptor encodeNames(name_selector* namecoder, core::record::Chunk& data);

 public:
    /**
     * @brief
     * @param _write_raw
     */
    explicit EncoderStub(bool _write_raw) : core::ReadEncoder(_write_raw) {}

    ~EncoderStub() override = default;

    /**
     * @brief Process data
     * @param t Incoming chunk of data
     * @param id Multithreading sync ID
     */
    void FlowIn(core::record::Chunk&& t, const util::Section& id) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace basecoder
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_BASECODER_ENCODERSTUB_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
