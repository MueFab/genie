#ifndef GENIE_FILE_HEADER_H
#define GENIE_FILE_HEADER_H
#include <vector>
namespace genie {
namespace format {
namespace mpegg_p1 {

class FileHeader {
   public:

   private:
    /**
    * ISO 23092-1 Section 6.6.2 table 30
    *
    * ------------------------------------------------------------------------------------------------------------- */
    const char * const major_brand = "MPEG-G";
    const char * const minor_brand = "??00"; //TODO: should be: "<version number, as the last two digits of the year of release of the major brand>00"
    std::vector<char[4]> compatible_brand;
    
};

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

#endif  // GENIE_FILE_HEADER_H
