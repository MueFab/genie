#include <array>

#include <format/mpegg_p2/clutter.h>
#include "read-encoder.h"
#include "util/exceptions.h"
#include <format/mpegg_rec/alignment-container.h>
#include <format/mpegg_rec/mpegg-record.h>
#include <format/mpegg_rec/segment.h>

namespace lae {
    LocalAssemblyReadEncoder::LocalAssemblyReadEncoder() : container(util::make_unique<MpeggRawAu>(util::make_unique<format::mpegg_p2::ParameterSet>())), pos(0),
                                                           readCounter(0) {

    }

    void LocalAssemblyReadEncoder::encodeFirstSegment(const format::mpegg_rec::MpeggRecord *rec) {
        container->get(MpeggRawAu::GenomicDescriptor::RTYPE, 0).push(uint8_t(rec->getClassID()));

        container->get(MpeggRawAu::GenomicDescriptor::POS, 0).push(rec->getAlignment(0)->getPosition() - pos);
        pos = rec->getAlignment(0)->getPosition();

        container->get(MpeggRawAu::GenomicDescriptor::RLEN, 0).push(rec->getRecordSegment(0)->getLength() - 1);

        container->get(MpeggRawAu::GenomicDescriptor::RCOMP, 0).push(rec->getAlignment(0)->getAlignment()->getRComp());

        container->get(MpeggRawAu::GenomicDescriptor::FLAGS, 0).push((rec->getFlags() & 0x1u) >> 0u);
        container->get(MpeggRawAu::GenomicDescriptor::FLAGS, 1).push((rec->getFlags() & 0x2u) >> 1u);
        container->get(MpeggRawAu::GenomicDescriptor::FLAGS, 2).push((rec->getFlags() & 0x4u) >> 2u);

        container->get(MpeggRawAu::GenomicDescriptor::MSCORE, 0).push(
                rec->getAlignment(0)->getAlignment()->getMappingScore(0));
    }

    void LocalAssemblyReadEncoder::encodeAdditionalSegment(const format::mpegg_rec::MpeggRecord *rec, size_t index) {
        const format::mpegg_rec::SplitAlignmentSameRec *srec;
        if (rec->getAlignment(0)->getSplitAlignment(index)->getType() ==
            format::mpegg_rec::SplitAlignment::SplitAlignmentType::SAME_REC) {
            srec = dynamic_cast<const format::mpegg_rec::SplitAlignmentSameRec *>(rec->getAlignment(
                    0)->getSplitAlignment(index));
        } else {
            UTILS_DIE("Only same record split alignments supported");
        }

        container->get(MpeggRawAu::GenomicDescriptor::RLEN, 0).push(rec->getRecordSegment(index + 1)->getLength() - 1);
        container->get(MpeggRawAu::GenomicDescriptor::RCOMP, 0).push(srec->getAlignment()->getRComp());
        container->get(MpeggRawAu::GenomicDescriptor::MSCORE, 0).push(srec->getAlignment()->getMappingScore(0));

        uint32_t delta = srec->getDelta();
        bool first1 = srec->getDelta() >= 0;
        container->get(MpeggRawAu::GenomicDescriptor::PAIR, 0).push(0);
        container->get(MpeggRawAu::GenomicDescriptor::PAIR, 1).push((delta << 1u) | first1);
    }

    void LocalAssemblyReadEncoder::add(const format::mpegg_rec::MpeggRecord *rec, const std::string &ref1,
                                       const std::string &ref2) {

        ClipInformation clips[2];

        encodeFirstSegment(rec);
        codeVariants(*rec->getRecordSegment(0)->getSequence(), *rec->getAlignment(0)->getAlignment()->getECigar(), ref1,
                     rec->getClassID(), &clips[0]);

        if(rec->getNumberOfRecords() > 1) {
            encodeAdditionalSegment(rec, 0);
            const format::mpegg_rec::SplitAlignmentSameRec *srec;
            if (rec->getAlignment(0)->getSplitAlignment(0)->getType() ==
                format::mpegg_rec::SplitAlignment::SplitAlignmentType::SAME_REC) {
                srec = dynamic_cast<const format::mpegg_rec::SplitAlignmentSameRec *>(rec->getAlignment(
                        0)->getSplitAlignment(0));
            } else {
                UTILS_DIE("Only same record split alignments supported");
            }
            codeVariants(*rec->getRecordSegment(0)->getSequence(), *srec->getAlignment()->getECigar(), ref2,
                         rec->getClassID(), &clips[1]);
        }

        encodeClips(clips[0], clips[1]);

    }


    void
    LocalAssemblyReadEncoder::codeVariants(const std::string &read, const std::string &cigar, const std::string &ref,
                                           format::mpegg_rec::MpeggRecord::ClassType type, ClipInformation *clips) {
        size_t count = 0;
        size_t read_pos = 0;
        size_t ref_offset = 0;
        size_t num_of_deletions = 0;

        size_t lastMisMatch = 0;
        bool isRightClip = false;


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
                                container->get(MpeggRawAu::GenomicDescriptor::UREADS, 0).push(
                                        MpeggRawAu::getAlphabetProperties(
                                                format::mpegg_p2::ParameterSet::AlphabetID::ACGTN).inverseLut[read[read_pos]]);
                            } else {
                                container->get(MpeggRawAu::GenomicDescriptor::MMPOS, 0).push(0);
                                container->get(MpeggRawAu::GenomicDescriptor::MMPOS, 1).push(read_pos - lastMisMatch);

                                lastMisMatch = read_pos + 1;
                                container->get(MpeggRawAu::GenomicDescriptor::MMTYPE, 0).push(0);
                                if (type > format::mpegg_rec::MpeggRecord::ClassType::CLASS_N) {
                                    container->get(MpeggRawAu::GenomicDescriptor::MMTYPE, 1).push(
                                            MpeggRawAu::getAlphabetProperties(
                                                    format::mpegg_p2::ParameterSet::AlphabetID::ACGTN).inverseLut[read[read_pos]]);
                                }
                            }
                        }

                        read_pos++;
                        ref_offset++;
                    }
                    break;

                case 'I':
                    for (size_t i = 0; i < count; ++i) {
                        container->get(MpeggRawAu::GenomicDescriptor::MMPOS, 0).push(0);
                        container->get(MpeggRawAu::GenomicDescriptor::MMPOS, 1).push(read_pos - lastMisMatch);
                        lastMisMatch = read_pos + 1;
                        container->get(MpeggRawAu::GenomicDescriptor::MMTYPE, 0).push(1);
                        container->get(MpeggRawAu::GenomicDescriptor::MMTYPE, 2).push(
                                MpeggRawAu::getAlphabetProperties(
                                        format::mpegg_p2::ParameterSet::AlphabetID::ACGTN).inverseLut[read[read_pos]]);
                        read_pos++;
                    }
                    break;
                case 'S':
                case 'P':
                    for (size_t i = 0; i < count; ++i) {
                        if (read_pos >= read.length()) {
                            UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
                        }
                        clips->softClips[isRightClip] += MpeggRawAu::getAlphabetProperties(
                                format::mpegg_p2::ParameterSet::AlphabetID::ACGTN).inverseLut[read[read_pos]];
                        read_pos++;
                    }
                    break;

                case 'N':
                case 'D':
                    for (size_t i = 0; i < count; ++i) {
                        container->get(MpeggRawAu::GenomicDescriptor::MMPOS, 0).push(0);
                        container->get(MpeggRawAu::GenomicDescriptor::MMPOS, 1).push(read_pos - lastMisMatch);
                        lastMisMatch = read_pos;
                        container->get(MpeggRawAu::GenomicDescriptor::MMTYPE, 0).push(2);
                        num_of_deletions++;
                        ref_offset++;
                    }
                    break;

                case 'H':
                    clips->hardClips[isRightClip] += count;
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


        if (type > format::mpegg_rec::MpeggRecord::ClassType::CLASS_P) {
            container->get(MpeggRawAu::GenomicDescriptor::MMPOS, 0).push(1);
        }
    }

    std::unique_ptr<MpeggRawAu> LocalAssemblyReadEncoder::pollStreams() {
        auto tmp = std::move(container);
        container = util::make_unique<MpeggRawAu>(util::make_unique<format::mpegg_p2::ParameterSet>());
        return tmp;
    }

    bool LocalAssemblyReadEncoder::encodeClips(const ClipInformation &inf, bool last) {
        bool ret = false;
        if (inf.hardClips[0] || inf.hardClips[1] || !inf.softClips[0].empty() || !inf.softClips[1].empty()) {
            ret = true;
            container->get(MpeggRawAu::GenomicDescriptor::CLIPS, 0).push(readCounter);
        }

        for(size_t index = 0; index < 2; ++index) {
            if (inf.hardClips[index]) {
                container->get(MpeggRawAu::GenomicDescriptor::CLIPS, 1).push(0x4u | (last << 1u) | index);
                container->get(MpeggRawAu::GenomicDescriptor::CLIPS, 3).push(inf.hardClips[index]);
            } else if (!inf.softClips[index].empty()) {
                container->get(MpeggRawAu::GenomicDescriptor::CLIPS, 1).push( (last << 1u) | index);
                for (const auto &c : inf.softClips[index]) {
                    container->get(MpeggRawAu::GenomicDescriptor::CLIPS, 2).push(c);
                }
                container->get(MpeggRawAu::GenomicDescriptor::CLIPS, 2).push(
                        MpeggRawAu::getAlphabetProperties(
                                format::mpegg_p2::ParameterSet::AlphabetID::ACGTN
                        ).lut.size()
                );
            }
        }

        return ret;
    }

    void LocalAssemblyReadEncoder::encodeClips(const ClipInformation &inf1, const ClipInformation &inf2) {
        bool present = encodeClips(inf1, false);
        present = encodeClips(inf2, true) || present;

        if(present) {
            container->get(MpeggRawAu::GenomicDescriptor::CLIPS, 1).push(8);
        }

        readCounter += 2;
    }
}