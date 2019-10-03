#include "full-local-assembly-encoder.h"

#include <iostream>

namespace lae {
    FullLocalAssemblyEncoder::FullLocalAssemblyEncoder(
            uint32_t cr_buf_max_size,
            bool _debug
    ) : refCoder(cr_buf_max_size),
        readCoder(),
        debug(_debug){

    }

    void FullLocalAssemblyEncoder::add(const util::SamRecord &rec) {
        std::string ref = refCoder.getReference(rec.pos, rec.cigar);
        refCoder.addRead(rec);
        readCoder.addRead(rec, ref);

        if(debug) {
            refCoder.printWindow();
            std::cout << "ref: " << std::endl;
            for (size_t i = 0; i < rec.pos - refCoder.getWindowBorder(); ++i) {
                std::cout << " ";
            }

            std::cout << ref << std::endl;
            std::cout << std::endl;
        }
    }

    void FullLocalAssemblyEncoder::addPair(const util::SamRecord &rec1, const util::SamRecord &rec2) {
        std::string ref1 = refCoder.getReference(rec1.pos, rec1.cigar);
        std::string ref2 = refCoder.getReference(rec2.pos, rec2.cigar);
        refCoder.addRead(rec1);
        refCoder.addRead(rec2);
        readCoder.addPair(rec1, ref1, rec2, ref2);

        if(debug) {
            refCoder.printWindow();
            std::cout << "pair!" << std::endl;
            std::cout << "ref1: " << std::endl;
            for (size_t i = 0; i < rec1.pos - refCoder.getWindowBorder(); ++i) {
                std::cout << " ";
            }
            std::cout << ref1 << std::endl;

            std::cout << "ref2: " << std::endl;
            for (size_t i = 0; i < rec2.pos - refCoder.getWindowBorder(); ++i) {
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