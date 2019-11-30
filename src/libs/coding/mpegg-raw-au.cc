#include "mpegg-raw-au.h"

#include <algorithm>

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