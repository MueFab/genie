#include "mpegg-raw-au.h"

#include <algorithm>
#include <util/make_unique.h>

const std::vector<MpeggRawAu::GenomicDescriptorProperties> &MpeggRawAu::getDescriptorProperties() {
    static const auto prop = []() -> std::vector<GenomicDescriptorProperties> {
        std::vector<GenomicDescriptorProperties> loc;
        loc.push_back({"pos", 2});
        loc.push_back({"rcomp", 1});
        loc.push_back({"flags", 3});
        loc.push_back({"mmpos", 2});
        loc.push_back({"mmtype", 3});
        loc.push_back({"clips", 4});
        loc.push_back({"ureads", 1});
        loc.push_back({"rlen", 1});
        loc.push_back({"pair", 8});
        loc.push_back({"mscore", 1});
        loc.push_back({"mmap", 5});
        loc.push_back({"msar", 2});
        loc.push_back({"rtype", 1});
        loc.push_back({"rgroup", 1});
        loc.push_back({"qv", 3});
        loc.push_back({"rname", 2});
        loc.push_back({"rftp", 1});
        loc.push_back({"rftt", 1});
        return loc;
    }();
    return prop;
}

const MpeggRawAu::Alphabet &MpeggRawAu::getAlphabetProperties(format::mpegg_p2::ParameterSet::AlphabetID id) {
    static const auto prop = []() -> std::vector<Alphabet> {
        std::vector<Alphabet> loc;
        loc.emplace_back();
        loc.back().lut = {'A', 'C', 'G', 'T', 'N'};
        loc.emplace_back();
        loc.back().lut = {'A', 'C', 'G', 'T', 'R', 'Y', 'S', 'W', 'K', 'M', 'B', 'D', 'H', 'V', 'N', '-'};
        for (auto &l : loc) {
            l.inverseLut = std::vector<char>(*std::max_element(l.lut.begin(), l.lut.end()) + 1, 0);
            for (size_t i = 0; i < l.lut.size(); ++i) {
                l.inverseLut[l.lut[i]] = i;
            }
        }
        return loc;
    }();
    return prop[uint8_t(id)];
}


MpeggRawAu::SubDescriptor::SubDescriptor(size_t wordsize) : data(0, wordsize), position(0) {

}

MpeggRawAu::SubDescriptor::SubDescriptor(gabac::DataBlock *d) : data(0, 1), position(0) {
    d->swap(d);
}

void MpeggRawAu::SubDescriptor::push(uint64_t val) {
    data.push_back(val);
}

void MpeggRawAu::SubDescriptor::inc() {
    position++;
}

uint64_t MpeggRawAu::SubDescriptor::get() const {
    return data.get(position);
}

bool MpeggRawAu::SubDescriptor::end() const {
    return data.size() == position;
}

size_t MpeggRawAu::SubDescriptor::rawSize() const {
    return data.getRawSize();
}

size_t MpeggRawAu::SubDescriptor::getWordSize() const {
    return data.getWordSize();
}

const void *MpeggRawAu::SubDescriptor::getData() const {
    return data.getData();
}

MpeggRawAu::SubDescriptor &MpeggRawAu::get(GenomicDescriptor desc, size_t sub) {
    return descriptors[uint8_t(desc)][sub];
}

const MpeggRawAu::SubDescriptor &MpeggRawAu::get(GenomicDescriptor desc, size_t sub) const {
    return descriptors[uint8_t(desc)][sub];
}


MpeggRawAu::MpeggRawAu() : descriptors(NUM_DESCRIPTORS) {
    const size_t WORDSIZE = 4;
    size_t idx = 0;
    for (const auto &s : getDescriptorProperties()) {
        descriptors[idx].resize(s.number_subsequences, SubDescriptor(WORDSIZE));
        idx++;
    }
}

