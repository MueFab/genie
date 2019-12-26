#include "mgrecs-importer.h"
#include <util/ordered-section.h>

MgrecsImporter::MgrecsImporter(size_t _blockSize, std::istream *_file_1)
    : blockSize(_blockSize), reader(_file_1), record_counter(0) {}

bool MgrecsImporter::pump(size_t id) {
    auto chunk = util::make_unique<format::mpegg_rec::MpeggChunk>();
    {
        OrderedSection section(&lock, id);
        for (size_t i = 0; i < blockSize; ++i) {
            chunk->push_back(util::make_unique<format::mpegg_rec::MpeggRecord>(&reader));
        }
    }
    flowOut(std::move(chunk), record_counter++);

    return reader.isGood();
}
void MgrecsImporter::dryIn() { dryOut(); }
