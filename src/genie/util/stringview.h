/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */


#ifndef GENIE_STRINGVIEW_H
#define GENIE_STRINGVIEW_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

struct StringView {
   private:
    size_t start;
    size_t stop;
    const char* memory;

   public:
    StringView(size_t start_p, size_t stop_p);
    StringView(size_t start_p, size_t stop_p, const char* mem);
    StringView deploy(const char* mem) const;
    const char* begin() const;
    const char* end() const;
    size_t length() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}
}

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_STRINGVIEW_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------