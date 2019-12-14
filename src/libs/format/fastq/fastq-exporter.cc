#include "fastq-exporter.h"


FastqExporter::FastqExporter(std::ostream *_file_1) : file{_file_1, nullptr} {

}

FastqExporter::FastqExporter(std::ostream *_file_1, std::ostream *_file_2) : file{_file_1, _file_2} {

}

void FastqExporter::flowIn(std::unique_ptr<format::mpegg_rec::MpeggChunk> t, size_t id) {
    (void) id;
    for (auto &i : *t) {
        for (size_t j = 0; j < i->getNumberOfRecords(); ++j) {
            file[j]->write("@", 1);
            file[j]->write(i->getReadName().c_str(), i->getReadName().length());
            file[j]->write("\n", 1);
            file[j]->write(i->getRecordSegment(j)->getSequence()->c_str(),
                           i->getRecordSegment(j)->getSequence()->length());
            file[j]->write("\n", 1);
            file[j]->write("+", 1);
            file[j]->write("\n", 1);
            if (i->getRecordSegment(j)->getQvDepth()) {
                file[j]->write(i->getRecordSegment(j)->getQuality(0)->c_str(),
                               i->getRecordSegment(j)->getQuality(0)->length());
            } else {
                std::string qual(i->getRecordSegment(j)->getSequence()->length(), '#');
                file[j]->write(qual.c_str(),
                               qual.length());
            }
            file[j]->write("\n", 1);
        }
    }
}

void FastqExporter::dryIn() {

}