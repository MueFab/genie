#include "output-file.h"

#include <cassert>
#include <string>

#include "util/exceptions.h"

namespace genie {

OutputFile::OutputFile(const std::string& path) : File(path, "ab") {
    // Check if file with content existed before by opening in append mode
    // and getting size
    if (size() != 0) UTILS_DIE("Output file already exists: " + path);
    close();
    open(path, "wb");
}

OutputFile::~OutputFile() = default;

void OutputFile::write(void* const items, size_t itemSize, size_t numItems) {
    assert(numItems == 0 || items != nullptr);

    size_t rc = fwrite(items, itemSize, numItems, m_fp);

    if (rc != numItems) {
        if (feof(m_fp) != 0) {
            UTILS_DIE("Hit EOF while trying to write to file: " + m_path);
        }
        UTILS_DIE("fwrite to '" + m_path + "' failed");
    }
}

}  // namespace genie
