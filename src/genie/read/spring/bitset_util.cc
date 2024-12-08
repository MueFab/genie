/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/spring/bitset_util.h"

#include "genie/read/spring/params.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

// -----------------------------------------------------------------------------
void BbHashDict::FindPos(int64_t* dict_idx,
                         const uint64_t& start_pos_idx) const {
  dict_idx[0] = start_pos_[start_pos_idx];
  if (const auto end_idx = start_pos_[start_pos_idx + 1];
      read_id_[end_idx - 1] ==
      kMax_Num_Reads)  // means exactly one read has been removed
    dict_idx[1] = end_idx - 1;
  else if (read_id_[end_idx - 1] ==
           kMax_Num_Reads + 1)  // means two or more reads have
    // been removed (in this case
    // second last entry stores the
    // number of reads left)
    dict_idx[1] = dict_idx[0] + read_id_[end_idx - 2];
  else
    dict_idx[1] = end_idx;
}

// -----------------------------------------------------------------------------
void BbHashDict::Remove(const int64_t* dict_idx, const uint64_t& start_pos_idx,
                        const int64_t current) {
  const auto size = dict_idx[1] - dict_idx[0];
  if (size == 1) {  // just one read left in bin
    empty_bin_[start_pos_idx] = true;
    return;  // need to keep one read to check during matching
  }
  const int64_t pos =
      std::lower_bound(read_id_.begin() + dict_idx[0],
                       read_id_.begin() + dict_idx[1], current) -
      (read_id_.begin() + dict_idx[0]);

  for (int64_t i = dict_idx[0] + pos; i < dict_idx[1] - 1; i++)
    read_id_[i] = read_id_[i + 1];
  if (const auto end_idx = start_pos_[start_pos_idx + 1];
      dict_idx[1] == end_idx) {  // this is first read to be deleted
    read_id_[end_idx - 1] = kMax_Num_Reads;
  } else if (read_id_[end_idx - 1] ==
             kMax_Num_Reads) {  // exactly one read has been deleted till now
    read_id_[end_idx - 1] = kMax_Num_Reads + 1;
    read_id_[end_idx - 2] =
        static_cast<uint32_t>(size - 1);  // number of reads left in bin
  } else {  // more than two reads have been deleted
    read_id_[end_idx - 2]--;
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
