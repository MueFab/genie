/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "contact_coder.h"
#include "contact_parameters.h"
#include "contact_subcm_parameters.h"
#include <genie/core/record/contact/record.h>
#include <genie/util/runtime-exception.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

void decompose(
    const EncodingOptions& opt,
    EncodingBlock& block,
    std::vector<core::record::ContactRecord>& recs
){
    UTILS_DIE_IF(recs.empty(), "No records found for the process!");

    auto& params = block.params;

    bool sample_parsed = false;
    bool interv_parsed = false;
//    std::vector<uint32_t> intervals;

    for (auto& rec: recs){
        auto sample_ID = rec.getSampleID();
        auto sample_name = rec.getSampleName();

        // TODO (Yeremia): Extend to multi-samples
        if (sample_parsed) {
            auto& samples = params.getSamples();
            UTILS_DIE_IF(samples.find(sample_ID) != samples.end(), "sample_ID already exists!");
        } else {
            params.addSample(sample_ID, std::move(sample_name));
            sample_parsed = true;
        }

        // TODO (Yeremia): Extend to multi-intervals
        if (interv_parsed){
            if (opt.multi_intervals){
                UTILS_DIE("Not implemented error!");
            } else {
                UTILS_DIE_IF(params.getInterval() != rec.inferInterval(), "Interval differs");
            }
        } else {
            params.setInterval(rec.inferInterval());
        }

        params.upsertChromosome(
            rec.getChr1ID(),
            rec.getChr1Name(),
            rec.inferChr1Length()
        );

        params.upsertChromosome(
            rec.getChr2ID(),
            rec.getChr2Name(),
            rec.inferChr2Length()
        );

        // TODO (Yeremia): Extend to norm_methods and norm_mat
    }

}

// ---------------------------------------------------------------------------------------------------------------------

}
}

// ---------------------------------------------------------------------------------------------------------------------