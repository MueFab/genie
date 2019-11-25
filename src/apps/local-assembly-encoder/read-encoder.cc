#include <array>

#include <format/part2/clutter.h>
#include "read-encoder.h"
#include "util/exceptions.h"

namespace lae {
    LocalAssemblyReadEncoder::LocalAssemblyReadEncoder() : container(new StreamContainer()), pos(0), readCounter(0) {

    }

    void LocalAssemblyReadEncoder::addSingleRead(const util::SamRecord &rec, format::DataUnit::AuType type) {
        container->rlen_0.push_back(rec.seq.length() - 1);

        if(type > format::DataUnit::AuType::P_TYPE_AU) {
            container->mmpos_0.push_back(1);
        }
        container->rcomp_0.push_back((rec.flag >> 4) & 0x1);
        container->flags_0.push_back((rec.flag >> 10) & 0x1);
        container->flags_1.push_back((rec.flag >> 9) & 0x1);
        container->flags_2.push_back((rec.flag >> 1) & 0x1);

        container->mscore_0.push_back(rec.mapq);

        readCounter++;
    }

    format::DataUnit::AuType LocalAssemblyReadEncoder::addRead(const util::SamRecord &rec, const std::string &ref) {
        auto t = getClass(rec.seq, rec.cigar, ref);

        container->rtype_0.push_back(uint8_t(t));
        container->pos_0.push_back(rec.pos - pos);
        pos = rec.pos;

        codeVariants(rec.seq, rec.cigar, ref, t, true);
        addSingleRead(rec, t);

        if (!container->clips_0.empty() && container->clips_0.get(container->clips_0.size() - 1) == readCounter) {
            container->clips_1.push_back(8);
        }

        return t;
    }

    format::DataUnit::AuType LocalAssemblyReadEncoder::addPair(const util::SamRecord &rec1, const std::string &ref1, const util::SamRecord &rec2,
                                      const std::string &ref2) {
        auto t = getClass(rec1.seq, rec1.cigar, ref1);
        t = std::max(t, getClass(rec2.seq, rec2.cigar, ref2));

        container->rtype_0.push_back(uint8_t(t));
        container->pos_0.push_back(rec1.pos - pos);
        pos = rec1.pos;

        codeVariants(rec1.seq, rec1.cigar, ref1, t, true);
        addSingleRead(rec1, t);

        codeVariants(rec2.seq, rec2.cigar, ref2, t, false);
        addSingleRead(rec2, t);

        if (!container->clips_0.empty() && container->clips_0.get(container->clips_0.size() - 1) == readCounter) {
            container->clips_1.push_back(8);
        }

        container->pair_0.push_back(0);
        uint32_t delta = std::max(rec1.pos, rec2.pos) - std::min(rec1.pos, rec2.pos);
        bool first1 = rec1.pos < rec2.pos;
        container->pair_1.push_back((delta << 1) | first1);

        return t;
    }

    void LocalAssemblyReadEncoder::codeVariants(const std::string &read, const std::string &cigar, const std::string &ref, format::DataUnit::AuType type,
                                           bool isFirst) {
        size_t count = 0;
        size_t read_pos = 0;
        size_t ref_offset = 0;
        size_t num_of_deletions = 0;

        size_t lastMisMatch = 0;
        bool isRightClip = false;
        std::array<std::string, 2> softClips;
        std::array<size_t, 2> hardClips = {0, 0};

        for (size_t cigar_pos = 0; cigar_pos < cigar.length(); ++cigar_pos) {
            if (cigar[cigar_pos] >= '0' && cigar[cigar_pos] <= '9') {
                count *= 10;
                count += cigar[cigar_pos] - '0';
                continue;
            }
            switch (cigar[cigar_pos]) {
                case 'M':
                case '=':
                case 'X':
                    isRightClip = true;
                    for (size_t i = 0; i < count; ++i) {
                        if (read_pos >= read.length()) {
                            UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
                        }
                        if (read[read_pos] != ref[ref_offset]) {
                            if (ref[ref_offset] == 0) {
                                container->ureads_0.push_back(format::getAlphabetProperties(
                                        format::ParameterSet::AlphabetID::ACGTN).inverseLut[read[read_pos]]);
                            } else {
                                container->mmpos_0.push_back(0);
                                container->mmpos_1.push_back(read_pos - lastMisMatch);

                                lastMisMatch = read_pos + 1;
                                container->mmtype_0.push_back(0);
                                if(type > format::DataUnit::AuType::N_TYPE_AU) {
                                    container->mmtype_1.push_back(format::getAlphabetProperties(
                                            format::ParameterSet::AlphabetID::ACGTN).inverseLut[read[read_pos]]);
                                }
                            }
                        }

                        read_pos++;
                        ref_offset++;
                    }
                    break;

                case 'I':
                    for (size_t i = 0; i < count; ++i) {
                        container->mmpos_0.push_back(0);
                        container->mmpos_1.push_back(read_pos - lastMisMatch);
                        lastMisMatch = read_pos + 1;
                        container->mmtype_0.push_back(1);
                        container->mmtype_2.push_back(format::getAlphabetProperties(
                                format::ParameterSet::AlphabetID::ACGTN).inverseLut[read[read_pos]]);
                        read_pos++;
                    }
                    break;
                case 'S':
                case 'P':
                    for (size_t i = 0; i < count; ++i) {
                        if (read_pos >= read.length()) {
                            UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
                        }
                        softClips[isRightClip] += format::getAlphabetProperties(
                                format::ParameterSet::AlphabetID::ACGTN).inverseLut[read[read_pos]];
                        read_pos++;
                    }
                    break;

                case 'N':
                case 'D':
                    for (size_t i = 0; i < count; ++i) {
                        container->mmpos_0.push_back(0);
                        container->mmpos_1.push_back(read_pos - lastMisMatch);
                        lastMisMatch = read_pos;
                        container->mmtype_0.push_back(2);
                        num_of_deletions++;
                        ref_offset++;
                    }
                    break;

                case 'H':
                    hardClips[isRightClip] += count;
                    ref_offset += count;
                    break;

                default:
                    UTILS_THROW_RUNTIME_EXCEPTION("Unknown CIGAR character");
            }
            count = 0;
        }

        if (read_pos != read.length()) {
            UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
        }

        if (!softClips[0].empty() || !softClips[1].empty() || hardClips[0] || hardClips[1]) {
            container->clips_0.push_back(readCounter);
        }

        if (hardClips[0]) {
            if (isFirst) {
                container->clips_1.push_back(4);
            } else {
                container->clips_1.push_back(6);
            }
            container->clips_3.push_back(hardClips[0]);
        }

        if (hardClips[1]) {
            if (isFirst) {
                container->clips_1.push_back(5);
            } else {
                container->clips_1.push_back(7);
            }
            container->clips_3.push_back(hardClips[1]);
        }

        if (!softClips[0].empty() && !hardClips[0]) {
            if (isFirst) {
                container->clips_1.push_back(0);
            } else {
                container->clips_1.push_back(2);
            }
            for (const auto &c : softClips[0]) {
                container->clips_2.push_back(c);
            }
            container->clips_2.push_back(
                    format::getAlphabetProperties(format::ParameterSet::AlphabetID::ACGTN).lut.size());
        }
        if (!softClips[1].empty() && !hardClips[1]) {
            if (isFirst) {
                container->clips_1.push_back(1);
            } else {
                container->clips_1.push_back(3);
            }
            for (const auto &c : softClips[1]) {
                container->clips_2.push_back(c);
            }
            container->clips_2.push_back(
                    format::getAlphabetProperties(format::ParameterSet::AlphabetID::ACGTN).lut.size());
        }
    }

    format::DataUnit::AuType
    LocalAssemblyReadEncoder::getClass(const std::string &read, const std::string &cigar, const std::string &ref) {
        size_t count = 0;
        size_t read_pos = 0;
        size_t ref_offset = 0;
        size_t num_of_deletions = 0;

        format::DataUnit::AuType type = format::DataUnit::AuType::P_TYPE_AU;


        for (size_t cigar_pos = 0; cigar_pos < cigar.length(); ++cigar_pos) {
            if (cigar[cigar_pos] >= '0' && cigar[cigar_pos] <= '9') {
                count *= 10;
                count += cigar[cigar_pos] - '0';
                continue;
            }
            switch (cigar[cigar_pos]) {
                case 'M':
                case '=':
                case 'X':
                    for (size_t i = 0; i < count; ++i) {
                        if (read_pos >= read.length()) {
                            UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
                        }
                        if (read[read_pos] != ref[ref_offset]) {
                            if (ref[ref_offset] == 0) {
                            } else {
                                if(read[read_pos] == 'N') {
                                    type = std::max(type, format::DataUnit::AuType::N_TYPE_AU);
                                } else {
                                    type = std::max(type, format::DataUnit::AuType::M_TYPE_AU);
                                }
                            }
                        }

                        read_pos++;
                        ref_offset++;
                    }
                    break;

                case 'I':
                    for (size_t i = 0; i < count; ++i) {
                        type = std::max(type, format::DataUnit::AuType::I_TYPE_AU);
                        read_pos++;
                    }
                    break;
                case 'S':
                case 'P':
                    type = std::max(type, format::DataUnit::AuType::I_TYPE_AU);
                    for (size_t i = 0; i < count; ++i) {
                        if (read_pos >= read.length()) {
                            UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
                        }
                        read_pos++;
                    }
                    break;

                case 'N':
                case 'D':
                    type = std::max(type, format::DataUnit::AuType::I_TYPE_AU);
                    for (size_t i = 0; i < count; ++i) {
                        num_of_deletions++;
                        ref_offset++;
                    }
                    break;

                case 'H':
                    type = std::max(type, format::DataUnit::AuType::I_TYPE_AU);
                    ref_offset += count;
                    break;

                default:
                    UTILS_THROW_RUNTIME_EXCEPTION("Unknown CIGAR character");
            }
            count = 0;
        }

        if (read_pos != read.length()) {
            UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
        }
        return type;
    }

    std::unique_ptr<StreamContainer> LocalAssemblyReadEncoder::pollStreams() {
        auto tmp = std::move(container);
        container = std::unique_ptr<StreamContainer>(new StreamContainer());
        return tmp;
    }
}