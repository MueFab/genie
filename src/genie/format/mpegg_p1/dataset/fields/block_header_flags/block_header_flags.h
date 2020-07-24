#ifndef GENIE_PART1_DATASET_HEADER_BLOCK_HEADER_H
#define GENIE_PART1_DATASET_HEADER_BLOCK_HEADER_H

#include <memory>

#include "genie/util/bitwriter.h"
#include "class_info/class_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class BlockHeader {
   private:

   public:
    BlockHeader();

    BlockHeader(bool _block_header_flag, bool _mit_flag);

    /*
     *
     */
    bool getBlockHeaderFlag() const;

    /*
     *
     */
    bool getMITFlag() const;

    /*
     *
     */
    uint8_t getNumClasses() const;

    void addClassInfo(ClassInfo&& _cls_info);

    void addClassInfos(std::vector<ClassInfo>& _cls_infos);

    void setClassInfos(std::vector<ClassInfo>&& _cls_infos);

    virtual void write(genie::util::BitWriter& bit_writer) const;

    void writeClassInfos(genie::util::BitWriter& bit_writer) const;

   protected:
    bool block_header_flag;
    bool MIT_flag;

    std::vector<ClassInfo> class_infos;
};


}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DATASET_HEADER_BLOCK_HEADER_H
