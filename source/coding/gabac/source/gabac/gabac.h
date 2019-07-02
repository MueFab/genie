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
#include "gabac/constants.h"
#include "gabac/exceptions.h"

/* Encode / Decode */
#include "gabac/analysis.h"
#include "gabac/configuration.h"
#include "gabac/run.h"

/* io */
#include "gabac/block_stepper.h"
#include "gabac/data_block.h"
#include "gabac/streams.h"

#endif  /* __cplusplus */

#include "gabac/c_interface.h"

#endif /* GABAC_GABAC_H_ */
