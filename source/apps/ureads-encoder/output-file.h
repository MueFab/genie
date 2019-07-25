#ifndef GABACIFY_OUTPUT_FILE_H_
#define GABACIFY_OUTPUT_FILE_H_


#include <string>
#include <vector>
#include <iostream>

#include "file.h"

namespace genie {


class OutputFile : public File
{
 public:
    explicit OutputFile(
            const std::string& path
    );

    ~OutputFile() override;

    void write(
            void *items,
            size_t itemSize,
            size_t numItems
    );
};

}  // namespace gabacify


#endif  // GABACIFY_OUTPUT_FILE_H_
