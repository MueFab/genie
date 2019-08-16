import os
import copy
import json
import subprocess
import unittest
import ctypes as ct

import numpy as np

from new_gabac import libgabac
from new_gabac import GABAC_BINARIZATION, GABAC_CONTEXT_SELCT, GABAC_LOG_LEVEL, GABAC_LOG_LEVEL
from new_gabac import GABAC_OPERATION, GABAC_RETURN, GABAC_STREAM_MODE, GABAC_TRANSFORM
from new_gabac import gabac_data_block
from new_gabac import gabac_io_config

from new_gabac import GABAC_STREAM_MODE
from new_gabac import gabac_stream

from test_python_api import array


process = subprocess.Popen("git rev-parse --show-toplevel".split(), stdout=subprocess.PIPE)
output, error = process.communicate()
root_path = output.strip().decode("utf-8")

libc = ct.CDLL("libc.so.6")

config_json_path = os.path.join(
    root_path,
    'resources',
    'configuration_files',
    'match_coding.json'
)

with open(config_json_path, 'rb') as f:
    config_json_raw = f.read()
config_json_ctype = ct.c_char_p(config_json_raw)

input_bitstream_path = os.path.join(
    root_path,
    'resources',
    'input_files',
    'one_mebibyte_random'
)

with open(input_bitstream_path, 'rb') as f:
    input_bitstream_raw = f.read()

input_bitstream_ctype = ct.c_char_p(input_bitstream_raw)
random_input = np.zeros((10000), dtype=np.uint64)

blocks = array(gabac_data_block, 3)

input_g_data_block = libgabac.gabac_data_block_init(
    blocks[0],
    # input_bitstream_raw,
    # len(input_bitstream_raw) // ct.sizeof(ct.c_int),
    # ct.sizeof(ct.c_int)
    random_input.ctypes.data_as(ct.c_void_p),
    len(random_input),
    ct.sizeof(ct.c_uint64),
)

# test = np.ndarray(
#     (blocks[0].values_size),
#     'uint64',
#     bytes(blocks[0].values),
#     order='C'
# )
    
g_stream_input = gabac_stream(
    # data=ct.cast(input_bitstream_raw, ct.c_void_p),
    # data=ct.cast(input_bitstream_ctype, ct.c_void_p),
    # data=random_input.ctypes.data_as(ct.c_void_p),
    data=ct.cast(ct.byref(blocks[0]), ct.c_void_p),
    input_mode=GABAC_STREAM_MODE.BUFFER
)

# return_val = libgabac.gabac_data_block_init(
#     blocks[1],
#     # input_bitstream_raw,
#     # len(input_bitstream_raw) // ct.sizeof(ct.c_int),
#     # ct.sizeof(ct.c_int)
#     random_input.ctypes.data_as(ct.c_void_p),
#     len(random_input),
#     ct.sizeof(ct.c_uint64),
# )

# output = gabac_data_block()

g_stream_output = gabac_stream(
    # data=ct.cast(output, ct.c_void_p),
    # data=None,
    # data=random_input.ctypes.data_as(ct.c_void_p),
    data=ct.cast(ct.byref(blocks[1]), ct.c_void_p),
    input_mode=GABAC_STREAM_MODE.BUFFER
)

# log = ct.c_char_p()
g_stream_output = gabac_stream(
    # data=ct.cast(log, ct.c_void_p),
    # data=None,
    # data=random_input.ctypes.data_as(ct.c_void_p),
    data=ct.cast(ct.byref(blocks[2]), ct.c_void_p),
    input_mode=GABAC_STREAM_MODE.BUFFER
)

g_io_config = gabac_io_config(
    input=g_stream_input,
    output=g_stream_output,
    log=g_stream_output,
    log_level=GABAC_LOG_LEVEL.INFO,
    blocksize=0,
)

out = libgabac.gabac_run(
    GABAC_OPERATION.ENCODE,
    g_io_config,
    config_json_ctype,
    len(config_json_raw)
)