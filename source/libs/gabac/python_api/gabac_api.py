"""GABAC library."""
import ctypes as ct
import numpy as np
import os
import sys
import time

#libgabac_path = os.environ['LIBGABAC_PATH']
#libgabac = ct.cdll.LoadLibrary(libgabac_path)
import subprocess

process = subprocess.Popen("git rev-parse --show-toplevel".split(), stdout=subprocess.PIPE)
output, error = process.communicate()
root_path = output.strip().decode("utf-8")

libgabac = ct.cdll.LoadLibrary(os.path.join(
    root_path,
    'build/lib/libgabac.so'
))

libc = ct.CDLL("libc.so.6")

r"""
c_bool 	    _Bool 	bool (1)
c_char 	    char 	1-character string
c_wchar 	    wchar_t 	1-character unicode string
c_byte 	    char 	int/long
c_ubyte 	    unsigned char 	int/long
c_short 	    short 	int/long
c_ushort 	    unsigned short 	int/long
c_int 	    int 	int/long
c_uint 	    unsigned int 	int/long
c_long 	    long 	int/long
c_ulong 	    unsigned long 	int/long
c_longlong 	    __int64 or long long 	int/long
c_ulonglong 	    unsigned __int64 or unsigned long long 	int/long
c_float 	    float 	float
c_double 	    double 	float
c_longdouble 	    long double 	float
c_char_p 	    char * (NUL terminated) 	string or None
c_wchar_p 	    wchar_t * (NUL terminated) 	unicode or None
c_void_p
[ct.c_void_p,
ct.c_size_t,
ct.c_uint,
ct.c_void_p,
ct.c_size_t,
ct.c_uint,
ct.c_void_p,
ct.c_void_p]
"""

### As Input
# unsigned char **const bitstream               bitstream = ct.pointer(ct.c_ubyte()
#                                               ct.pointer(bitstream))               
### As Output                        
# const uint64_t *const symbols                 symbols.ctypes.data_as(ct.POINTER(ct.c_uint64))
# int64_t *const symbols                        symbols.ctypes.data_as(ct.POINTER(ct.c_uint64))
# unsigned int *const binarizationParameters    binarization_parameters.ctypes.data_as(ct.POINTER(ct.c_uint))

STRONG_TYPE = True

class GABAC_RETURN:
    r"""Return Codes.
    
    Values:
        SUCCESS : if success.
        FAILURE : if failed.
    """
    SUCCESS = 0
    FAILURE = 1

class GABAC_LOG_LEVEL:
    r"""
    Different logging urgency

    Values:
        TRACE   : Log every step in great detail
        DEBUG   : Intermediate results
        INFO    : Expected Results
        WARNING : Suspicious events (may be an error)
        ERROR   : Handled errors
        FATAL   : Error causing application to terminate
    """
    TRACE = 0
    DEBUG = 1
    INFO = 2
    WARNING = 3
    ERROR = 4
    FATAL = 5

class GABAC_TRANSFORM:
    r"""
    Gabac available transformations

    Values:
        gabac_transform_NONE        : Do nothing
        gabac_transform_EQUALITY    : Find equal values sequentially
        gabac_transform_MATCH       : Find larger sequence matches
        gabac_transform_RLE         : Find run lengths
        gabac_transform_LUT         : Remap symbols based on probability
        gabac_transform_DIFF        : Use differences between symbol values instead of symbols
        gabac_transform_CABAC       : libs based on cabac
    """
    NONE = 0
    EQUALITY = 1
    MATCH = 2
    RLE = 3
    LUT = 4
    DIFF = 5
    CABAC = 6

class GABAC_BINARIZATION:
    r"""
    Binarizations for cabac transformation

    Values:
        BI : Binary
        TU : Truncated Unary
        EG : Exponential Golomb
        SEG  : Signed Exponential Golomb
        TEG  : Truncated Exponential Golomb
        STEG : Signed Truncated Exponential Golomb
    """
    BI = 0
    TU = 1
    EG = 2
    SEG = 3
    TEG = 4
    STEG = 5

class GABAC_CONTEXT_SELECT:
    r"""
    Context selection modes for cabac transformation

    Values:
        BYPASS           : Do not use arithmetic libs
        ADAPTIVE_ORDER_0 : Current symbol only
        ADAPTIVE_ORDER_1 : Use current + previous symbol
        ADAPTIVE_ORDER_2 : Use current + previous + before previous symbol
    """
    BYPASS = 0
    ADAPTIVE_ORDER_0 = 1
    ADAPTIVE_ORDER_1 = 2
    ADAPTIVE_ORDER_2 = 3 

class GABAC_OPERATION:
    r"""
    Enum for gabac operation

    Values
        ENCODE  : Use configuration to compress
        DECODE  : Use configuration to decompress
        ANALYZE : Find best configuration for input data
    """
    ENCODE = 0
    DECODE = 1
    ANALYZE = 2

class GABAC_STREAM_MODE:
    r"""
    Flags for different data types

    Values
        FILE    : Read/write from file
        BUFFER  : Read/write from data block
    """
    FILE = 0
    BUFFER = 1

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
    #ct.c_void_p,
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

def gabac_execute_transform(
    transformation_id,
    params,
    inverse,
    input_gabac_data_block,
):
    # uint8_t transformationID,
    # const uint64_t *param,
    # int inverse,
    # gabac_data_block *input
    return_code = libgabac.gabac_execute_transform(
        transformation_id,
        params.ctypes.data_as(ct.POINTER(ct.c_uint64)),
        inverse,
        input_gabac_data_block.ctypes.data_as(ct.POINTER(gabac_data_block))
    )

    if return_code == GABAC_RETURN.FAILURE:
        sys.exit("error: gabac_encode() failed")

    return input_gabac_data_block

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

def gabac_run(
    operation,
    io_config,
    config_json
):

    io_config = ct.pointer(gabac_io_config)

    return_code = libgabac.gabac_run(
        operation,
        io_config.ctypes.data_as(ct.POINTER(gabac_io_config)),
        config_json.ctypes.data_as(ct.POINTER(ct.c_char)),
        len(config_json)
    )

    if return_code == GABAC_RETURN.FAILURE:
        sys.exit("error: gabac_run() failed")

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

###=============================================================================================================
### Additional functions

def array(dtype, data):
    if isinstance(data, int):
        arr_dtype = data * dtype
        return arr_dtype()
    elif isinstance(data, (list, tuple, bytes)):
        arr_dtype = dtype * len(data)
        return arr_dtype(*data)
    elif isinstance(data, str):
        arr_dtype = dtype * len(data)
        return arr_dtype(*data.encode())
    elif isinstance(data, dict):
        raise TypeError("Not yet implemented for type dictionary")
    else:
        raise TypeError("Incorrect datatype of data")

def print_array(arr):
    for val in arr:
        if isinstance(val, bytes):
            print("{}".format(val.decode()), end='')
        else:
            print("{}".format(val), end='')
    print()

def print_block(block):
    for i in range(block.values_size):
        # print("{:02d}".format(libgabac.gabac_data_block_get(ct.byref(block), i)), end='')
        libc.printf(b"%lu ", libgabac.gabac_data_block_get(ct.byref(block), i))
    # print()
    libc.printf(b"\n")

def get_block_values(block):
    values = ""
    for i in range(block.values_size):
        values += "{:02d}".format(libgabac.gabac_data_block_get(ct.byref(block), i))
    return values

def are_blocks_equal(block1, block2):
    # st = time.time()
    if block1.values_size == block2.values_size:
        for i in range(block1.values_size):
            if libgabac.gabac_data_block_get(ct.byref(block1), i) != libgabac.gabac_data_block_get(ct.byref(block2), i):
                print(i)
                return False
        # ~7 seconds
        # et = time.time() - st
        return True
    else:
        return False


