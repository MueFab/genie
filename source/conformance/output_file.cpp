#include "gabacify/output_file.h"

#include <cassert>
#include <string>

#include "gabacify/exceptions.h"


namespace gabacify {

OutputFile::OutputFile(
        const std::string& path
)
        : File(path, "ab")
{
    // Check if file with content existed before by opening in append mode
    // and getting size
    if(size() != 0)
        GABACIFY_DIE("Output file already exists: " + path);
    close();
    open(path, "wb");
}


OutputFile::~OutputFile() = default;


void OutputFile::write(
        void *const items,
        size_t itemSize,
        size_t numItems
){
    assert(numItems == 0 || items != nullptr);

    size_t rc = fwrite(items, itemSize, numItems, m_fp);

    if (rc != numItems)
    {
        if (feof(m_fp) != 0)
        {
            GABACIFY_DIE("Hit EOF while trying to write to file: " + m_path);
        }
        GABACIFY_DIE("fwrite to '" + m_path + "' failed");
    }
}


}  // namespace gabacify
