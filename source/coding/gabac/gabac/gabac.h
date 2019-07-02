/**
 * @file
 * @brief Main header. Whenever using gabac you should include _just_ this file.
 * @authors The GABAC authors. TODO: Include names
 * @version 2.0.0
 * @date 2018-2019
 * @copyright This file is part of the GABAC encoder. See LICENCE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_GABAC_H_
#define GABAC_GABAC_H_

#ifdef __cplusplus

/* General */
#include "constants.h"
#include "exceptions.h"

/* Encode / Decode */
#include "analysis.h"
#include "configuration.h"
#include "run.h"

/* io */
#include "block_stepper.h"
#include "data_block.h"
#include "streams.h"

#endif  /* __cplusplus */

#include "c_interface.h"

#endif /* GABAC_GABAC_H_ */
