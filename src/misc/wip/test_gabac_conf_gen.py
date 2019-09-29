import os
import copy
import json
import random
import math
import ctypes as ct
from collections import OrderedDict

import numpy as np
import matplotlib.pyplot as plt

from ..gabac_api import (
    repo_path,
    libgabac,
    libc,
    gabac_data_block,
    gabac_io_config,
    GABAC_BINARIZATION, 
    GABAC_CONTEXT_SELECT, 
    GABAC_LOG_LEVEL, 
    GABAC_LOG_LEVEL,
    GABAC_OPERATION, 
    GABAC_RETURN, 
    GABAC_STREAM_MODE, 
    GABAC_TRANSFORM,    
    array, 
    print_array, 
    print_block, 
    get_block_values,
)

from gabac_conf_gen import GabacConfiguration

with open(os.path.join(repo_path, 'tmp', 'gabac', 'mpeg-g-descriptor-streams', 'cropped_to_1_MiB_max', '9827_2#49.bam_filtered.gpair.cropped'), 'rb') as f:
    data = f.read()

gc = GabacConfiguration(
    # GABAC_TRANSFORM.RLE,
    GABAC_TRANSFORM.EQUALITY,
    #GABAC_TRANSFORM.MATCH,
    #GABAC_TRANSFORM.NONE,
    data,
    ena_roundtrip=True
)

init_config = gc.generate_random_config()

new_config = gc.mutate_nparams(init_config, nparams=2)

with open('curr_config.json', 'r') as f:
    config = json.load(f)

gc.adjust_config(config)

print(json.dumps(config, indent=4))