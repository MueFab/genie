/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FASTQ_EXPORTER_H
#define GENIE_FASTQ_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------

#include <format/mpegg_rec/alignment-container.h>
#include <format/mpegg_rec/external-alignment.h>
#include <format/mpegg_rec/meta-alignment.h>
#include <format/mpegg_rec/segment.h>
#include <util/drain.h>
#include <util/make_unique.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace fastq {
/**
 *
 */
class FastqExporter : public Drain<std::unique_ptr<format::mpegg_rec::MpeggChunk>> {
    std::vector<std::ostream *> file;  //!<

   public:
    /**
     *
     * @param _file_1
     */
    explicit FastqExporter(std::ostream *_file_1);

    /**
     *
     * @param _file_1
     * @param _file_2
     */
    FastqExporter(std::ostream *_file_1, std::ostream *_file_2);

    /**
     *
     * @param t
     * @param id
     */
    void flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) override;

    /**
     *
     */
    void dryIn() override;
};

}  // namespace fastq
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FASTQ_EXPORTER_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------