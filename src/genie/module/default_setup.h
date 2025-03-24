/**
 * Copyright 2018-2024 The Genie Authors.
 * @file default_setup.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of default setup functions for building GENIE components.
 *
 * This file contains the declarations of functions that build the default
 * encoder, decoder, and converter setups for the GENIE framework. These
 * functions create and configure flow graphs for encoding, decoding, and
 * converting genomic data using the GENIE library's components.
 *
 * @details The default setup functions allow users to quickly configure and
 * build different components of the GENIE framework with standard parameters.
 * They are designed to simplify the process of setting up a complete GENIE
 * pipeline for genomic data processing.
 */

#ifndef SRC_GENIE_MODULE_DEFAULT_SETUP_H_
#define SRC_GENIE_MODULE_DEFAULT_SETUP_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>

#include "genie/core/classifier_regroup.h"
#include "genie/core/flow_graph_convert.h"
#include "genie/core/flow_graph_decode.h"
#include "genie/core/flow_graph_encode.h"

// -----------------------------------------------------------------------------

namespace genie::module {

/**
 * @brief Constructs and configures the default encoder setup for Genie
 *
 * This function builds a `FlowGraphEncode` object, which represents the flow
 * graph for encoding genomic data using the GENIE framework. It accepts various
 * parameters such as the number of threads, working directory, reference modes,
 * and raw stream options to configure the encoder for different use cases.
 *
 * @param threads The number of threads to be used for parallel encoding.
 * @param working_dir The working directory for temporary and output files.
 * @param block_size Encoder access unit Size
 * @param external_ref Size parameter for external reference data.
 * @param raw_ref Flag indicating if raw reference data should be used.
 * @param write_raw_streams Flag indicating if raw streams should be written to
 * output.
 * @return A unique pointer to the configured `FlowGraphEncode` object.
 */
std::unique_ptr<core::FlowGraphEncode> BuildDefaultEncoder(
    size_t threads, const std::string& working_dir, size_t block_size,
    core::ClassifierRegroup::RefMode external_ref, bool raw_ref,
    bool write_raw_streams);

/**
 * @brief Constructs and configures the default decoder setup for Genie
 *
 * This function builds a `FlowGraphDecode` object, which represents the flow
 * graph for decoding genomic data using the GENIE framework. It accepts
 * parameters such as the number of threads, combine pairs flag, and a Size
 * parameter for specific configurations.
 *
 * @param threads The number of threads to be used for parallel decoding.
 * @param working_dir The working directory for temporary and output files.
 * @param combine_pairs_flag Flag indicating if read pairs should be combined
 * during decoding.
 * @return A unique pointer to the configured `FlowGraphDecode` object.
 */
std::unique_ptr<core::FlowGraphDecode> build_default_decoder(
    size_t threads, const std::string& working_dir, bool combine_pairs_flag);

/**
 * @brief Constructs and configures the default converter setup for Genie
 *
 * This function builds a `FlowGraphConvert` object, which represents the flow
 * graph for converting genomic data between different formats using the GENIE
 * framework. It accepts the number of threads to configure the conversion
 * pipeline for parallel processing.
 *
 * @param threads The number of threads to be used for parallel conversion.
 * @return A unique pointer to the configured `FlowGraphConvert` object.
 */
std::unique_ptr<core::FlowGraphConvert> build_default_converter(size_t threads);

// -----------------------------------------------------------------------------

}  // namespace genie::module

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_MODULE_DEFAULT_SETUP_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
