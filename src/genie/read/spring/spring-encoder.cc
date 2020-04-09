/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "spring-encoder.h"
#include "reorder-compress-quality-id.h"

namespace genie {
namespace read {
namespace spring {

void SpringEncoder::flowIn(core::record::Chunk &&t, size_t id) {
    preprocessor.preprocess(std::move(t), id);
}

void SpringEncoder::dryIn() {
    preprocessor.finish();

    util::FastqStats stats;

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
        reorder_compress_quality_id(preprocessor.temp_dir, preprocessor.cp, &stats);
        auto rcqi_end = std::chrono::steady_clock::now();
        std::cout << "Reordering and compressing quality and/or ids done!\n";
        std::cout << "Time for this step: "
                  <<
                  std::chrono::duration_cast<std::chrono::seconds>(rcqi_end -
                                                                   rcqi_start).count() << " s\n";
    }


    dryOut();
}
}  // namespace spring
}  // namespace read_coding
}  // namespace genie