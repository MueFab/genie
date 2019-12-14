#include "mgrecs-importer.h"


MgrecsImporter::MgrecsImporter(size_t _blockSize, std::istream *_file_1) : blockSize(_blockSize), reader(_file_1),
                                                                           record_counter(0) {

}

bool MgrecsImporter::pump() {
    auto chunk = util::make_unique<format::mpegg_rec::MpeggChunk>();
    for (size_t i = 0; i < blockSize; ++i) {
        chunk->push_back(util::make_unique<format::mpegg_rec::MpeggRecord>(&reader));
    }
    flowOut(std::move(chunk), record_counter++);

    if (!reader.isGood()) {
        dryOut();
        return false;
    }
    return true;
}
