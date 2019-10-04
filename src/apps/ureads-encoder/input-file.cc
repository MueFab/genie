#include "input-file.h"

#include <cassert>
#include <string>

#include "util/exceptions.h"

namespace genie {

InputFile::InputFile(const std::string& path) : File(path, "rb") {
    // Nothing to do here
}

InputFile::~InputFile() = default;

void InputFile::read(void* items, size_t itemSize, size_t numItems) {
    assert(numItems == 0 || items != nullptr);

    size_t rc = fread(items, itemSize, numItems, m_fp);

    if (rc != numItems) {
        if (feof(m_fp) != 0) {
            UTILS_DIE("Hit EOF while trying to read from file");
        }
        UTILS_DIE("fread from '" + m_path + "' failed");
    }
}

}  // namespace genie
