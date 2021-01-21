#ifndef GENIE_PART1_DATASET_HEADER_BLOCK_HEADER_H
#define GENIE_PART1_DATASET_HEADER_BLOCK_HEADER_H

#include <memory>

#include "genie/util/bitwriter.h"
#include "class_info/class_info.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

class BlockConfig {
   private:

   public:
    BlockConfig();

    BlockConfig(bool _block_header_flag, bool _mit_flag);

    /**
     *
     * @return
     */
    bool getBlockHeaderFlag() const;

    /**
     *
     * @return
     */
    bool getMITFlag() const;

    /**
     *
     * @return
     */
    uint8_t getNumClasses() const;

    /**
     *
     * @return
     */
    const std::vector<ClassInfo>& getClassInfos() const;

    /**
     *
     * @param _cls_info
     */
    void addClassInfo(ClassInfo&& _cls_info);

    /**
     *
     * @param _cls_infos
     */
    void addClassInfos(std::vector<ClassInfo>& _cls_infos);

    /**
     *
     * @param _cls_infos
     */
    void setClassInfos(std::vector<ClassInfo>&& _cls_infos);

    /**
     *
     * @return
     */
    virtual uint64_t getBitLength() const;

    /**
     *
     * @param bit_writer
     */
    virtual void write(genie::util::BitWriter& bit_writer) const;

    /**
     *
     * @param bit_writer
     */
    void writeClassInfos(genie::util::BitWriter& bit_writer) const;

   protected:
    bool block_header_flag;
    bool MIT_flag;
    //bool CC_mode_flag;
    //bool ordered_blocks_flag;

    std::vector<ClassInfo> class_infos;
};


}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_PART1_DATASET_HEADER_BLOCK_HEADER_H
