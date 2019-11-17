#include "bitreader.h"

namespace util {
BitReader::BitReader(std::istream *istream) : m_istream(istream), m_currentPositionInChar(8), m_currentChar(0) {}

bool BitReader::errorCheck() {
    if (!m_istream->good()) {
        fprintf(stderr, "Error in Utils::BitReader!\n");
        return false;
    }
    return true;
}

bool BitReader::readBit(uint8_t *value) {
    if (m_currentPositionInChar == 8) {
        uint8_t fileReadValue = m_istream->get();
        if (!errorCheck()) return false;
        m_currentPositionInChar = 0;
        m_currentChar = fileReadValue;
    }

    uint8_t mask = 1;
    mask <<= (7 - m_currentPositionInChar);
    *value = m_currentChar & mask;
    *value = *value != 0;

    m_currentPositionInChar++;
    return true;
}

bool BitReader::readNBits(uint32_t n, char *value) {
    uint8_t bufferValue = 0;

    uint32_t current_bit = 0;
    uint32_t current_byte = 0;

    for (uint32_t i = 0; i < n; i++) {
        if (!this->readBit(&bufferValue)) {
            return false;
        }

        if (bufferValue != 0) {
            value[current_byte] |= ((uint8_t)1) << (7 - current_bit);
        } else {
            value[current_byte] &= ~(((uint8_t)1) << (7 - current_bit));
        }
        current_bit++;
        if (current_bit == 8) {
            current_byte++;
            current_bit = 0;
        }
    }
    return true;
}
}  // namespace util