#include "full-local-assembly-encoder.h"

#include <iostream>

#include <format/mpegg_rec/alignment-container.h>

namespace lae {
    FullLocalAssemblyEncoder::FullLocalAssemblyEncoder(
            uint32_t cr_buf_max_size,
            bool _debug
    ) : refCoder(cr_buf_max_size),
        readCoder(),
        debug(_debug){

    }

    void FullLocalAssemblyEncoder::add(const format::mpegg_rec::MpeggRecord *rec) {

        const format::mpegg_rec::SplitAlignmentSameRec* srec;
        if(rec->getNumberOfRecords() > 1) {
            if (rec->getAlignment(0)->getSplitAlignment(0)->getType() ==
                format::mpegg_rec::SplitAlignment::SplitAlignmentType::SAME_REC) {
                srec = dynamic_cast<const format::mpegg_rec::SplitAlignmentSameRec *>(rec->getAlignment(
                        0)->getSplitAlignment(0));
            } else {
                UTILS_DIE("Only same record split alignments supported");
            }
        }

        std::string ref1 = refCoder.getReference(rec->getAlignment(0)->getPosition(), *rec->getAlignment(0)->getAlignment()->getECigar());
        std::string ref2;
        if(rec->getNumberOfRecords() > 1) {
            ref2 = refCoder.getReference(rec->getAlignment(0)->getPosition() + srec->getDelta(), *srec->getAlignment()->getECigar());
        }
        refCoder.addRead(rec);

        if(rec->getNumberOfRecords() > 1) {
            readCoder.add(rec, ref1, ref2);
        } else {
            readCoder.add(rec, ref1);
        }

        if(debug) {
            refCoder.printWindow();
            std::cout << "pair!" << std::endl;
            std::cout << "ref1: " << std::endl;
            for (size_t i = 0; i < rec->getAlignment(0)->getPosition() - refCoder.getWindowBorder(); ++i) {
                std::cout << " ";
            }
            std::cout << ref1 << std::endl;

            std::cout << "ref2: " << std::endl;
            for (size_t i = 0; i < rec->getAlignment(0)->getPosition() - refCoder.getWindowBorder(); ++i) {
                std::cout << " ";
            }
            std::cout << ref2 << std::endl;
            std::cout << std::endl;
        }
    }

    std::unique_ptr<StreamContainer> FullLocalAssemblyEncoder::pollStreams() {
        return readCoder.pollStreams();
    }
}