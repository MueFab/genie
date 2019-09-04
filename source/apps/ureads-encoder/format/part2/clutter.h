#ifndef GENIE_CLUTTER_H
#define GENIE_CLUTTER_H

#include <cstdint>
#include "parameter_set.h"

namespace format {

/* ----------------------------------------------------------------------------------------------------------- */

    struct CrInfo {
        /*  uint8_t cr_pad_size : 8;
          uint32_t cr_buf_max_size : 24;*/

        virtual void write(BitWriter &writer);
    };

/* ----------------------------------------------------------------------------------------------------------- */

    class QvCodebook {
        /*
    private:
        uint8_t qv_num_codebook_entries : 8;
        std::vector<uint8_t> qv_recon;

    public:
        QvCodebook();

        void addEntry(uint8_t entry);

        virtual void write(BitWriter *writer) const;*/
    };

/* ----------------------------------------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------------------------------------- */

/*class DecoderConfigurationCabacTokentype : public DecoderConfigurationCabac {
private:
    TransformSubseqParameters transform_subseq_parameters;
    std::vector<TransformSubseqCfg> transformSubseq_cfg;

public:
    void addSubsequenceCfg(const DescriptorSubsequenceCfg &cfg) override {
        (void) cfg;
    }

    std::unique_ptr<DecoderConfiguration> clone() const override {
        return make_unique<Decoder_configuration_cabac_tokentype>();
    }

    void write(BitWriter *writer) const override;
};*/

/* ----------------------------------------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------------------------------------------- */

    enum class GenomicDescriptor : uint8_t {
        pos = 0,
        rcomp = 1,
        flags = 2,
        mmpos = 3,
        mmtype = 4,
        clips = 5,
        ureads = 6,
        rlen = 7,
        pair = 8,
        mscore = 9,
        mmap = 10,
        msar = 11,
        rtype = 12,
        rgroup = 13,
        qv = 14,
        rname = 15,
        rftp = 16,
        rftt = 17
    };

    struct GenomicDescriptorProperties {
        std::string name;
        uint8_t number_subsequences;
    };
    constexpr size_t NUM_DESCRIPTORS = 18;

    const std::array<GenomicDescriptorProperties, NUM_DESCRIPTORS> &getDescriptorProperties();

/* ----------------------------------------------------------------------------------------------------------- */

    ParameterSet createQuickParameterSet(uint8_t _parameter_set_id,
                                         uint8_t _read_length,
                                         bool paired_end,
                                         bool qv_values_present,
                                         const std::vector<std::vector<gabac::EncodingConfiguration>> &parameters
    );

}

// -----------------------------------------------------------------------------------------------------------------

#endif //GENIE_CLUTTER_H
