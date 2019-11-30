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
        explicit SubDescriptor(size_t wordsize) : data(0, wordsize), position(0) {

        }
        explicit SubDescriptor(gabac::DataBlock* d) : data(0, 1), position(0) {
            d->swap(d);
        }
        void push(uint64_t val) {
            data.push_back(val);
        }
        void inc() {
            position++;
        }
        uint64_t get() const{
            return data.get(position);
        }

        bool end() const {
            return data.size() == position;
        }

        size_t rawSize () const{
            return data.getRawSize();
        }

        const void* getData () const {
            return data.getData();
        }
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

    SubDescriptor& get(GenomicDescriptor desc, size_t sub) {
        return descriptors[uint8_t(desc)][sub];
    }

    const SubDescriptor& get(GenomicDescriptor desc, size_t sub) const {
        return descriptors[uint8_t(desc)][sub];
    }

    static const Alphabet &getAlphabetProperties(format::mpegg_p2::ParameterSet::AlphabetID id);

    MpeggRawAu() : descriptors(NUM_DESCRIPTORS) {
        const size_t WORDSIZE = 4;
        size_t idx = 0;
        for(const auto &s : getDescriptorProperties()) {
            descriptors[idx].resize(s.number_subsequences, SubDescriptor(WORDSIZE));
            idx++;
        }
    }

private:
    std::vector<Descriptor> descriptors;
};


void write32bit(uint32_t
                val,
                std::vector<uint8_t> *ret
) {
    for (
            int i = sizeof(uint32_t) - 1;
            i >= 0; --i) {
        uint8_t writeVal = (val >> i * 8) & 0xff;
        ret->
                push_back(writeVal);
    }
}

void write16bit(uint16_t val, std::vector<uint8_t> *ret) {
    for (int i = sizeof(uint16_t) - 1; i >= 0; --i) {
        uint8_t writeVal = (val >> i * 8) & 0xff;
        ret->push_back(writeVal);
    }
}

void write8bit(uint8_t val, std::vector<uint8_t> *ret) {
    ret->push_back(val);
}

void writeu7v(uint32_t val, std::vector<uint8_t> *ret) {
    std::vector<uint8_t> tmp;
    do {
        tmp.push_back(val & 0x7f);
        val >>= 7;
    } while (val != 0);
    for (int i = tmp.size() - 1; i > 0; i--)
        ret->push_back(tmp[i] | 0x80);
    ret->push_back(tmp[0]);
}

void insert(gabac::DataBlock *transformed_subsequence, std::vector<uint8_t> *ret) {
    ret->insert(ret->end(), reinterpret_cast<const uint8_t *>(transformed_subsequence->getData()),
                reinterpret_cast<const uint8_t *>(transformed_subsequence->getData()) +
                transformed_subsequence->getRawSize());
}

void insert(std::vector<gabac::DataBlock> *subsequence, std::vector<uint8_t> *ret) {
    for (size_t i = 0; i < subsequence->size(); ++i) {
        std::vector<uint8_t> tmp;
        insert(&(*subsequence)[i], &tmp);
        if (i != subsequence->size() - 1) {
            write32bit(tmp.size(), ret);
        }

        // TODO: insert number of symbols in subsequence if more than one transformed subsequence
        ret->insert(ret->end(), tmp.begin(), tmp.end());
    }
}

void insert(std::vector<std::vector<gabac::DataBlock>> *descriptor, std::vector<uint8_t> *ret) {
    for (size_t i = 0; i < descriptor->size(); ++i) {
        std::vector<uint8_t> tmp;
        insert(&(*descriptor)[i], &tmp);
        if (i != descriptor->size() - 1) {
            write32bit(tmp.size(), ret);
        }
        ret->insert(ret->end(), tmp.begin(), tmp.end());
    }
}

std::vector<uint8_t> create_payload(std::vector<std::vector<gabac::DataBlock>> *block) {
    std::vector<uint8_t> ret;

    insert(block, &ret);
    return ret;
}

std::vector<uint8_t>
create_payload_tokentype(std::vector<std::vector<gabac::DataBlock>> *block, uint32_t records_count) {
    std::vector<uint8_t> ret;
    if (block->size() != 128 * 6)
        throw std::runtime_error("tokentype: number of tokens not 128*6 as expected.");
    write32bit(records_count, &ret);
    uint16_t num_tokentype_sequences = 0;
    // assumes standard id_tokenization used
    for (uint16_t i = 0; i < 128 * 6; i++) {
        if ((*block)[i].size() != 0)
            num_tokentype_sequences++;
    }
    write16bit(num_tokentype_sequences, &ret);
    for (uint16_t i = 0; i < 128 * 6; i++) {
        if ((*block)[i].size() == 0)
            continue;
        uint8_t type_id = i % 6;
        uint8_t method_id = 3; // CABAC_METHOD_0
        write8bit(16 * type_id + method_id, &ret); // 4 bits each

        // TODO: make sure this also work for (*block)[i].size() > 1

        // TODO: simplify this by having GABAC itself have option for u7(v) num_encoded_symbols

        std::vector<uint8_t> tmp;
        insert(&(*block)[i], &tmp);
        // now get num_output_symbols from tmp:
        uint32_t num_output_symbols;
        if (tmp.size() > 0)
            num_output_symbols = 256 * (256 * (256 * tmp[0] + tmp[1]) + tmp[2]) + tmp[3];
        else
            num_output_symbols = 0;
        writeu7v(num_output_symbols, &ret);
        auto start_it = tmp.begin();
        std::advance(start_it, 4);
        ret.insert(ret.end(), start_it, tmp.end());
    }
    return ret;
}

bool descriptorEmpty(const std::vector<std::vector<gabac::DataBlock>> &data) {
    for (auto &s : data) {
        if (!s.empty())
            return false;
    }
    return true;
}


#endif //GENIE_MPEGG_RAW_AU_H
