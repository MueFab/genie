/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "spring-encoder.h"
#include <genie/quality/paramqv1/qv_coding_config_1.h>
#include <genie/util/thread-manager.h>
#include <cmath>
#include <utility>
#include "generate-read-streams.h"
#include "reorder-compress-quality-id.h"

namespace genie {
namespace read {
namespace spring {

void SpringEncoder::flowIn(core::record::Chunk&& t, const util::Section& id) { preprocessor.preprocess(std::move(t), id.start); }

class SpringSource : public util::OriginalSource, public util::Source<core::AccessUnitRaw> {
   private:
    se_data data;
    pe_block_data bdata;
    compression_params cp;
    std::string temp_dir;
    core::stats::FastqStats* stats;
    uint32_t num_AUs;
    util::SideSelector<genie::core::QVEncoder, genie::core::QVEncoder::QVCoded, const genie::core::record::Chunk&>* coder;
    util::SideSelector<genie::core::NameEncoder, genie::core::AccessUnitRaw::Descriptor,
        const genie::core::record::Chunk&>* ncoder;

   public:
    SpringSource(util::SideSelector<genie::core::QVEncoder, genie::core::QVEncoder::QVCoded, const genie::core::record::Chunk&>* _coder, util::SideSelector<genie::core::NameEncoder, genie::core::AccessUnitRaw::Descriptor,
        const genie::core::record::Chunk&>* _ncoder, const compression_params& _cp, std::string  _temp_dir, core::stats::FastqStats* _stats)
        : cp(_cp), temp_dir(std::move(_temp_dir)), stats(_stats), coder(_coder), ncoder(_ncoder) {
        if (cp.paired_end) {
            loadPE_Data(cp, temp_dir, true, &data);
            generateBlocksPE(data, &bdata);
            data.order_arr.clear();
        } else {
            loadSE_Data(cp, temp_dir, &data);
        }

        num_AUs = std::ceil(float(cp.num_reads) / cp.num_reads_per_block);
    }

    bool pump(size_t& id) override {
        core::AccessUnitRaw au(core::parameter::ParameterSet(), 0);
        if (cp.paired_end) {
            au = generate_read_streams_pe(data, bdata, id);
        } else {
            au = generate_read_streams_se(data, id);
        }

        core::record::Chunk chunk;
        if(coder || ncoder) {
            std::ifstream qvfile(temp_dir + "/quality_streams." + std::to_string(id));
            std::ifstream namefile(temp_dir + "/id_streams." + std::to_string(id));
            for(size_t i = 0; i < au.getNumRecords(); ++i) {
                std::string name;
                if(ncoder) {
                    UTILS_DIE_IF(!std::getline(namefile, name), "Name file too short");
                }
                chunk.emplace_back((uint8_t )cp.paired_end + 1, core::record::ClassType::CLASS_U, std::move(name),
                    "", 0);
                if(coder) {
                    for(size_t j = 0; j < (uint8_t )cp.paired_end + 1u; ++j) {
                        std::string qv;
                        UTILS_DIE_IF(!std::getline(qvfile, qv), "Qv file too short");
                        core::record::Segment s(std::string(qv.length(), 'N'));
                        s.addQualities(std::move(qv));
                        chunk.back().addSegment(std::move(s));
                    }
                }
            }
        }

        if(ncoder) {
            au.get(core::GenDesc::RNAME) = this->ncoder->process(chunk);
        }

        if(coder) {
            auto qv_coded = this->coder->process(chunk);
            au.get(core::GenDesc::QV) = qv_coded.second;
            au.getParameters().addClass(genie::core::record::ClassType::CLASS_U, std::move(qv_coded.first));
        } else {
            au.getParameters().addClass(genie::core::record::ClassType::CLASS_U,
                                        util::make_unique<genie::quality::paramqv1::QualityValues1>(
                                            genie::quality::paramqv1::QualityValues1::QvpsPresetId::ASCII, false));
        }


        flowOut(std::move(au), util::Section{id, 0, false});
        return id != num_AUs - 1 ;
    }

    /**
     * @brief Signal end of data.
     */
    virtual void dryIn() override { dryOut(); }
};

void SpringEncoder::dryIn() {
    return;
    preprocessor.finish();

    std::cout << "Reordering ...\n";
    auto reorder_start = std::chrono::steady_clock::now();
    call_reorder(preprocessor.temp_dir, preprocessor.cp);
    auto reorder_end = std::chrono::steady_clock::now();
    std::cout << "Reordering done!\n";
    std::cout << "Time for this step: "
              << std::chrono::duration_cast<std::chrono::seconds>(reorder_end - reorder_start).count() << " s\n";

    std::cout << "Encoding ...\n";
    auto encoder_start = std::chrono::steady_clock::now();
    call_encoder(preprocessor.temp_dir, preprocessor.cp);
    auto encoder_end = std::chrono::steady_clock::now();
    std::cout << "Encoding done!\n";
    std::cout << "Time for this step: "
              << std::chrono::duration_cast<std::chrono::seconds>(encoder_end - encoder_start).count() << " s\n";

    if (preprocessor.cp.preserve_quality || preprocessor.cp.preserve_id) {
        std::cout << "Reordering and compressing quality and/or ids ...\n";
        auto rcqi_start = std::chrono::steady_clock::now();
        reorder_compress_quality_id(preprocessor.temp_dir, preprocessor.cp, stats);
        auto rcqi_end = std::chrono::steady_clock::now();
        std::cout << "Reordering and compressing quality and/or ids done!\n";
        std::cout << "Time for this step: "
                  << std::chrono::duration_cast<std::chrono::seconds>(rcqi_end - rcqi_start).count() << " s\n";
    }

    SpringSource src(this->qvcoder, this->namecoder,this->preprocessor.cp, this->preprocessor.temp_dir, this->stats);
    src.setDrain(this->drain);
    std::vector<util::OriginalSource*> srcVec = {&src};
    util::ThreadManager mgr(preprocessor.cp.num_thr);
    mgr.setSource(srcVec);
    mgr.run();
}
}  // namespace spring
}  // namespace read
}  // namespace genie
