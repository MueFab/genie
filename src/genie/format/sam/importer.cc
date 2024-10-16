/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/sam/importer.h"

#include <genie/capsulator/program-options.h>
#include <genie/transcode-fastq/program-options.h>

#include <queue>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/record/class-type.h"
#include "genie/transcode-sam/utils.h"
#include "genie/util/ordered-section.h"
#include "genie/util/stop-watch.h"
#include "sam/sam_to_mgrec/sorter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::sam {

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(size_t _blockSize, std::string input, std::string ref) : blockSize(_blockSize),
input_sam_file(std::move(input)), input_ref_file(std::move(ref)) , phase1_complete(false), reader_prio(cmp_readers), refinf(input_ref_file) {}


// ---------------------------------------------------------------------------------------------------------------------

bool cmp_readers(const genieapp::transcode_sam::sam::sam_to_mgrec::SubfileReader * a,
    genieapp::transcode_sam::sam::sam_to_mgrec::SubfileReader * b) {
        return !genieapp::transcode_sam::compare(a->getRecord().value(), b->getRecord().value());
    }

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

bool fix_ecigar(genie::core::record::Record& r, const std::vector<std::pair<std::string, size_t>>&,
                    genieapp::transcode_sam::sam::sam_to_mgrec::RefInfo& ref) {
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

void Importer::setup_merge(int num_chunks) {

    std::cerr << "Merging " << num_chunks << " chunks..." << std::endl;
    removed_unsupported_base = 0;

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
    readers.reserve(num_chunks);

    for (int i = 0; i < num_chunks; ++i) {
        readers.emplace_back(
            std::make_unique<genieapp::transcode_sam::sam::sam_to_mgrec::SubfileReader>( "/tmp/" + std::to_string(i) + PHASE1_EXT));
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

bool Importer::pumpRetrieve(core::Classifier *_classifier) {
    if (!phase1_complete) {
        genieapp::transcode_sam::sam::sam_to_mgrec::Config options;
        options.inputFile = input_sam_file;
        options.fasta_file_path = input_ref_file;
        refs = genieapp::transcode_sam::sam::sam_to_mgrec::sam_to_mgrec_phase1(options, nref);
        phase1_complete = true;
        setup_merge(nref);
    }
    util::Watch watch;
    core::record::Chunk chunk;
    size_t size_seq = 0;
    size_t size_qual = 0;
    size_t size_name = 0;
    bool eof = false;
    {
        if (!reader_prio.empty()) {
            auto* reader = reader_prio.top();
            reader_prio.pop();
            auto rec = reader->moveRecord();
            rec.patchRefID(sam_hdr_to_fasta_lut[rec.getAlignmentSharedData().getSeqID()]);

            if (fix_ecigar(rec, refs, refinf)) {
                chunk.getData().push_back(std::move(rec));
                //rec.write(total_output_writer);
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
            }
        }

        /*for (size_t cur_record = 0; cur_record < blockSize; ++cur_record) {
            auto data = readData(file_list);
            if (data.empty()) {
                eof = true;
                break;
            }
            auto record = buildRecord(data);
            for (const auto &seg : record.getSegments()) {
                size_seq += seg.getSequence().size();
                for (const auto &q : seg.getQualities()) {
                    size_qual += q.size();
                }
            }
            size_name += record.getName().size() * 2;
            chunk.getData().push_back(std::move(record));
        }*/
    }



    chunk.getStats().addInteger("size-sam-seq", size_seq);
    chunk.getStats().addInteger("size-sam-qual", size_qual);
    chunk.getStats().addInteger("size-sam-name", size_name);
    chunk.getStats().addInteger("size-sam-total", size_name + size_qual + size_seq);
    chunk.getStats().addDouble("time-sam-import", watch.check());
    _classifier->add(std::move(chunk));
    return !eof;
}

// ---------------------------------------------------------------------------------------------------------------------

core::record::Record Importer::buildRecord(std::vector<std::array<std::string, LINES_PER_RECORD>> data) {
    auto ret = core::record::Record(uint8_t(data.size()), core::record::ClassType::CLASS_U,
                                    data[Files::FIRST][Lines::ID].substr(1), "", 0);

    for (auto &cur_rec : data) {
        auto seg = core::record::Segment(std::move(cur_rec[Lines::SEQUENCE]));
        if (!cur_rec[Lines::QUALITY].empty()) {
            seg.addQualities(std::move(cur_rec[Lines::QUALITY]));
        }
        ret.addSegment(std::move(seg));
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::array<std::string, Importer::LINES_PER_RECORD>> Importer::readData(
    const std::vector<std::istream *> &_file_list) {
    std::vector<std::array<std::string, LINES_PER_RECORD>> data(_file_list.size());
    for (size_t cur_file = 0; cur_file < _file_list.size(); ++cur_file) {
        for (size_t cur_line = 0; cur_line < LINES_PER_RECORD; ++cur_line) {
            if (!std::getline(*(_file_list[cur_file]), data[cur_file][cur_line])) {
                data.clear();
                return data;
            }
        }

        sanityCheck(data[cur_file]);
    }
    return data;
}

// ---------------------------------------------------------------------------------------------------------------------

void Importer::sanityCheck(const std::array<std::string, LINES_PER_RECORD> &data) {
    constexpr char ID_TOKEN = '@';
    UTILS_DIE_IF(data[Lines::ID].front() != ID_TOKEN, "Invald sam identifier");
    constexpr char RESERVED_TOKEN = '+';
    UTILS_DIE_IF(data[Lines::RESERVED].front() != RESERVED_TOKEN, "Invald sam line 3");
    UTILS_DIE_IF(data[Lines::SEQUENCE].size() != data[Lines::QUALITY].size(),
                 "Qual and Seq in sam do not match in length");
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::sam

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
