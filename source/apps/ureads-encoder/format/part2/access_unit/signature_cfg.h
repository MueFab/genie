#ifndef GENIE_SIGNATURE_CFG_H
#define GENIE_SIGNATURE_CFG_H

// -----------------------------------------------------------------------------------------------------------------



// -----------------------------------------------------------------------------------------------------------------

namespace format {

/**
* ISO 23092-2 Section 3.4.1.1 table 19 lines 26 to 39
*/
    class SignatureCfg {
    private:
        uint64_t U_cluster_signature_0; // TODO: Size //!< Line 26
        std::unique_ptr <uint16_t> num_signatures; //!< : 16; Line 33
        std::vector <uint64_t> U_cluster_signature; // TODO: Size //!< Lines 29 and 25 joined


        uint8_t U_signature_size; //!< Internal
    public:
        SignatureCfg(uint64_t _U_cluster_signature_0, uint8_t _U_signature_size)
                : U_cluster_signature_0(_U_cluster_signature_0), num_signatures(nullptr), U_cluster_signature(0),
                  U_signature_size(_U_signature_size) {
            if (U_cluster_signature[0] == (1 << U_signature_size) - 1) {
                num_signatures = make_unique<uint16_t>(1);
            }
        }

        void addSignature(uint64_t _U_cluster_signature) {
            U_cluster_signature.push_back(_U_cluster_signature);
            if (num_signatures) {
                ++*num_signatures;
            }
        }

        virtual void write(BitWriter *writer);
    };
}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_SIGNATURE_CFG_H
