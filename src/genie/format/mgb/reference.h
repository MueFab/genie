#ifndef GENIE_MGB_REFERENCE_H
#define GENIE_MGB_REFERENCE_H

#include <reference.h>
#include <string>
namespace genie {
namespace format {
namespace mgb {


class Importer;

/**
 *
 */
class Reference : public core::Reference {
   private:
        Importer* mgr;
        size_t f_pos;
        bool raw;
   public:
    /**
     *
     * @param name
     * @param length
     * @param m
     */
    Reference(const std::string& _name, size_t _start, size_t _end, Importer* _mgr, size_t filepos, bool _raw)
        : core::Reference(_name,_start,_end), mgr(_mgr), f_pos(filepos), raw(_raw) {

    }

    /**
     *
     * @param _start
     * @param _end
     * @return
     */
    std::string getSequence(uint64_t _start, uint64_t _end) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace fasta
}  // namespace format
}  // namespace genie

#endif  // GENIE_REFERENCE_H
