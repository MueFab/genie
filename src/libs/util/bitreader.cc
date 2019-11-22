#include "bitreader.h"

namespace util {
BitReader::BitReader(std::istream *istream) : m_istream(istream), m_currentPositionInChar(8), m_currentChar(0) {}

bool BitReader::errorCheck() {
    if (!m_istream->good()) {
        fprintf(stdout, "Error in Utils::BitReader!\n");
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

    for (uint32_t i = 0; i < n; i++) {
        if (!this->readBit(&bufferValue)) {
            return false;
        }

        if (bufferValue != 0) {
            value[current_bit] = '1';
        } else {
            value[current_bit] = '0';
        }
        current_bit++;
    }
    return true;
}

bool BitReader::readNBits(uint32_t n, uint32_t *value) {
    char *buffer = (char *)malloc(sizeof(unsigned char) * (n + 1));
    if (!readNBits(n, buffer)) return false;
    buffer[n] = '\0';
    std::string str(buffer);
    *value = std::stoul(str, nullptr, 10);
    free(buffer);
    return true;
}

bool BitReader::readNBitsDec(uint32_t n, uint32_t *value) {
    char *buffer = (char *)malloc(sizeof(unsigned char) * (n + 1));
    if (!readNBits(n, buffer)) return false;
    buffer[n] = '\0';
    std::string str(buffer);
    *value = std::stoul(str, nullptr, 2);
    free(buffer);
    return true;
}

bool BitReader::skipNBits(uint32_t bitsToSkip) {
    uint8_t value;
    for (uint32_t i = 0; i < bitsToSkip; ++i) {
        if (!this->readBit(&value)) return false;
    }
    return true;
}

bool BitReader::isGood() {
    m_istream->peek();  // check for eof
    if (!m_istream->good()) {
        return false;
    }
    return true;
}
}  // namespace util