#include "mgrecs-exporter.h"
#include <util/ordered-section.h>

MgrecsExporter::MgrecsExporter(std::ostream *_file_1) : writer(_file_1) {
}

void MgrecsExporter::flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) {
    OrderedSection section(&lock, id);
    for (auto &i : *t) {
        i->write(&writer);
    }
}

void MgrecsExporter::dryIn() {

}