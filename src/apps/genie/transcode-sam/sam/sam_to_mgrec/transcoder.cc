/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "apps/genie/transcode-sam/sam/sam_to_mgrec/transcoder.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sam_group.h"
#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sam_reader.h"
#include "apps/genie/transcode-sam/sam/sam_to_mgrec/sorter.h"
#include "apps/genie/transcode-sam/utils.h"
#include "boost/optional/optional.hpp"
#include "filesystem/filesystem.hpp"
#include "genie/core/record/alignment_split/other-rec.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace transcode_sam {
namespace sam {
namespace sam_to_mgrec {

// ---------------------------------------------------------------------------------------------------------------------

RefInfo::RefInfo(const std::string& fasta_name)
    : refMgr(genie::util::make_unique<genie::core::ReferenceManager>(4)), valid(false) {
    if (!ghc::filesystem::exists(fasta_name)) {
        return;
    }

    std::string fai_name = fasta_name.substr(0, fasta_name.find_last_of('.')) + ".fai";
    if (!ghc::filesystem::exists(fai_name)) {
        std::ifstream fasta_in(fasta_name);
        std::ofstream fai_out(fai_name);
        genie::format::fasta::FastaReader::index(fasta_in, fai_out);
    }

    fastaFile = genie::util::make_unique<std::ifstream>(fasta_name);
    faiFile = genie::util::make_unique<std::ifstream>(fai_name);

    fastaMgr = genie::util::make_unique<genie::format::fasta::Manager>(*fastaFile, *faiFile, refMgr.get());
    valid = true;
}

// ---------------------------------------------------------------------------------------------------------------------

bool RefInfo::isValid() const { return valid; }

// ---------------------------------------------------------------------------------------------------------------------

genie::core::ReferenceManager* RefInfo::getMgr() { return refMgr.get(); }

// ---------------------------------------------------------------------------------------------------------------------

void phase1_thread(SamReader& sam_reader, int& chunk_id, const std::string& tmp_path, std::mutex& lock) {
    while (true) {
        std::vector<genie::core::record::Record> output_buffer;
        std::vector<std::vector<SamRecord>> queries;
        int ret = 0;
        int this_chunk = 0;

        // Load data

        {
            std::lock_guard<std::mutex> guard(lock);
            this_chunk = chunk_id++;
            std::cerr << "Processing chunk " << this_chunk << "..." << std::endl;
            for (int i = 0; i < PHASE2_BUFFER_SIZE; ++i) {
                queries.emplace_back();
                ret = sam_reader.readSamQuery(queries.back());
                if (ret == EOF) {
                    break;
                }
                UTILS_DIE_IF(ret, "Error reading sam query: " + std::string(strerror(ret)));
            }
        }

        // Convert data
        for (auto& q : queries) {
            SamRecordGroup buffer;
            for (auto& s : q) {
                buffer.addRecord(std::move(s));
            }
            std::list<genie::core::record::Record> records;
            buffer.convert(records);
            for (auto& m : records) {
                output_buffer.push_back(std::move(m));
            }
            q.clear();
        }
        queries.clear();

        // Sort data

        std::sort(output_buffer.begin(), output_buffer.end(), compare);

        // Write data
        {
            std::string fpath = tmp_path + "/" + std::to_string(this_chunk) + PHASE1_EXT;
            std::ofstream output_file(fpath, std::ios::trunc | std::ios::binary);
            genie::util::BitWriter bwriter(&output_file);

            for (auto& r : output_buffer) {
                r.write(bwriter);
            }

            bwriter.flush();
        }

        if (ret == EOF) {
            return;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::pair<std::string, size_t>> sam_to_mgrec_phase1(Config& options, int& chunk_id) {
    auto sam_reader = SamReader(options.inputFile);
    UTILS_DIE_IF(!sam_reader.isReady() || !sam_reader.isValid(), "Cannot open SAM file.");

    chunk_id = 0;
    std::mutex lock;
    std::vector<std::thread> threads;
    threads.reserve(options.num_threads);
    for (uint32_t i = 0; i < options.num_threads; ++i) {
        threads.emplace_back([&]() { phase1_thread(sam_reader, chunk_id, options.tmp_dir_path, lock); });
    }
    for (auto& t : threads) {
        t.join();
    }

    auto refs = sam_reader.getRefs();
    return refs;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string patch_ecigar(const std::string& ref, const std::string& seq, const std::string& ecigar) {
    std::string fixedCigar;
    auto classChecker = [&](uint8_t cigar, const genie::util::StringView& _bs,
                            const genie::util::StringView& _rs) -> bool {
        auto bs = _bs.deploy(seq.data());
        auto rs = _rs.deploy(ref.data());
        auto length = std::max(bs.length(), rs.length());
        switch (cigar) {
            case '+':
            case '-':
            case ')':
            case ']':
            case '*':
            case '/':
            case '%':
                if (cigar == ')') {
                    fixedCigar += std::string(1, '(');
                }
                if (cigar == ']') {
                    fixedCigar += std::string(1, '[');
                }
                fixedCigar += std::to_string(length) + std::string(1, cigar);
                return true;
            default:
                break;
        }

        if (genie::core::getAlphabetProperties(genie::core::AlphabetID::ACGTN).isIncluded(cigar)) {
            fixedCigar += std::string(1, cigar);
            return true;
        }

        UTILS_DIE_IF(cigar != '=', "Unknown ecigar char.");

        size_t counter = 0;

        for (size_t i = 0; i < length; ++i) {
            if (*(bs.begin() + i) != *(rs.begin() + i)) {
                if (counter != 0) {
                    fixedCigar += std::to_string(counter) + "=";
                    counter = 0;
                }
                fixedCigar += std::string(1, *(bs.begin() + i));

            } else {
                counter++;
            }
        }

        if (counter != 0) {
            fixedCigar += std::to_string(counter) + "=";
            counter = 0;
        }

        return true;
    };

    genie::core::CigarTokenizer::tokenize(ecigar, genie::core::getECigarInfo(), classChecker);
    return fixedCigar;
}

// ---------------------------------------------------------------------------------------------------------------------

genie::core::record::ClassType classifyEcigar(const std::string& cigar) {
    genie::core::record::ClassType ret = genie::core::record::ClassType::CLASS_P;
    for (const auto& c : cigar) {
        if (c >= '0' && c <= '9') {
            continue;
        }
        if (c == '+' || c == '-' || c == '(' || c == '[') {
            return genie::core::record::ClassType::CLASS_I;
        }
        if (genie::core::getAlphabetProperties(genie::core::AlphabetID::ACGTN).isIncluded(c)) {
            if (c == 'N') {
                ret = std::max(ret, genie::core::record::ClassType::CLASS_N);
            } else {
                ret = std::max(ret, genie::core::record::ClassType::CLASS_M);
            }
        }
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void fix_ecigar(genie::core::record::Record& r, const std::vector<std::pair<std::string, size_t>>& refNames,
                RefInfo& ref) {
    if (r.getClassID() == genie::core::record::ClassType::CLASS_U) {
        return;
    }

    if (!ref.isValid()) {
        return;
    }

    size_t alignment_ctr = 0;
    for (auto& a : r.getAlignments()) {
        auto pos = a.getPosition();
        auto refSeq = ref.getMgr()
                          ->load(refNames[r.getAlignmentSharedData().getSeqID()].first, pos,
                                 pos + r.getMappedLength(alignment_ctr, 0))
                          .getString(pos, pos + r.getMappedLength(alignment_ctr, 0));
        auto cigar = a.getAlignment().getECigar();
        auto seq = r.getSegments()[r.getSegments().size() == 2 ? !r.isRead1First() : 0].getSequence();

        cigar = patch_ecigar(refSeq, seq, cigar);

        if (r.getClassID() != genie::core::record::ClassType::CLASS_HM) {
            r.setClassType(classifyEcigar(cigar));
        }

        auto alg = genie::core::record::Alignment(std::move(cigar), a.getAlignment().getRComp());
        for (const auto& s : a.getAlignment().getMappingScores()) {
            alg.addMappingScore(s);
        }
        genie::core::record::AlignmentBox newBox(a.getPosition(), std::move(alg));

        // -----------

        if (a.getAlignmentSplits().size() == 1) {
            if (a.getAlignmentSplits().front()->getType() == genie::core::record::AlignmentSplit::Type::SAME_REC) {
                const auto& split =
                    dynamic_cast<const genie::core::record::alignment_split::SameRec&>(*a.getAlignmentSplits().front());
                pos = a.getPosition() + split.getDelta();
                refSeq = ref.getMgr()
                             ->load(refNames[r.getAlignmentSharedData().getSeqID()].first, pos,
                                    pos + r.getMappedLength(alignment_ctr, 1))
                             .getString(pos, pos + r.getMappedLength(alignment_ctr, 1));
                cigar = split.getAlignment().getECigar();
                seq = r.getSegments()[r.isRead1First()].getSequence();

                cigar = patch_ecigar(refSeq, seq, cigar);

                if (r.getClassID() != genie::core::record::ClassType::CLASS_HM) {
                    r.setClassType(std::max(r.getClassID(), classifyEcigar(cigar)));
                }

                alg = genie::core::record::Alignment(std::move(cigar), split.getAlignment().getRComp());
                for (const auto& s : split.getAlignment().getMappingScores()) {
                    alg.addMappingScore(s);
                }

                newBox.addAlignmentSplit(genie::util::make_unique<genie::core::record::alignment_split::SameRec>(
                    split.getDelta(),
                    std::move(alg)));
            } else {
                newBox.addAlignmentSplit(a.getAlignmentSplits().front()->clone());
            }
        }
        r.setAlignment(alignment_ctr, std::move(newBox));
        alignment_ctr++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void sam_to_mgrec_phase2(Config& options, int num_chunks, const std::vector<std::pair<std::string, size_t>>& refs) {
    std::cerr << "Merging " << num_chunks << " chunks..." << std::endl;
    RefInfo refinf(options.fasta_file_path);

    std::unique_ptr<std::ostream> total_output;
    std::ostream* out_stream = &std::cout;
    if (options.outputFile.substr(0, 2) != "-.") {
        total_output = genie::util::make_unique<std::ofstream>(options.outputFile, std::ios::binary | std::ios::trunc);
        out_stream = total_output.get();
    }
    genie::util::BitWriter total_output_writer(out_stream);

    std::vector<std::unique_ptr<SubfileReader>> readers;
    readers.reserve(num_chunks);
    auto cmp = [&](const SubfileReader* a, SubfileReader* b) {
        return !compare(a->getRecord().get(), b->getRecord().get());
    };
    std::priority_queue<SubfileReader*, std::vector<SubfileReader*>, decltype(cmp)> heap(cmp);
    for (int i = 0; i < num_chunks; ++i) {
        readers.emplace_back(
            genie::util::make_unique<SubfileReader>(options.tmp_dir_path + "/" + std::to_string(i) + PHASE1_EXT));
        if (!readers.back()->getRecord()) {
            auto path = readers.back()->getPath();
            std::cerr << path << " depleted" << std::endl;
            readers.pop_back();
            std::remove(path.c_str());
        } else {
            heap.push(readers.back().get());
        }
    }

    while (true) {
        auto* reader = heap.top();
        heap.pop();
        auto rec = reader->moveRecord();

        fix_ecigar(rec, refs, refinf);

        rec.write(total_output_writer);

        if (reader->getRecord()) {
            heap.push(reader);
        } else {
            auto path = reader->getPath();
            std::cerr << path << " depleted" << std::endl;
            for (auto it = readers.begin(); it != readers.end(); ++it) {
                if (it->get() == reader) {
                    readers.erase(it);
                    break;
                }
            }
            std::remove(path.c_str());

            if (heap.empty()) {
                break;
            }
        }
    }

    total_output_writer.flush();
    out_stream->flush();

    std::cerr << "Finished merging!" << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

void transcode_sam2mpg(Config& options) {
    int nref;

    auto refs = sam_to_mgrec_phase1(options, nref);
    sam_to_mgrec_phase2(options, nref, refs);
}

// ---------------------------------------------------------------------------------------------------------------------

char convertECigar2CigarChar(char token) {
    static const auto lut_loc = []() -> std::string {
        std::string lut(128, 0);
        lut['='] = 'M';
        lut['+'] = 'I';
        lut['-'] = 'D';
        lut['*'] = 'N';
        lut[')'] = 'S';
        lut[']'] = 'H';
        lut['%'] = 'N';
        lut['/'] = 'N';
        return lut;
    }();
    UTILS_DIE_IF(token < 0, "Invalid cigar token" + std::to_string(token));
    char ret = lut_loc[token];
    UTILS_DIE_IF(ret == 0, "Invalid cigar token" + std::to_string(token));
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

int stepRef(char token) {
    static const auto lut_loc = []() -> std::string {
        std::string lut(128, 0);
        lut['M'] = 1;
        lut['='] = 1;
        lut['X'] = 1;
        lut['I'] = 0;
        lut['D'] = 1;
        lut['N'] = 1;
        lut['S'] = 0;
        lut['H'] = 0;
        lut['P'] = 0;
        return lut;
    }();
    UTILS_DIE_IF(token < 0, "Invalid cigar token" + std::to_string(token));
    return lut_loc[token];
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t mappedLength(const std::string& cigar) {
    if (cigar == "*") {
        return 0;
    }
    std::string digits;
    uint64_t length = 0;
    for (const auto& c : cigar) {
        if (std::isdigit(c)) {
            digits += c;
            continue;
        }
        length += std::stoi(digits) * stepRef(c);
        digits.clear();
    }
    return length;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string eCigar2Cigar(const std::string& ecigar) {
    std::string cigar;
    cigar.reserve(ecigar.size());
    size_t mismatchCount = 0;
    std::string mismatchDigits;
    for (const auto& c : ecigar) {
        if ((c == '[') || (c == '(')) {
            continue;
        }
        if (std::isdigit(c)) {
            if (mismatchCount) {
                mismatchDigits += c;
            } else {
                cigar.push_back(c);
            }
            continue;
        }
        if (genie::core::getAlphabetProperties(genie::core::AlphabetID::ACGTN).isIncluded(c)) {
            ++mismatchCount;
        } else {
            if (mismatchCount) {
                cigar += std::to_string(mismatchCount) + 'X';
                cigar += mismatchDigits;
                mismatchDigits.clear();
                mismatchCount = 0;
            }
            cigar.push_back(convertECigar2CigarChar(c));
        }
    }
    if (mismatchCount) {
        cigar += std::to_string(mismatchCount) + 'X';
        mismatchCount = 0;
    }
    return cigar;
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t computeSAMFlags(size_t s, size_t a, const genie::core::record::Record& record) {
    uint16_t flags = 0;
    if (record.getNumberOfTemplateSegments() > 1) {
        flags |= 0x1;
    }
    if (record.getFlags() & genie::core::GenConst::FLAGS_PROPER_PAIR_MASK) {
        flags |= 0x2;
    }
    // This read is unmapped
    if (record.getClassID() == genie::core::record::ClassType::CLASS_U ||
        ((record.getClassID() == genie::core::record::ClassType::CLASS_HM) &&
         ((s == 1 && record.isRead1First()) || (s == 0 && !record.isRead1First())))) {
        flags |= 0x4;
    }
    // Paired read is unmapped
    if (record.getClassID() == genie::core::record::ClassType::CLASS_U ||
        ((record.getClassID() == genie::core::record::ClassType::CLASS_HM) &&
         ((s == 0 && record.isRead1First()) || (s == 1 && !record.isRead1First())))) {
        flags |= 0x8;
    }
    // First or second read?
    if ((s == 0 && record.getSegments().size() == 2) || (record.getSegments().size() == 1 && record.isRead1First())) {
        flags |= 0x40;
    } else {
        flags |= 0x80;
    }
    // Secondary alignment
    if (a > 0) {
        flags |= 0x100;
    }
    if (record.getFlags() & genie::core::GenConst::FLAGS_QUALITY_FAIL_MASK) {
        flags |= 0x200;
    }
    if (record.getFlags() & genie::core::GenConst::FLAGS_PCR_DUPLICATE_MASK) {
        flags |= 0x400;
    }
    return flags;
}

// ---------------------------------------------------------------------------------------------------------------------

void processFirstMappedSegment(size_t s, size_t a, const genie::core::record::Record& record, std::string& rname,
                               std::string& pos, int64_t& tlen, std::string& mapping_qual, std::string& cigar,
                               uint16_t& flags) {
    // This read is mapped, process mapping
    rname = std::to_string(record.getAlignmentSharedData().getSeqID());
    if ((s == 0 && record.isRead1First()) || (s == 1 && !record.isRead1First()) || record.getSegments().size() == 1) {
        // First segment is in primary alignment
        pos = std::to_string(record.getAlignments()[a].getPosition() + 1);
        tlen -= record.getAlignments()[a].getPosition() + 1;
        mapping_qual = record.getAlignments()[a].getAlignment().getMappingScores().empty()
                           ? "0"
                           : std::to_string(record.getAlignments()[a].getAlignment().getMappingScores().front());
        cigar = eCigar2Cigar(record.getAlignments()[a].getAlignment().getECigar());
        if (record.getAlignments()[a].getAlignment().getRComp()) {
            flags |= 0x10;
        }
    } else {
        // First segment is mapped in split alignment
        const auto& split = dynamic_cast<const genie::core::record::alignment_split::SameRec&>(
            *record.getAlignments()[a].getAlignmentSplits().front().get());
        cigar = eCigar2Cigar(split.getAlignment().getECigar());
        pos = std::to_string(record.getAlignments()[a].getPosition() + split.getDelta() + 1);
        tlen -= record.getAlignments()[a].getPosition() + split.getDelta() + 1 + mappedLength(cigar);
        mapping_qual = split.getAlignment().getMappingScores().empty()
                           ? "0"
                           : std::to_string(split.getAlignment().getMappingScores().front());
        if (split.getAlignment().getRComp()) {
            flags |= 0x10;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void processSecondMappedSegment(size_t s, const genie::core::record::Record& record, int64_t& tlen, uint16_t& flags,
                                std::string& pnext, std::string& rnext) {
    // According to SAM standard, primary alignments only
    auto split_type = record.getClassID() == genie::core::record::ClassType::CLASS_HM
                          ? genie::core::record::AlignmentSplit::Type::UNPAIRED
                          : record.getAlignments()[0].getAlignmentSplits().front()->getType();
    if ((((s == 1 && record.isRead1First()) || (s == 0 && !record.isRead1First())) &&
         split_type == genie::core::record::AlignmentSplit::Type::SAME_REC) ||
        (split_type == genie::core::record::AlignmentSplit::Type::UNPAIRED)) {
        // Paired read is first read
        pnext = std::to_string(record.getAlignments()[0].getPosition() + 1);
        tlen += record.getAlignments()[0].getPosition() + 1;
        rnext = std::to_string(record.getAlignmentSharedData().getSeqID());
        if (record.getAlignments()[0].getAlignment().getRComp()) {
            flags |= 0x20;
        }
    } else {
        // Paired read is second read
        if (split_type == genie::core::record::AlignmentSplit::Type::SAME_REC) {
            const auto& split = dynamic_cast<const genie::core::record::alignment_split::SameRec&>(
                *record.getAlignments()[0].getAlignmentSplits().front().get());
            pnext = std::to_string(record.getAlignments()[0].getPosition() + split.getDelta() + 1);
            tlen += record.getAlignments()[0].getPosition() + split.getDelta() + 1 +
                    mappedLength(eCigar2Cigar(split.getAlignment().getECigar()));

            rnext = std::to_string(record.getAlignmentSharedData().getSeqID());
            if (split.getAlignment().getRComp()) {
                flags |= 0x20;
            }
        } else if (split_type == genie::core::record::AlignmentSplit::Type::OTHER_REC) {
            const auto& split = dynamic_cast<const genie::core::record::alignment_split::OtherRec&>(
                *record.getAlignments()[0].getAlignmentSplits().front().get());
            rnext = std::to_string(split.getNextSeq());
            pnext = std::to_string(split.getNextPos() + 1);
            tlen = 0;  // Not available without reading second record
        } else {
            tlen = 0;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void transcode_mpg2sam(Config& options) {
    std::istream* input_file = &std::cin;
    std::ostream* output_file = &std::cout;
    boost::optional<std::ifstream> input_stream;
    boost::optional<std::ofstream> output_stream;

    if (options.inputFile.substr(0, 2) != "-.") {
        input_stream = std::ifstream(options.inputFile);
        input_file = &input_stream.get();
    }

    if (options.outputFile.substr(0, 2) != "-.") {
        output_stream = std::ofstream(options.outputFile);
        output_file = &output_stream.get();
    }

    genie::util::BitReader reader(*input_file);

    while (reader.isGood()) {
        genie::core::record::Record record(reader);
        // One line per segment and alignment
        for (size_t s = 0; s < record.getSegments().size(); ++s) {
            for (size_t a = 0; a < std::max(record.getAlignments().size(), size_t(1)); ++a) {
                std::string sam_record = record.getName() + "\t";
                uint16_t flags = computeSAMFlags(s, a, record);
                bool mapped = !(flags & 0x4);
                bool other_mapped = !(flags & 0x8);

                std::string rname = "*";
                std::string pos = "0";
                std::string mapping_qual = "0";
                std::string cigar = "*";
                std::string rnext = "*";
                std::string pnext = "0";
                int64_t tlen = 0;
                if (mapped) {
                    // First segment is mapped
                    processFirstMappedSegment(s, a, record, rname, pos, tlen, mapping_qual, cigar, flags);
                } else if (record.getClassID() == genie::core::record::ClassType::CLASS_HM) {
                    // According to SAM standard, HM-like records should have same position for the unmapped part, too
                    pos = std::to_string(record.getAlignments()[a].getPosition() + 1);
                    rname = std::to_string(record.getAlignmentSharedData().getSeqID());
                    if (a > 0) {
                        // No need to write unmapped records for secondary alignmentd
                        continue;
                    }
                }

                if (other_mapped && record.getNumberOfTemplateSegments() == 2) {
                    processSecondMappedSegment(s, record, tlen, flags, pnext, rnext);
                } else {
                    rnext = rname;
                    pnext = pos;
                    tlen = 0;
                }

                // Use "=" shorthand
                if (rnext == rname && rnext != "*") {
                    rnext = "=";
                }

                if (record.getClassID() == genie::core::record::ClassType::CLASS_HM ||
                    record.getClassID() == genie::core::record::ClassType::CLASS_U) {
                    tlen = 0;
                }

                sam_record += std::to_string(flags) + "\t";
                sam_record += rname + "\t";
                sam_record += pos + "\t";
                sam_record += mapping_qual + "\t";
                sam_record += cigar + "\t";
                sam_record += rnext + "\t";
                sam_record += pnext + "\t";
                sam_record += std::to_string(tlen) + "\t";
                sam_record += record.getSegments()[s].getSequence() + "\t";
                if (record.getSegments()[s].getQualities().empty()) {
                    sam_record += "*\n";
                } else {
                    sam_record += record.getSegments()[s].getQualities()[0] + "\n";
                }

                output_file->write(sam_record.c_str(), sam_record.length());
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace sam_to_mgrec
}  // namespace sam
}  // namespace transcode_sam
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
