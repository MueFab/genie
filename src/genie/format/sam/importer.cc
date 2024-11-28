/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/sam/importer.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/cigar-tokenizer.h"
#include "genie/core/record/alignment_split/other-rec.h"
#include "genie/core/record/class-type.h"
#include "genie/format/sam/sam_to_mgrec/sam_group.h"
#include "genie/format/sam/sam_to_mgrec/sam_reader.h"
#include "genie/format/sam/sam_to_mgrec/sorter.h"
#include "genie/format/sam/sam_to_mgrec/transcoder.h"
#include "genie/util/ordered-section.h"
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::sam {

RefInfo::RefInfo(const std::string& fasta_name)
    : refMgr(std::make_unique<genie::core::ReferenceManager>(4)), valid(false) {
    if (!std::filesystem::exists(fasta_name)) {
        return;
    }

    std::string fai_name = fasta_name.substr(0, fasta_name.find_last_of('.')) + ".fai";
    std::string sha_name = fasta_name.substr(0, fasta_name.find_last_of('.')) + ".sha256";
    if (!std::filesystem::exists(fai_name)) {
        std::ifstream fasta_in(fasta_name);
        std::ofstream fai_out(fai_name);
        genie::format::fasta::FastaReader::index(fasta_in, fai_out);
    }
    if (!std::filesystem::exists(sha_name)) {
        std::ifstream fasta_in(fasta_name);
        std::ifstream fai_in(fai_name);
        genie::format::fasta::FaiFile faifile(fai_in);
        std::ofstream sha_out(sha_name);
        genie::format::fasta::FastaReader::hash(faifile, fai_in, sha_out);
    }

    fastaFile = std::make_unique<std::ifstream>(fasta_name);
    faiFile = std::make_unique<std::ifstream>(fai_name);
    shaFile = std::make_unique<std::ifstream>(sha_name);

    fastaMgr =
        std::make_unique<genie::format::fasta::Manager>(*fastaFile, *faiFile, *shaFile, refMgr.get(), fasta_name);
    valid = true;
}

// ---------------------------------------------------------------------------------------------------------------------

bool RefInfo::isValid() const { return valid; }

// ---------------------------------------------------------------------------------------------------------------------

genie::core::ReferenceManager* RefInfo::getMgr() { return refMgr.get(); }

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(size_t _blockSize, std::string input, std::string ref)
    : blockSize(_blockSize),
      input_sam_file(std::move(input)),
      input_ref_file(std::move(ref)),
      phase1_complete(false),
      reader_prio(CmpReaders()),
      refinf(input_ref_file) {}

// ---------------------------------------------------------------------------------------------------------------------

bool CmpReaders::operator()(const sam_to_mgrec::SubfileReader* a, sam_to_mgrec::SubfileReader* b) const {
    return !sam_to_mgrec::compare(a->getRecord().value(), b->getRecord().value());
}

struct CleanStatistics {
    size_t hm_recs{};
    size_t splice_recs{};
    size_t distance{};
    size_t additional_alignments{};
};

// ---------------------------------------------------------------------------------------------------------------------

std::string patch_ecigar(const std::string& ref, const std::string& seq, const std::string& ecigar) {
    std::string fixedCigar;
    auto classChecker = [&](uint8_t cigar, const std::pair<size_t, size_t>& _bs,
                            const std::pair<size_t, size_t>& _rs) -> bool {
        auto bs = std::string_view(seq).substr(_bs.first, _bs.second);
        auto rs = std::string_view(ref).substr(_rs.first, _rs.second);
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

bool validateBases(const std::string& seq, const genie::core::Alphabet& alphabet) {
    return std::all_of(seq.begin(), seq.end(), [&alphabet](const char& c) { return alphabet.isIncluded(c); });
}

// ---------------------------------------------------------------------------------------------------------------------

bool fix_ecigar(genie::core::record::Record& r, const std::vector<std::pair<std::string, size_t>>&, RefInfo& ref) {
    if (r.getClassID() == genie::core::record::ClassType::CLASS_U) {
        return true;
    }

    if (!ref.isValid()) {
        return true;
    }

    size_t alignment_ctr = 0;
    for (auto& a : r.getAlignments()) {
        auto pos = a.getPosition();
        auto refSeq = ref.getMgr()
                          ->load(ref.getMgr()->ID2Ref(r.getAlignmentSharedData().getSeqID()), pos,
                                 pos + r.getMappedLength(alignment_ctr, 0))
                          .getString(pos, pos + r.getMappedLength(alignment_ctr, 0));
        auto cigar = a.getAlignment().getECigar();
        auto seq = r.getSegments()[0].getSequence();

        if (!validateBases(seq, genie::core::getAlphabetProperties(genie::core::AlphabetID::ACGTN))) {
            return false;
        }

        if (!validateBases(refSeq, genie::core::getAlphabetProperties(genie::core::AlphabetID::ACGTN))) {
            return false;
        }

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
                auto ex = ref.getMgr()->load(ref.getMgr()->ID2Ref(r.getAlignmentSharedData().getSeqID()), pos,
                                             pos + r.getMappedLength(alignment_ctr, 1));
                refSeq = ex.getString(pos, pos + r.getMappedLength(alignment_ctr, 1));
                cigar = split.getAlignment().getECigar();
                seq = r.getSegments()[1].getSequence();

                if (!validateBases(seq, genie::core::getAlphabetProperties(genie::core::AlphabetID::ACGTN))) {
                    return false;
                }

                if (!validateBases(refSeq, genie::core::getAlphabetProperties(genie::core::AlphabetID::ACGTN))) {
                    return false;
                }

                cigar = patch_ecigar(refSeq, seq, cigar);

                if (r.getClassID() != genie::core::record::ClassType::CLASS_HM) {
                    r.setClassType(std::max(r.getClassID(), classifyEcigar(cigar)));
                }

                alg = genie::core::record::Alignment(std::move(cigar), split.getAlignment().getRComp());
                for (const auto& s : split.getAlignment().getMappingScores()) {
                    alg.addMappingScore(s);
                }

                newBox.addAlignmentSplit(
                    std::make_unique<genie::core::record::alignment_split::SameRec>(split.getDelta(), std::move(alg)));
            } else {
                newBox.addAlignmentSplit(a.getAlignmentSplits().front()->clone());
            }
        }
        r.setAlignment(alignment_ctr, std::move(newBox));
        alignment_ctr++;
    }
    return true;
}

std::vector<genie::core::record::Record> splitRecord(genie::core::record::Record&& rec) {
    auto input = std::move(rec);
    std::vector<genie::core::record::Record> ret;
    if (input.getSegments().size() != 2) {
        ret.emplace_back(std::move(input));
        return ret;
    }
    switch (input.getClassID()) {
        case genie::core::record::ClassType::CLASS_HM: {
            genie::core::record::AlignmentBox box(
                input.getAlignments().front().getPosition(),
                genie::core::record::Alignment(input.getAlignments().front().getAlignment()));

            ret.emplace_back(input.getNumberOfTemplateSegments(), genie::core::record::ClassType::CLASS_I,
                             std::string(input.getName()), std::string(input.getGroup()), input.getFlags(),
                             input.isRead1First());
            ret.back().setQVDepth(1);
            ret.back().getSegments().emplace_back(input.getSegments()[0]);

            ret.emplace_back(input.getNumberOfTemplateSegments(), genie::core::record::ClassType::CLASS_U,
                             std::string(input.getName()), std::string(input.getGroup()), input.getFlags(),
                             !input.isRead1First());
            ret.back().setQVDepth(1);
            ret.back().getSegments().emplace_back(input.getSegments()[1]);
        } break;
        case genie::core::record::ClassType::CLASS_U:
            ret.emplace_back(input.getNumberOfTemplateSegments(), genie::core::record::ClassType::CLASS_U,
                             std::string(input.getName()), std::string(input.getGroup()), input.getFlags(),
                             input.isRead1First());
            ret.back().getSegments().emplace_back(input.getSegments()[0]);
            ret.back().setQVDepth(1);

            ret.emplace_back(input.getNumberOfTemplateSegments(), genie::core::record::ClassType::CLASS_U,
                             std::string(input.getName()), std::string(input.getGroup()), input.getFlags(),
                             !input.isRead1First());
            ret.back().getSegments().emplace_back(input.getSegments()[1]);
            ret.back().setQVDepth(1);
            break;
        default:
            ret.emplace_back(input.getNumberOfTemplateSegments(), input.getClassID(), std::string(input.getName()),
                             std::string(input.getGroup()), input.getFlags(), input.isRead1First());
            ret.back().setQVDepth(1);
            ret.back().getSegments().emplace_back(input.getSegments()[0]);
            genie::core::record::AlignmentBox box(
                input.getAlignments().front().getPosition(),
                genie::core::record::Alignment(input.getAlignments().front().getAlignment()));

            const auto& split = dynamic_cast<const genie::core::record::alignment_split::SameRec&>(
                *input.getAlignments().front().getAlignmentSplits().front());
            genie::core::record::AlignmentBox box2(input.getAlignments().front().getPosition() + split.getDelta(),
                                                   genie::core::record::Alignment(split.getAlignment()));

            box.addAlignmentSplit(std::make_unique<genie::core::record::alignment_split::OtherRec>(
                box2.getPosition(), input.getAlignmentSharedData().getSeqID()));

            box2.addAlignmentSplit(std::make_unique<genie::core::record::alignment_split::OtherRec>(
                box.getPosition(), input.getAlignmentSharedData().getSeqID()));

            ret.back().addAlignment(input.getAlignmentSharedData().getSeqID(), genie::core::record::AlignmentBox(box));

            ret.emplace_back(input.getNumberOfTemplateSegments(), input.getClassID(), std::string(input.getName()),
                             std::string(input.getGroup()), input.getFlags(), !input.isRead1First());
            ret.back().setQVDepth(1);
            ret.back().getSegments().emplace_back(input.getSegments()[1]);
            ret.back().addAlignment(input.getAlignmentSharedData().getSeqID(), genie::core::record::AlignmentBox(box2));
    }
    return ret;
}

genie::core::record::Record dealignRecord(genie::core::record::Record&& rec) {
    auto input = std::move(rec);
    genie::core::record::Record ret(input.getNumberOfTemplateSegments(), genie::core::record::ClassType::CLASS_U,
                                    std::string(input.getName()), std::string(input.getGroup()), input.getFlags(),
                                    input.isRead1First());

    for (auto& i : input.getSegments()) {
        ret.getSegments().emplace_back(i);
    }
    ret.setQVDepth(1);
    return ret;
}

bool isECigarSupported(const std::string& ecigar) {
    // Splices not supported
    if ((ecigar.find_first_of('*') != std::string::npos) || (ecigar.find_first_of('/') != std::string::npos) ||
        (ecigar.find_first_of('%') != std::string::npos)) {
        return false;
    }
    return true;
}

genie::core::record::Record stripAdditionalAlignments(genie::core::record::Record&& rec) {
    if (rec.getClassID() == genie::core::record::ClassType::CLASS_U || rec.getAlignments().size() < 2) {
        return std::move(rec);
    }

    auto input = std::move(rec);
    genie::core::record::Record ret(input.getNumberOfTemplateSegments(), input.getClassID(),
                                    std::string(input.getName()), std::string(input.getGroup()), input.getFlags(),
                                    input.isRead1First());

    for (auto& i : input.getSegments()) {
        ret.getSegments().emplace_back(i);
    }

    ret.addAlignment(input.getAlignmentSharedData().getSeqID(),
                     genie::core::record::AlignmentBox(input.getAlignments().front()));
    ret.setQVDepth(1);
    return ret;
}

std::pair<std::vector<genie::core::record::Record>, CleanStatistics> cleanRecord(genie::core::record::Record&& input) {
    std::vector<genie::core::record::Record> ret;
    CleanStatistics stats;
    stats.additional_alignments += input.getAlignments().empty() ? 0 : input.getAlignments().size() - 1;
    ret.emplace_back(stripAdditionalAlignments(std::move(input)));
    for (size_t i = 0; i < ret.size(); ++i) {
        /*       if (ret[i].getClassID() == genie::core::record::ClassType::CLASS_U &&
                   ret[i].getSegments().size() != ret[i].getNumberOfTemplateSegments()) {
                   return false;
               }*/
        if (ret[i].getClassID() == genie::core::record::ClassType::CLASS_HM) {
            ret[i] = dealignRecord(std::move(ret[i]));
            stats.hm_recs++;
            break;
        }
        if (ret[i].getClassID() == genie::core::record::ClassType::CLASS_U) {
            continue;
        }
        if (!isECigarSupported(ret[i].getAlignments().front().getAlignment().getECigar())) {
            ret[i] = dealignRecord(std::move(ret[i]));
            stats.splice_recs++;
            break;
        }
        for (const auto& s : ret[i].getAlignments().front().getAlignmentSplits()) {
            if (s->getType() == genie::core::record::AlignmentSplit::Type::SAME_REC) {
                if (!isECigarSupported(
                        dynamic_cast<genie::core::record::alignment_split::SameRec&>(*s).getAlignment().getECigar())) {
                    ret[i] = dealignRecord(std::move(ret[i]));
                    stats.splice_recs++;
                    break;
                }
                // Splits with more than 32767 delta must be encoded in separate records, which is not yet supported
                if (std::abs(dynamic_cast<genie::core::record::alignment_split::SameRec&>(*s).getDelta()) > 32767) {
                    auto split = splitRecord(std::move(ret[i]));
                    ret[i] = std::move(split[0]);
                    stats.distance++;
                    for (size_t j = 1; j < split.size(); ++j) {
                        ret.emplace_back(std::move(split[j]));
                    }
                    break;
                }
            }
        }
    }
    return std::make_pair(ret, stats);
}

// ---------------------------------------------------------------------------------------------------------------------

void phase1_thread(sam_to_mgrec::SamReader& sam_reader, int& chunk_id, const std::string& tmp_path, bool clean,
                   std::mutex& lock, CleanStatistics& stats) {
    while (true) {
        std::vector<genie::core::record::Record> output_buffer;
        std::vector<std::vector<sam_to_mgrec::SamRecord>> queries;
        int ret = 0;
        int this_chunk = 0;

        CleanStatistics local_stats;

        // Load data

        {
            std::lock_guard<std::mutex> guard(lock);
            this_chunk = chunk_id++;
            std::cerr << "Processing chunk " << this_chunk << "..." << std::endl;
            for (int i = 0; i < PHASE2_BUFFER_SIZE; ++i) {
                queries.emplace_back();
                ret = sam_reader.readSamQuery(queries.back());
                if (ret == EOF) {
                    if (queries.back().empty()) {
                        queries.pop_back();
                    }
                    break;
                }
                UTILS_DIE_IF(ret, "Error reading sam query: " + std::string(strerror(ret)));
            }
        }

        // Convert data
        for (auto& q : queries) {
            sam_to_mgrec::SamRecordGroup buffer;
            for (auto& s : q) {
                buffer.addRecord(std::move(s));
            }
            std::list<genie::core::record::Record> records;
            buffer.convert(records);
            for (auto& m : records) {
                std::vector<genie::core::record::Record> buf;
                if (clean) {
                    auto r = cleanRecord(std::move(m));
                    buf = std::move(r.first);
                    local_stats.splice_recs += r.second.splice_recs;
                    local_stats.distance += r.second.distance;
                    local_stats.additional_alignments += r.second.additional_alignments;
                    local_stats.hm_recs += r.second.hm_recs;
                } else {
                    buf.emplace_back(std::move(m));
                }
                for (auto& b : buf) {
                    output_buffer.push_back(std::move(b));
                }
            }
            q.clear();
        }
        queries.clear();

        // Sort data

        std::sort(output_buffer.begin(), output_buffer.end(), sam_to_mgrec::compare);

        // Write data
        {
            std::string fpath = tmp_path + "/" + std::to_string(this_chunk) + PHASE1_EXT;
            std::ofstream output_file(fpath, std::ios::trunc | std::ios::binary);
            genie::util::BitWriter bwriter(output_file);

            for (auto& r : output_buffer) {
                r.write(bwriter);
            }

            bwriter.flushBits();
        }

        {
            std::lock_guard<std::mutex> guard(lock);
            stats.splice_recs += local_stats.splice_recs;
            stats.hm_recs += local_stats.hm_recs;
            stats.additional_alignments += local_stats.additional_alignments;
            stats.distance += local_stats.distance;
            local_stats = CleanStatistics{};
        }

        if (ret == EOF) {
            return;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::pair<std::string, size_t>> Importer::sam_to_mgrec_phase1(format::sam::Config& options, int& chunk_id) {
    auto sam_reader = sam_to_mgrec::SamReader(options.inputFile);
    UTILS_DIE_IF(!sam_reader.isReady() || !sam_reader.isValid(), "Cannot open SAM file.");

    chunk_id = 0;
    std::mutex lock;
    std::vector<std::thread> threads;
    threads.reserve(options.num_threads);
    CleanStatistics stats;
    for (uint32_t i = 0; i < options.num_threads; ++i) {
        threads.emplace_back(
            [&]() { phase1_thread(sam_reader, chunk_id, options.tmp_dir_path, options.clean, lock, stats); });
    }
    for (auto& t : threads) {
        t.join();
    }

    if (options.clean) {
        std::cerr << "HM records dealigned: " << stats.hm_recs << std::endl;
        std::cerr << "I records split because of large mapping distance: " << stats.distance << std::endl;
        std::cerr << "Additional alignments removed: " << stats.additional_alignments << std::endl;
        std::cerr << "Records dealigned because of splices: " << stats.splice_recs << std::endl;
    }

    refs = sam_reader.getRefs();
    return refs;
}

void Importer::setup_merge(int num_chunks) {
    std::cerr << "Merging " << num_chunks << " chunks..." << std::endl;

    if (!input_ref_file.empty()) {
        for (const auto& ref : refs) {
            bool found = false;
            for (size_t j = 0; j < refinf.getMgr()->getSequences().size(); ++j) {
                if (ref.first == refinf.getMgr()->getSequences().at(j)) {
                    sam_hdr_to_fasta_lut.push_back(j);
                    found = true;
                    break;
                }
            }
            UTILS_DIE_IF(!found, "Did not find ref " + ref.first);
        }
    } else {
        for (size_t i = 0; i < refs.size(); ++i) {
            sam_hdr_to_fasta_lut.push_back(i);
        }
    }

    // std::unique_ptr<std::ostream> total_output;
    // std::ostream* out_stream = &std::cout;
    // genie::util::BitWriter total_output_writer(*out_stream);

    readers.reserve(num_chunks);

    std::string tmp_dir_path("/tmp");
    for (int i = 0; i < num_chunks; ++i) {
        readers.emplace_back(
            std::make_unique<sam_to_mgrec::SubfileReader>(tmp_dir_path + "/" + std::to_string(i) + PHASE1_EXT));
        if (!readers.back()->getRecord()) {
            auto path = readers.back()->getPath();
            std::cerr << path << " depleted" << std::endl;
            readers.pop_back();
            std::remove(path.c_str());
        } else {
            reader_prio.push(readers.back().get());
        }
    }
}

bool Importer::pumpRetrieve(core::Classifier* _classifier) {
    if (!phase1_complete) {
        format::sam::Config options;
        options.inputFile = input_sam_file;
        options.fasta_file_path = input_ref_file;
        options.clean = true;
        sam_to_mgrec_phase1(options, nref);
        phase1_complete = true;
        setup_merge(nref);  // beginning of phase 2
    }
    // rest of phase 2
    util::Watch watch;
    core::record::Chunk chunk;
    size_t size_seq = 0;
    size_t size_qual = 0;
    size_t size_name = 0;
    bool eof = false;
    {
        if (!reader_prio.empty()) {
            for (int i = 0; i < 10; ++i) {
                auto* reader = reader_prio.top();
                reader_prio.pop();
                auto rec = reader->moveRecord();
                rec.patchRefID(sam_hdr_to_fasta_lut[rec.getAlignmentSharedData().getSeqID()]);

                if (fix_ecigar(rec, refs, refinf)) {
                    chunk.getData().push_back(std::move(rec));
                } else {
                    removed_unsupported_base++;
                }

                if (reader->getRecord()) {
                    reader_prio.push(reader);
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
                    if (reader_prio.empty()) {
                        eof = true;
                        break;
                    }
                }
            }
        } else {
            eof = true;
        }
    }

    chunk.getStats().addInteger("size-sam-seq", size_seq);
    chunk.getStats().addInteger("size-sam-qual", size_qual);
    chunk.getStats().addInteger("size-sam-name", size_name);
    chunk.getStats().addInteger("size-sam-total", size_name + size_qual + size_seq);
    chunk.getStats().addDouble("time-sam-import", watch.check());
    _classifier->add(std::move(chunk));
    return !eof;
}

void sam_to_mgrec_phase2(format::sam::Config& options, int num_chunks,
                         const std::vector<std::pair<std::string, size_t>>& refs) {
    std::cerr << "Merging " << num_chunks << " chunks..." << std::endl;
    RefInfo refinf(options.fasta_file_path);
    size_t removed_unsupported_base = 0;

    std::vector<size_t> sam_hdr_to_fasta_lut;
    if (!options.no_ref) {
        for (const auto& ref : refs) {
            bool found = false;
            for (size_t j = 0; j < refinf.getMgr()->getSequences().size(); ++j) {
                if (ref.first == refinf.getMgr()->getSequences().at(j)) {
                    sam_hdr_to_fasta_lut.push_back(j);
                    found = true;
                    break;
                }
            }
            UTILS_DIE_IF(!found, "Did not find ref " + ref.first);
        }
    } else {
        for (size_t i = 0; i < refs.size(); ++i) {
            sam_hdr_to_fasta_lut.push_back(i);
        }
    }

    std::unique_ptr<std::ostream> total_output;
    std::ostream* out_stream = &std::cout;
    if (options.outputFile.substr(0, 2) != "-.") {
        total_output = std::make_unique<std::ofstream>(options.outputFile, std::ios::binary | std::ios::trunc);
        out_stream = total_output.get();
    }
    genie::util::BitWriter total_output_writer(*out_stream);

    std::vector<std::unique_ptr<sam_to_mgrec::SubfileReader>> readers;
    readers.reserve(num_chunks);
    auto cmp = [&](const sam_to_mgrec::SubfileReader* a, sam_to_mgrec::SubfileReader* b) {
        return !sam_to_mgrec::compare(a->getRecord().value(), b->getRecord().value());
    };
    std::priority_queue<sam_to_mgrec::SubfileReader*, std::vector<sam_to_mgrec::SubfileReader*>, decltype(cmp)> heap(
        cmp);
    for (int i = 0; i < num_chunks; ++i) {
        readers.emplace_back(
            std::make_unique<sam_to_mgrec::SubfileReader>(options.tmp_dir_path + "/" + std::to_string(i) + PHASE1_EXT));
        if (!readers.back()->getRecord()) {
            auto path = readers.back()->getPath();
            std::cerr << path << " depleted" << std::endl;
            readers.pop_back();
            std::remove(path.c_str());
        } else {
            heap.push(readers.back().get());
        }
    }

    if (!heap.empty()) {
        while (true) {
            auto* reader = heap.top();
            heap.pop();
            auto rec = reader->moveRecord();
            rec.patchRefID(sam_hdr_to_fasta_lut[rec.getAlignmentSharedData().getSeqID()]);

            if (fix_ecigar(rec, refs, refinf)) {
                rec.write(total_output_writer);
            } else {
                removed_unsupported_base++;
            }

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
    }

    total_output_writer.flushBits();
    out_stream->flush();

    std::cerr << "Finished merging!" << std::endl;
    std::cerr << removed_unsupported_base << " records removed because of unsupported bases." << std::endl;
}

void Importer::transcode_sam2mpg(format::sam::Config& options) {
    refs = sam_to_mgrec_phase1(options, nref);
    sam_to_mgrec_phase2(options, nref, refs);
}

}  // namespace genie::format::sam

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
