#ifndef GENIE_FORMAT_EXPORTER_H
#define GENIE_FORMAT_EXPORTER_H

class FormatExport {
public:
    ~FormatExport() = default;
    virtual void exportBlock(std::vector<MpeggRecord>* vec) = 0;
};

#endif //GENIE_FORMAT_EXPORTER_H
