#ifndef GENIE_MPEGG_RAW_AU_H
#define GENIE_MPEGG_RAW_AU_H

#include <cstdint>
#include <string>
#include <vector>
#include <format/mpegg_p2/parameter_set.h>


class MpeggRawAu {
public:
    struct SubDescriptor {
    private:
        gabac::DataBlock data;
        size_t position;
    public:
        explicit SubDescriptor(size_t wordsize);
        explicit SubDescriptor(gabac::DataBlock* d);
        void push(uint64_t val);

        void inc();

        uint64_t get() const;

        bool end() const;

        size_t rawSize () const;

        size_t getWordSize() const;

        const void* getData () const;
    };
    typedef std::vector<SubDescriptor> Descriptor;

    enum class GenomicDescriptor : uint8_t {
        POS = 0,
        RCOMP = 1,
        FLAGS = 2,
        MMPOS = 3,
        MMTYPE = 4,
        CLIPS = 5,
        UREADS = 6,
        RLEN = 7,
        PAIR = 8,
        MSCORE = 9,
        MMAP = 10,
        MSAR = 11,
        RTYPE = 12,
        RGROUP = 13,
        QV = 14,
        RNAME = 15,
        RFTP = 16,
        RFTT = 17
    };

    struct GenomicDescriptorProperties {
        std::string name;
        uint8_t number_subsequences;
    };
    static constexpr size_t NUM_DESCRIPTORS = 18;

    struct Alphabet {
        std::vector<char> lut;
        std::vector<char> inverseLut;
    };

    static const std::vector<GenomicDescriptorProperties> &getDescriptorProperties();

    SubDescriptor& get(GenomicDescriptor desc, size_t sub);

    const SubDescriptor& get(GenomicDescriptor desc, size_t sub) const;

    static const Alphabet &getAlphabetProperties(format::mpegg_p2::ParameterSet::AlphabetID id);

    MpeggRawAu();

    void setParameters(std::unique_ptr<format::mpegg_p2::ParameterSet> _parameters) {
        parameters = std::move(_parameters);
    }

    format::mpegg_p2::ParameterSet* getParameters() {
        return parameters.get();
    }

    std::unique_ptr<format::mpegg_p2::ParameterSet> moveParameters() {
        return std::move(parameters);
    }

    const format::mpegg_p2::ParameterSet* getParameters() const {
        return parameters.get();
    }

private:
    std::vector<Descriptor> descriptors;
    std::unique_ptr<format::mpegg_p2::ParameterSet> parameters;
};

#endif //GENIE_MPEGG_RAW_AU_H
