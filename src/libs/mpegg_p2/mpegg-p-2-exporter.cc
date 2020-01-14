#include "mpegg-p-2-exporter.h"

namespace genie {
namespace mpegg_p2 {
    MpeggP2Exporter::MpeggP2Exporter(std::ostream* _file) : writer(_file) {}
    void MpeggP2Exporter::flowIn(genie::BlockPayloadSet&& t, size_t id) {
        genie::BlockPayloadSet data = std::move(t);
        util::OrderedSection section(&lock, id);
        data.getParameters().write(writer);

        mpegg_p2::AccessUnit au(id, id, genie::mpegg_rec::ClassType::CLASS_I, data.getRecordNum(),
                                mpegg_p2::DataUnit::DatasetType::ALIGNED, 32, 32, 0);
        au.setAuTypeCfg(util::make_unique<AuTypeCfg>(data.getReference(), data.getMinPos(), data.getMaxPos(),
                                                     data.getParameters().getPosSize()));
        for (size_t descriptor = 0; descriptor < genie::getDescriptors().size(); ++descriptor) {
            if (data.getPayload(genie::GenDesc(descriptor)).isEmpty()) {
                continue;
            }
            au.addBlock(Block(descriptor, data.movePayload(descriptor)));
        }
        au.write(writer);
    }
    void MpeggP2Exporter::dryIn() {}
}  // namespace mpegg_p2
}  // namespace genie