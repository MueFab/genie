### Python built-in modules
import ctypes as ct

### External modules (requires pip install)
### Own modules
from . import (
    libgabac_fpath,
)

libgabac = ct.cdll.LoadLibrary(libgabac_fpath)

### As Input
# unsigned char **const bitstream               bitstream = ct.pointer(ct.c_ubyte()
#                                               ct.pointer(bitstream))               
### As Output                        
# const uint64_t *const symbols                 symbols.ctypes.data_as(ct.POINTER(ct.c_uint64))
# int64_t *const symbols                        symbols.ctypes.data_as(ct.POINTER(ct.c_uint64))
# unsigned int *const binarizationParameters    binarization_parameters.ctypes.data_as(ct.POINTER(ct.c_uint))

STRONG_TYPE = True


###-------Data Block-------###

class gabac_data_block(ct.Structure):
    r"""
    uint8_t *values;    /**<  @brief Actual data */
    size_t values_size; /**<  @brief Number of elements (not bytes, except if word_size = 1!) */
    uint8_t word_size;  /**<  @brief Number of bytes per element */
    """
    _fields_ = [
        ("values", ct.c_void_p),
        ("values_size", ct.c_size_t),
        ("word_size", ct.c_uint8),
    ]


# Arguments
#   gabac_data_block *block,
#   const void *data,
#   size_t size,
#   uint8_t wordsize
# Return
#   int gabac_data_block_init()
libgabac.gabac_data_block_init.argtypes = [
    ct.POINTER(gabac_data_block) if STRONG_TYPE else ct.c_void_p,
    ct.c_void_p,
    ct.c_size_t,
    ct.c_uint8
]
libgabac.gabac_data_block_init.restype = ct.c_int

# Arguments
#   gabac_data_block *block
# Return
#   int gabac_data_block_release()
libgabac.gabac_data_block_release.argtypes = [
    ct.POINTER(gabac_data_block) if STRONG_TYPE else ct.c_void_p,
]
libgabac.gabac_data_block_init.restype = ct.c_int

# Arguments
#   gabac_data_block *block
#   size_t size
# Return
#   int gabac_data_block_resize()
libgabac.gabac_data_block_resize.argtypes = [
    ct.POINTER(gabac_data_block) if STRONG_TYPE else ct.c_void_p,
    ct.c_size_t
]
libgabac.gabac_data_block_resize.restype = ct.c_int

# Arguments
#   gabac_data_block *stream1
#   gabac_data_block *stream2
# Return
#   int gabac_data_block_swap()
libgabac.gabac_data_block_swap.argtypes = [
    ct.POINTER(gabac_data_block) if STRONG_TYPE else ct.c_void_p,
    ct.POINTER(gabac_data_block) if STRONG_TYPE else ct.c_void_p,
]
libgabac.gabac_data_block_swap.restype = ct.c_int

# Arguments
#   gabac_data_block *drain,
#   gabac_data_block *source
# Return
#   int gabac_data_block_copy()
libgabac.gabac_data_block_copy.argtypes = [
    ct.POINTER(gabac_data_block) if STRONG_TYPE else ct.c_void_p,
    ct.POINTER(gabac_data_block) if STRONG_TYPE else ct.c_void_p,
]
libgabac.gabac_data_block_copy.restype = ct.c_int

# Arguments
#   gabac_data_block *block1,
#   gabac_data_block *block2
# Return
#   int gabac_data_block_equals(
libgabac.gabac_data_block_equals.argtypes = [
    ct.POINTER(gabac_data_block) if STRONG_TYPE else ct.c_void_p,
    ct.POINTER(gabac_data_block) if STRONG_TYPE else ct.c_void_p,
]
libgabac.gabac_data_block_equals.restype = ct.c_int

# Arguments
#   const gabac_data_block *block,
#   size_t index
# Return
#   uint64_t gabac_data_block_get()
libgabac.gabac_data_block_get.argtypes = [
    ct.POINTER(gabac_data_block) if STRONG_TYPE else ct.c_void_p,
    ct.c_size_t
]
libgabac.gabac_data_block_get.restype = ct.c_uint64

# Arguments
#   const gabac_data_block *block,
#   size_t index,
#   uint64_t val
# Return
#   void gabac_data_block_set()   
libgabac.gabac_data_block_set.argtypes = [
    ct.POINTER(gabac_data_block) if STRONG_TYPE else ct.c_void_p,
    ct.c_size_t
]
libgabac.gabac_data_block_set.restype = None


class gabac_stream(ct.Structure):
    r"""
    void *data;
    gabac_stream_mode input_mode;
    """
    _fields_ = [
        ("data", ct.c_void_p),
        ("input_mode", ct.c_uint)
    ]


# Arguments
#   gabac_stream *stream,
#   const char *filename,
#   size_t filename_size,
#   int write
# Return
#   int gabac_stream_create_file(
libgabac.gabac_stream_create_file.argtypes = [
    # ct.c_void_p,
    # ct.c_void_p,
    ct.POINTER(gabac_stream) if STRONG_TYPE else ct.c_void_p,
    ct.c_char_p if STRONG_TYPE else ct.c_void_p,
    ct.c_size_t,
    ct.c_int
]
libgabac.gabac_stream_create_file.restype = ct.c_int

# Arguments
#   gabac_stream *stream,
#   gabac_data_block *block
# Return
#   int gabac_stream_create_buffer(
libgabac.gabac_stream_create_buffer.argtypes = [
    # ct.c_void_p,
    # ct.c_void_p,
    ct.POINTER(gabac_stream) if STRONG_TYPE else ct.c_void_p,
    ct.POINTER(gabac_data_block) if STRONG_TYPE else ct.c_void_p,
]
libgabac.gabac_stream_create_buffer.restype = ct.c_int

# Arguments
#   gabac_stream *stream,
#   gabac_data_block *block
# Return
#   int gabac_stream_swap_block(
libgabac.gabac_stream_swap_block.argtypes = [
    ct.POINTER(gabac_stream) if STRONG_TYPE else ct.c_void_p,
    ct.POINTER(gabac_data_block) if STRONG_TYPE else ct.c_void_p,
]
libgabac.gabac_stream_swap_block.restype = ct.c_int

# Arguments
#   gabac_stream *stream,
#   FILE **file
# Return
#   int gabac_stream_swap_file(
# TODO: Check datatype for file
libgabac.gabac_stream_swap_file.argtypes = [
    ct.POINTER(gabac_stream) if STRONG_TYPE else ct.c_void_p,
    ct.c_void_p
]
libgabac.gabac_stream_swap_file

# Arguments
#   gabac_stream *stream
# Return
#   int gabac_stream_release(
libgabac.gabac_stream_release.argtypes = [
    ct.POINTER(gabac_stream) if STRONG_TYPE else ct.c_void_p,
]
libgabac.gabac_stream_release.restype = ct.c_int


class gabac_io_config(ct.Structure):
    r"""
    gabac_stream input
    gabac_stream output
    gabac_stream log
    gabac_log_level log_level
    size_t blocksize
    """
    _fields_ = [
        ("input", gabac_stream),
        ("output", gabac_stream),
        ("log", gabac_stream),
        ("log_level", ct.c_uint),
        ("blocksize", ct.c_size_t),
    ]


# gabac_execute_transform
# -----------------------------------------------------------------------------

# Arguments
#   uint8_t transformationID,
#   const uint64_t *param,
#   int inverse,
#   gabac_data_block *input
# Return
#   int gabac_execute_transform(
libgabac.gabac_execute_transform.argtypes = [
    ct.c_uint8,
    ct.POINTER(ct.c_uint64) if STRONG_TYPE else ct.c_void_p,
    ct.c_int,
    ct.POINTER(gabac_data_block) if STRONG_TYPE else ct.c_void_p,
]

libgabac.gabac_execute_transform.restype = ct.c_int

# gabac_run
# -----------------------------------------------------------------------------
# Arguments
#   gabac_operation     operation
#   gabac_io_config     *io_config,
#   const char          *config_json,
#   size_t              json_length
# Return
#   int gabac_run(

libgabac.gabac_run.argtypes = [
    ct.c_uint,
    ct.POINTER(gabac_io_config) if STRONG_TYPE else ct.c_void_p,
    ct.c_char_p if STRONG_TYPE else ct.c_void_p,
    ct.c_size_t
]

libgabac.gabac_run.restype = ct.c_int

# gabac_config_is_general
# Arguments
#   const char* inconf, 
#   size_t inconf_size, 
#   uint64_t max, 
#   uint8_t wsize
# Return
#   int gabac_config_is_general();
libgabac.gabac_config_is_general.argtypes = [
    ct.c_char_p if STRONG_TYPE else ct.c_void_p,
    ct.c_size_t,
    ct.c_uint64,
    ct.c_uint8
]

# Function
#   int gabac_config_is_general(
#       const char *inconf, 
#       size_t inconf_size,
#       uint64_t max, 
#       uint8_t wsize
#   )
libgabac.gabac_config_is_general.argtypes = [
    ct.c_char_p if STRONG_TYPE else ct.c_void_p,
    ct.c_size_t,
    ct.c_uint64,
    ct.c_uint8
]
