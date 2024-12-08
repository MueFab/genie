/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file that consolidates various components of the gabac library.
 *
 * This file serves as a unified header for the gabac library, including
 * functionalities for encoding, decoding, and stream manipulation. It provides
 * access to essential components like configuration management, run operations,
 * and stream handling for gabac encoding and decoding processes.
 *
 * @details The `gabac.h` header file is the central entry point for using the
 * gabac library. It includes other header files required for configuration,
 * running encode/Decode operations, and managing input/output streams. The file
 * ensures compatibility with C++ projects by wrapping the includes and
 * components within `__cplusplus` guards.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_GABAC_H_
#define SRC_GENIE_ENTROPY_GABAC_GABAC_H_

// -----------------------------------------------------------------------------
#ifdef __cplusplus  // Ensure compatibility with C++ compilers.

// -----------------------------------------------------------------------------
/* Encode / Decode */
#include "genie/entropy/gabac/configuration.h"
#include "genie/entropy/gabac/run.h"

/* io */
#include "genie/entropy/gabac/streams.h"

// -----------------------------------------------------------------------------

#endif /* __cplusplus */

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_GABAC_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
