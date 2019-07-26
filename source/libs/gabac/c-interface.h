/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_C_INTERFACE_H_
#define GABAC_C_INTERFACE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Data block ---- */

/**
 * @brief Basic chunk of data
 */
typedef struct gabac_data_block {
    uint8_t *values;    /**<  @brief Actual data */
    size_t values_size; /**<  @brief Number of elements (not bytes, except if
                           word_size = 1!) */
    uint8_t word_size;  /**<  @brief Number of bytes per element */
} gabac_data_block;

/**
 * @brief Allocates a new data block and copies the contents from a source
 * memory location
 * @param block Block to analyze
 * @param data Location to copy. You can pass NULL to initialize with zeros
 * instead
 * @param size Number of elements. Pass zero to create an empty data block.
 * @param wordsize Bytes per element. Valid values are 1, 2, 4 and 8.
 * @return gabac_return_SUCCESS if no error occurred, gabac_return_FAILURE
 * otherwise
 */
int gabac_data_block_init(gabac_data_block *block, const void *data,
                          size_t size, uint8_t wordsize);

/**
 * @brief Free all ressources allocated inside a data block
 * @param block Note that this pointe in particular won't be freed. Just the
 * internal memory.
 * @return gabac_return_SUCCESS
 */
int gabac_data_block_release(gabac_data_block *block);

/**
 * @brief Resizes a data block. If the new size is bigger, the new space is not
 * initiaized.
 * @param block
 * @param size New size
 * @return gabac_return_SUCCESS if no error occurred, gabac_return_FAILURE
 * otherwise
 */
int gabac_data_block_resize(gabac_data_block *block, size_t size);

/**
 * @brief Swaps the contents of two data blocks without actually copying data
 * @param stream1
 * @param stream2
 * @return gabac_return_SUCCESS
 */
int gabac_data_block_swap(gabac_data_block *stream1, gabac_data_block *stream2);

/**
 * @brief Copy the contents to an other block.
 * @param drain Will be resized accordingly
 * @param source
 * @return gabac_return_SUCCESS if no error occurred, gabac_return_FAILURE
 * otherwise
 */
int gabac_data_block_copy(gabac_data_block *drain, gabac_data_block *source);

/**
 * Checks if two data blocks contain the same data
 * @param block1 First block
 * @param block2 Second block
 * @return 0 (false) if blocks are different, 1 (true) otherwise.
 */
int gabac_data_block_equals(gabac_data_block *block1, gabac_data_block *block2);

/**
 * Returns the maximum symbol in a data block
 * @param block
 * @return Maximum
 */
uint64_t gabac_data_block_max(gabac_data_block *block);

/**
 * Returns the maximum possible word size for a data block
 * @param block
 * @return Maximum
 */
uint8_t gabac_data_block_max_wordsize(gabac_data_block *block);

/**
 * @brief Extract a single element from a data block
 * @param block
 * @param index Must be smaller than values_size
 * @return The element
 */
uint64_t gabac_data_block_get(const gabac_data_block *block, size_t index);

/**
 * @brief Set a single element in a data block
 * @param block
 * @param index Must be smaller than values_size
 * @param val New value
 */
void gabac_data_block_set(const gabac_data_block *block, size_t index,
                          uint64_t val);

/* Data block end*/

/* Constants */

/**
 * @brief Return states
 */
typedef enum gabac_return {
    gabac_return_SUCCESS = 0, /**< @brief OK */
    gabac_return_FAILURE = 1  /**< @brief Not OK */
} gabac_return;

/**
 * @brief Different logging urgency
 */
typedef enum gabac_log_level {
    gabac_log_level_TRACE = 0, /**< @brief Log every step in great detail */
    gabac_log_level_DEBUG = 1, /**< @brief Intermediate results */
    gabac_log_level_INFO = 2,  /**< @brief Expected Results */
    gabac_log_level_WARNING =
        3, /**< @brief Suspicious events (may be an error) */
    gabac_log_level_ERROR = 4, /**< @brief Handled errors */
    gabac_log_level_FATAL =
        5 /**< @brief Error causing application to terminate */
} gabac_log_level;

/**
 * @brief Gabac available transformations
 */
typedef enum gabac_transform {
    gabac_transform_NONE = 0,     /**< @brief Do nothing */
    gabac_transform_EQUALITY = 1, /**< @brief Find equal values sequentially */
    gabac_transform_MATCH = 2,    /**< @brief Find larger sequence matches */
    gabac_transform_RLE = 3,      /**< @brief Find run lengths */
    gabac_transform_LUT = 4,  /**< @brief Remap symbols based on probability */
    gabac_transform_DIFF = 5, /**< @brief Use differences between symbol values
                                 instead of symbols */
    gabac_transform_CABAC = 6 /**< @brief Entropy coding based on cabac */
} gabac_transform;

/**
 * @brief Contains the number of expected parameters for each gabac_transform
 */
extern const uint8_t gabac_transform_PARAM_NUM[];

/**
 * @brief Contains the number of expected streams for each gabac_transform
 */
extern const uint8_t gabac_transform_STREAM_NUM[];

/**
 * @brief Contains the expected word size for each stream in a gabac_transform.
 * 255 means that the word size will be equal to the input stream word size
 */
extern const uint8_t gabac_transform_WORD_SIZES[][3];

/**
 * @brief gabac_run operations
 */
typedef enum gabac_operation {
    gabac_operation_ENCODE = 0, /**< @brief Use configuration to compress */
    gabac_operation_DECODE = 1, /**< @brief Use configuration to decompress */
    gabac_operation_ANALYZE =
        2 /**< @brief Find best configuration for input data */
} gabac_operation;

/**
 * @brief Binarizations for cabac transformation
 */
typedef enum gabac_binarization {
    gabac_binarization_BI = 0,  /**< @brief Binary */
    gabac_binarization_TU = 1,  /**< @brief Truncated Unary */
    gabac_binarization_EG = 2,  /**< @brief Exponential Golomb */
    gabac_binarization_SEG = 3, /**< @brief Signed Exponential Golomb */
    gabac_binarization_TEG = 4, /**< @brief Truncated Exponential Golomb */
    gabac_binarization_STEG =
        5 /**< @brief Signed Truncated Exponential Golomb */
} gabac_binarization;

/**
 * @brief Context selection modes for cabac transformation
 */
typedef enum gabac_context_select {
    gabac_context_select_BYPASS = 0, /**< @brief Do not use arithmetic coding */
    gabac_context_select_ADAPTIVE_ORDER_0 =
        1, /**< @brief Current symbol only */
    gabac_context_select_ADAPTIVE_ORDER_1 =
        2, /**< @brief Use current + previous symbol */
    gabac_context_select_ADAPTIVE_ORDER_2 =
        3 /**< @brief Use current + previous + before previous symbol */
} gabac_context_select;

/* Constants end */

/* Operations */

/**
 * @brief Flags for different data types
 */
typedef enum gabac_stream_mode {
    gabac_stream_mode_FILE = 0,  /**< @brief Read/write from file */
    gabac_stream_mode_BUFFER = 1 /**< @brief Read/write from data block */
} gabac_stream_mode;

/**
 * @brief An i/o-stream
 */
typedef struct gabac_stream {
    void *data; /**< @brief data_block* if input_mode == buffer and FILE* if
                   input_mode == file */
    gabac_stream_mode input_mode; /**< @brief Flag for data type */
} gabac_stream;

extern const char
    *gabac_stream_create_file_STDOUT; /**< @brief Constant for stdout */
extern const char
    *gabac_stream_create_file_STDERR; /**< @brief Constant for stderr */
extern const char
    *gabac_stream_create_file_STDIN; /**< @brief Constant for stdin */
extern const char
    *gabac_stream_create_file_TMP; /**< @brief Constant for tmpfile */

/**
 * @brief Initialize a stream from a file
 * @param stream Where to initialize
 * @param filename Filename to open. You can also pass
 * gabac_stream_create_file_* to open stdout, stderr, stdin or a tmpfile
 * instead. Note that you need to call swap after using a tmpfile to read it.
 * Otherwise it will be closed when released. Stdout is not closed.
 * @param filename_size Length of file name. Pass 0 when using a gabac constant.
 * @param write True to open in write mode instead of read mode
 * @return gabac_return_SUCCESS or gabac_return_FAILURE if an error occurred
 */
int gabac_stream_create_file(gabac_stream *stream, const char *filename,
                             size_t filename_size, int write);

/**
 * @brief Create a stream from a data block.
 * @param stream Where to initialize
 * @param block Initial stream content (will be swapped inside). Intended for
 * input streams. You can also pass NULL for no initial contents.
 * @return gabac_return_SUCCESS or gabac_return_FAILURE if an error occurred
 */
int gabac_stream_create_buffer(gabac_stream *stream, gabac_data_block *block);

/**
 * @brief Swap the block contents in a stream into an other data block. Does not
 * work in file mode.
 * @param stream Stream
 * @param block External block
 * @return gabac_return_SUCCESS or gabac_return_FAILURE if an error occurred
 */
int gabac_stream_swap_block(gabac_stream *stream, gabac_data_block *block);

/**
 * @brief Swap the file pointer in a stream with an other file pointer. Does not
 * work in block mode.
 * @param stream Stream
 * @param file File pointer
 * @return gabac_return_SUCCESS or gabac_return_FAILURE if an error occurred
 */
int gabac_stream_swap_file(gabac_stream *stream, FILE **file);

/**
 * @brief Release a stream (including associated files or buffers)
 * @param stream
 * @return gabac_return_SUCCESS
 */
int gabac_stream_release(gabac_stream *stream);

/**
 * @brief Stream i/o information
 */
typedef struct gabac_io_config {
    gabac_stream input;  /**< @brief Where to get data from */
    gabac_stream output; /**< @brief Where to write data to */
    gabac_stream log;    /**< @brief Where to output log */
    gabac_log_level log_level /**< @brief Logging level */;
    size_t blocksize; /**< @brief Block size. Put 0 for infinite block size */
} gabac_io_config;

/**
 * @brief Execute a single transformation in gabac
 * <pre>
 * transform       | param0       | param1    | param2         | param3    |
 * stream0(wordsize) | stream1(wordsize) | stream2(wordsize)    \n
 * -------------------------------------------------------------------------------------------------------------------------------------
 * \n no_transform    | _            | _         | _              | _         |
 * sequence(I)       | _                 | _                    \n
 * equality_coding | _            | _         | _              | _         |
 * raw_values(I)     | equality_flags(1) | _                    \n match_coding
 * | window_size  | _         | _              | _         | raw_values(I)     |
 * pointers(4)       | lengths(4)           \n rle_coding      | guard        |
 * _         | _              | _         | raw_values(I)     | run_lengths(4)
 * | _                    \n lut_transform   | order        | _         | _ | _
 * | sequence(I)       | order0_table(I)   | high_order_table(I)  \n diff_coding
 * | _            | _         | _              | _         | sequence(I)       |
 * _                 | _                    \n cabac_coding    | binarization |
 * bin_param | context_select | wordsize* | sequence(I)       | _ | _ \n
 *  --> I: input data word size; --> *: only needed for decoding; \n
 * </pre>
 *
 * @param transformationID Which trasnformation to execute
 * @param param Array of parameters for the transformation
 * @param inverse If you want to do the inverse transformation instead
 * @param input Array of input data streams
 * @return gabac_return_SUCCESS or gabac_return_FAILURE if an error occurred
 */
int gabac_execute_transform(uint8_t transformationID, const uint64_t *param,
                            int inverse, gabac_data_block *input);

/**
 * @brief Execute a complete run of gabac
 * @param operation Which operation to execute
 * @param io_config
 * @param config_json Pointer to json string
 * @param json_length Length of json stream
 * @return gabac_return_SUCCESS or gabac_return_FAILURE if an error occurred
 */
int gabac_run(gabac_operation operation, gabac_io_config *io_config,
              const char *config_json, size_t json_length);

/* Operations end */

/* Configuration */

/**
 * Checks if a configuration is valid for all streams in the passed
 * specification
 * @param inconf Configuration string
 * @param inconf_size length of string
 * @param max Maximum stream value
 * @param wsize Maximum word size
 * @return True if it is general
 */
int gabac_config_is_general(const char *inconf, size_t inconf_size,
                            uint64_t max, uint8_t wsize);

/**
 * Generalizes a configuration to a complete set of streams
 * @param inconf String of conf
 * @param inconf_size length of string
 * @param max Maximum stream value
 * @param wsize Maximum word size
 * @param outconf Where to put the new config. You have to free this return
 * value.
 * @param outconf_size Where to put size of new config string
 * @return success
 */
int gabac_config_generalize_create(const char *inconf, size_t inconf_size,
                                   uint64_t max, uint8_t wsize, char **outconf,
                                   size_t *outconf_size);

/**
 * Checks if a configuration has optimal tweaks applied for all streams in the
 * passed specification
 * @param inconf Configuration string
 * @param inconf_size length of string
 * @param max Maximum stream value
 * @param wsize Maximum word size
 * @return True if it is optimal
 */
int gabac_config_is_optimal(const char *inconf, size_t inconf_size,
                            uint64_t max, uint8_t wsize);

/**
 * Optimizes a configuration with small tweaks.
 * @param inconf String of conf
 * @param inconf_size length of string
 * @param max Maximum stream value
 * @param wsize Maximum word size
 * @param outconf Where to put the new config. You have to free this return
 * value.
 * @param outconf_size Where to put size of new config string
 * @return success
 */
int gabac_config_optimize_create(const char *inconf, size_t inconf_size,
                                 uint64_t max, uint8_t wsize, char **outconf,
                                 size_t *outconf_size);

/**
 * Convenience function to free a configuration string returned by other
 * functions
 * @param outconf
 * @return
 */
int gabac_config_free(char **outconf);

/* Configuration End */

#ifdef __cplusplus
}
#endif

#endif /* GABAC_C_INTERFACE_H_ */
