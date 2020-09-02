/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_WRITER_H_
#define GABAC_WRITER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <vector>

#include <genie/util/bitwriter.h>
#include "binary-arithmetic-encoder.h"
#include "streams.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 *
 */
class Writer {
   public:
    /**
     *
     * @param bitstream
     * @param bypassFlag
     * @param numContexts
     */
    explicit Writer(OBufferStream *bitstream, const bool bypassFlag = true, const unsigned long numContexts = 0);

    /**
     *
     */
    ~Writer();

    /**
     *
     */
    void start();

    /**
     *
     */
    void close();

    /**
     *
     */
    void reset();

    /**
     *
     * @param input
     * @param binParams
     */
    void writeAsBIbypass(uint64_t input, const std::vector<unsigned int> binParams);

    /**
     *
     * @param input
     * @param binParams
     */
    void writeAsBIcabac(uint64_t input, const std::vector<unsigned int> binParams);

    /**
     *
     * @param input
     * @param binParams
     */
    void writeAsTUbypass(uint64_t input, const std::vector<unsigned int> binParams);

    /**
     *
     * @param input
     * @param binParams
     */
    void writeAsTUcabac(uint64_t input, const std::vector<unsigned int> binParams);

    /**
     *
     * @param input
     * @param binParams
     */
    void writeAsEGbypass(uint64_t input, const std::vector<unsigned int> binParams);

    /**
     *
     * @param input
     * @param binParams
     */
    void writeAsEGcabac(uint64_t input, const std::vector<unsigned int> binParams);

    /**
     *
     * @param input
     * @param binParams
     */
    void writeAsTEGbypass(uint64_t input, const std::vector<unsigned int> binParams);

    /**
     *
     * @param input
     * @param binParams
     */
    void writeAsTEGcabac(uint64_t input, const std::vector<unsigned int> binParams);

    /**
     *
     * @param input
     * @param binParams
     */
    void writeAsSUTUbypass(uint64_t input, const std::vector<unsigned int> binParams);

    /**
     *
     * @param input
     * @param binParams
     */
    void writeAsSUTUcabac(uint64_t input, const std::vector<unsigned int> binParams);

    /**
     *
     * @param input
     * @param binParams
     */
    void writeAsDTUbypass(uint64_t input, const std::vector<unsigned int> binParams);

    /**
     *
     * @param input
     * @param binParams
     */
    void writeAsDTUcabac(uint64_t input, const std::vector<unsigned int> binParams);

    /**
     *
     * @param input
     * @param codingSubsymSize
     */
    void writeLutSymbol(uint64_t input, const uint8_t codingSubsymSize);

    /**
     *
     * @param input
     */
    void writeSignFlag(int64_t input);

   private:
    util::BitWriter m_bitOutputStream;  //!<

    BinaryArithmeticEncoder m_binaryArithmeticEncoder;  //!<

    bool m_bypassFlag;            //!<
    unsigned long m_numContexts;  //!<

    std::vector<ContextModel> m_contextModels;  //!<
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GABAC_WRITER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------