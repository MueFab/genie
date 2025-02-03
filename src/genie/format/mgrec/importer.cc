/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgrec/importer.h"
#include <iostream>
#include <string>
#include <utility>
#include "genie/util/ordered_section.h"
#include "genie/util/stop_watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgrec {

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(size_t _blockSize, std::istream& _file_1, std::ostream& _unsupported, bool _checkSupport)
    : blockSize(_blockSize),
      reader(_file_1),
      writer(&_unsupported),
      bufferedRecord(boost::none),
      checkSupport(_checkSupport) {}

// ---------------------------------------------------------------------------------------------------------------------

bool isECigarSupported(const std::string& ecigar) {
    // Splices not supported
    if ((ecigar.find_first_of('*') != std::string::npos) || (ecigar.find_first_of('/') != std::string::npos) ||
        (ecigar.find_first_of('%') != std::string::npos)) {
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

bool Importer::isRecordSupported(const core::record::Record& rec) {
    if (!checkSupport) {
        return true;
    }
   /* if (rec.getClassID() == genie::core::record::ClassType::CLASS_U &&
        rec.getSegments().size() != rec.getNumberOfTemplateSegments()) {
        discarded_missing_pair_U++;
        return false;
    }*/
    for (const auto& a : rec.getAlignments()) {
        if (rec.getClassID() == genie::core::record::ClassType::CLASS_HM) {
            discarded_HM++;
            return false;
        }
        if (!isECigarSupported(a.getAlignment().getECigar())) {
            discarded_splices++;
            return false;
        }
        for (const auto& s : a.getAlignmentSplits()) {
            if (s->getType() == core::record::AlignmentSplit::Type::SAME_REC) {
                if (!isECigarSupported(
                        dynamic_cast<core::record::alignment_split::SameRec&>(*s).getAlignment().getECigar())) {
                    discarded_splices++;
                    return false;
                }
                // Splits with more than 32767 delta must be encoded in separate records, which is not yet supported
                if (std::abs(dynamic_cast<core::record::alignment_split::SameRec&>(*s).getDelta()) > 32767) {
                    discarded_long_distance++;
                    return false;
                }
            }
        }
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

bool Importer::pumpRetrieve(core::Classifier* _classifier) {
    util::Watch watch;
    core::record::Chunk chunk;
    bool seqid_valid = false;
    for (size_t i = 0; i < blockSize; ++i) {
        if (bufferedRecord) {
            chunk.setRefID(bufferedRecord->getAlignmentSharedData().getSeqID());
            seqid_valid = true;
            if (isRecordSupported(bufferedRecord.get())) {
                chunk.getData().emplace_back(std::move(bufferedRecord.get()));
            } else {
                bufferedRecord.get().write(writer);
            }
            bufferedRecord = boost::none;
            continue;
        }
        core::record::Record rec(reader);
        if (!reader.isGood()) {
            break;
        }

        if (!seqid_valid) {
            chunk.setRefID(rec.getAlignmentSharedData().getSeqID());
            seqid_valid = true;
        }

        if (chunk.getRefID() != rec.getAlignmentSharedData().getSeqID()) {
            bufferedRecord = std::move(rec);
            break;
        } else {
            if (isRecordSupported(rec)) {
                chunk.getData().emplace_back(std::move(rec));
            } else {
                rec.write(writer);
            }
        }
    }

    chunk.getStats().addDouble("time-mgrec-import", watch.check());
    for (const auto& c : chunk.getData()) {
        missing_additional_alignments += c.getAlignments().empty() ? 0 : c.getAlignments().size() - 1;
    }
    _classifier->add(std::move(chunk));
    return reader.isGood() || bufferedRecord;
}

// ---------------------------------------------------------------------------------------------------------------------

void Importer::printStats() const {
    std::cerr << std::endl << "The following number of reads were dropped:" << std::endl;
    std::cerr << discarded_splices << " containing splices" << std::endl;
    std::cerr << discarded_HM << " class HM reads" << std::endl;
    std::cerr << discarded_long_distance << " aligned, paired reads with mapping distance too big" << std::endl;
    std::cerr << discarded_missing_pair_U << " unaligned reads with missing pair" << std::endl;
    std::cerr << discarded_splices + discarded_HM + discarded_long_distance + discarded_missing_pair_U << " in total"
              << std::endl;
    std::cerr << std::endl << missing_additional_alignments << " additional alignments were dropped" << std::endl;
}

// ---------------------------------------------------------------------------------------------------------------------

void Importer::FlushIn(uint64_t& pos) {
    FormatImporter::flushIn(pos);
    printStats();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgrec
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
