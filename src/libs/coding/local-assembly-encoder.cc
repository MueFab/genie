#include "local-assembly-encoder.h"

#include <util/make_unique.h>

#include <format/mpegg_p2/parameter_set/qv_coding_config_1/qv_coding_config_1.h>
#include <format/mpegg_rec/alignment-container.h>
#include <format/mpegg_rec/segment.h>
#include <format/mpegg_rec/split_alignment/split-alignment-same-rec.h>

void LocalAssemblyEncoder::flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) {
    refCoder = lae::LocalAssemblyReferenceEncoder(cr_buf_max_size);

    bool paired_end = t->front()->getNumberOfTemplateSegments() > 1;
    size_t read_length = t->front()->getRecordSegment(0)->getLength();
    auto classType = format::mpegg_rec::MpeggRecord::ClassType::NONE;

    for (const auto &r : *t) {
        const format::mpegg_rec::SplitAlignmentSameRec *srec;

        if ((r->getNumberOfTemplateSegments() > 1) != paired_end) {
            UTILS_DIE("Mix of paired / unpaired not supported");
        }

        classType = std::max(classType, r->getClassID());

        if (r->getRecordSegment(0)->getLength() != read_length) {
            read_length = 0;
        }

        if (r->getNumberOfRecords() > 1) {
            if (r->getAlignment(0)->getSplitAlignment(0)->getType() ==
                format::mpegg_rec::SplitAlignment::SplitAlignmentType::SAME_REC) {
                srec = dynamic_cast<const format::mpegg_rec::SplitAlignmentSameRec *>(
                    r->getAlignment(0)->getSplitAlignment(0));
            } else {
                UTILS_DIE("Only same record split alignments supported");
            }

            if (r->getRecordSegment(1)->getLength() != read_length) {
                read_length = 0;
            }
        }

        std::string ref1 =
            refCoder.getReference(r->getAlignment(0)->getPosition(), *r->getAlignment(0)->getAlignment()->getECigar());
        std::string ref2;
        if (r->getNumberOfRecords() > 1) {
            ref2 = refCoder.getReference(r->getAlignment(0)->getPosition() + srec->getDelta(),
                                         *srec->getAlignment()->getECigar());
        }

        refCoder.addRead(r.get());
        readCoder.add(r.get(), ref1, ref2);

        if (debug) {
            refCoder.printWindow();
            std::cout << "pair!" << std::endl;
            std::cout << "ref1: " << std::endl;
            for (size_t i = 0; i < r->getAlignment(0)->getPosition() - refCoder.getWindowBorder(); ++i) {
                std::cout << " ";
            }
            std::cout << ref1 << std::endl;

            std::cout << "ref2: " << std::endl;
            for (size_t i = 0; i < r->getAlignment(0)->getPosition() - refCoder.getWindowBorder(); ++i) {
                std::cout << " ";
            }
            std::cout << ref2 << std::endl;
            std::cout << std::endl;
        }
    }

    format::mpegg_p2::DataUnit::DatasetType dataType = format::mpegg_p2::DataUnit::DatasetType::ALIGNED;
    auto ret = util::make_unique<format::mpegg_p2::ParameterSet>(id, id, dataType, AlphabetID::ACGTN, read_length,
                                                                 paired_end, false, false, 0, false, false);
    ret->addClass(classType, util::make_unique<format::mpegg_p2::qv_coding1::QvCodingConfig1>(
                                 format::mpegg_p2::qv_coding1::QvCodingConfig1::QvpsPresetId::ASCII, false));

    auto rawAU = readCoder.pollStreams();

    rawAU->setParameters(std::move(ret));

    flowOut(std::move(rawAU), id);
}

void LocalAssemblyEncoder::dryIn() {
    // Output data left over
    dryOut();
}

LocalAssemblyEncoder::LocalAssemblyEncoder(uint32_t _cr_buf_max_size, bool _debug)
    : refCoder(_cr_buf_max_size), readCoder(), debug(_debug), cr_buf_max_size(_cr_buf_max_size) {}