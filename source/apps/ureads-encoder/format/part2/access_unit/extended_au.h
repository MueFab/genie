#ifndef GENIE_EXTENDED_AU_H
#define GENIE_EXTENDED_AU_H

// -----------------------------------------------------------------------------------------------------------------



// -----------------------------------------------------------------------------------------------------------------

namespace format {

/**
* ISO 23092-2 Section 3.4.1.1 table 19 lines 21 to 24
*/
    class ExtendedAu {
    private:
        uint64_t extended_AU_start_position; // TODO: Size //!< Line 22
        uint64_t extended_AU_end_position; // TODO: Size //!< Line 23

    public:
        ExtendedAu(uint64_t _extended_AU_start_position, uint64_t _extended_AU_end_position)
                : extended_AU_start_position(_extended_AU_start_position),
                  extended_AU_end_position(_extended_AU_end_position) {

        }

        virtual void write(BitWriter *writer);
    };
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_EXTENDED_AU_H