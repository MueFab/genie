#ifndef GABACIFY_INPUT_FILE_H_
#define GABACIFY_INPUT_FILE_H_


#include <string>

#include "file.h"


namespace genie {


class InputFile : public File
{
 public:
    explicit InputFile(
            const std::string& path
    );

    ~InputFile() override;

    void read(
            void *items,
            size_t itemSize,
            size_t numItems
    );
};


}  // namespace gabacify


#endif  // GABACIFY_INPUT_FILE_H_
