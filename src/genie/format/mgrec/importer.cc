/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgrec/importer.h"
#include <string>
#include <utility>
#include "genie/util/ordered-section.h"
#include "genie/util/watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgrec {

// ---------------------------------------------------------------------------------------------------------------------

Importer::Importer(size_t _blockSize, std::istream& _file_1)
    : blockSize(_blockSize), reader(_file_1), bufferedRecord(boost::none) {}

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

bool isRecordSupported(const core::record::Record& rec) {
    for (const auto& a : rec.getAlignments()) {
        if (!isECigarSupported(a.getAlignment().getECigar())) {
            return false;
        }
        for (const auto& s : a.getAlignmentSplits()) {
            if (s->getType() == core::record::AlignmentSplit::Type::SAME_REC) {
                if (!isECigarSupported(
                        dynamic_cast<core::record::alignment_split::SameRec&>(*s).getAlignment().getECigar())) {
                    return false;
                }
                // Splits with more than 32767 delta must be encoded in separate records, which is not yet supported
                if (std::abs(dynamic_cast<core::record::alignment_split::SameRec&>(*s).getDelta()) > 32767) {
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
            }
        }
    }

    chunk.getStats().addDouble("time-mgrec-import", watch.check());
    _classifier->add(std::move(chunk));
    return reader.isGood() || bufferedRecord;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgrec
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
