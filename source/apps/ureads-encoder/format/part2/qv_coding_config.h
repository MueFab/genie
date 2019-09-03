#ifndef GENIE_QV_CODING_CONFIG_H
#define GENIE_QV_CODING_CONFIG_H

#include <cstdint>

class BitWriter;

/**
* ISO 23092-2 Section 3.3.2 table 7 lines 32 to 40
*/
class Qv_coding_config {
public:

    /**
     * Inserted for convenience, not named in ISO 23092-2
     */
    enum class Qv_coding_mode : uint8_t {
        ONE = 1
    };

    virtual void write(BitWriter *writer) const = 0;

    virtual ~Qv_coding_config() = default;

    Qv_coding_config(
            Qv_coding_mode _qv_coding_mode,
            bool _qv_reverse_flag
    );

protected:
    Qv_coding_mode qv_coding_mode; //!< : 4; Line 32
    bool qv_reverse_flag : 1; //!< Line 40
};


#endif //GENIE_QV_CODING_CONFIG_H
