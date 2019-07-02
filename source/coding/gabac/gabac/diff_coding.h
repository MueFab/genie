/**
 * @file
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_DIFF_CODING_H_
#define GABAC_DIFF_CODING_H_

namespace gabac {

class DataBlock;

void transformDiffCoding(
        DataBlock *transformedSymbols
);


void inverseTransformDiffCoding(
        DataBlock *transformedSymbols
);


}  // namespace gabac


#endif  // GABAC_DIFF_CODING_H_
