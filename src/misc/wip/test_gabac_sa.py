import os
import copy
import json
import random
import math
import ctypes as ct
from collections import OrderedDict

import numpy as np
import matplotlib.pyplot as plt

from gabac_api import libgabac
from gabac_api import gabac_stream
from gabac_api import gabac_io_config
from gabac_api import gabac_data_block
from gabac_api import GABAC_BINARIZATION, GABAC_CONTEXT_SELECT, GABAC_LOG_LEVEL, GABAC_LOG_LEVEL
from gabac_api import GABAC_OPERATION, GABAC_RETURN, GABAC_STREAM_MODE, GABAC_TRANSFORM
from gabac_api import root_path
from test_python_api import array, libc

from gabac_conf_gen import GabacConfiguration
from gabac_sa import SimulatedAnnealingForGabac

# gc = GabacConfiguration(GABAC_TRANSFORM.RLE)

# config = gc.generate_random_config()

# r_conf = gc.generate_random_neighbor(config)

#with open(os.path.join(root_path, 'resources', 'input_files', 'one_mebibyte_random'), 'rb') as f:
# with open(os.path.join(root_path, 'resources', 'input_files', 'one_million_zero_bytes'), 'rb') as f:
with open(os.path.join(root_path, 'tmp', 'gabac', 'mpeg-g-descriptor-streams', 'cropped_to_1_MiB_max', '9827_2#49.bam_filtered.gpair.cropped'), 'rb') as f:
    data = f.read()

sa = SimulatedAnnealingForGabac(
    data, 
    GABAC_TRANSFORM.RLE,
    #GABAC_TRANSFORM.EQUALITY,
    #GABAC_TRANSFORM.MATCH,
    #GABAC_TRANSFORM.NONE,
    kmax=100,
    kt=1,
    ena_roundtrip=True,
    verbose=True,
    debug=True
)

s,E = sa.start()

print(E)
# print(json.dumps(s, indent=4))
# with open('best_config.json', 'w') as f:
#     json.dump(s, f, indent=4)

sa.show_plot()
sa.result_as_csv('res.csv', '9827_2#49.bam_filtered.gpair.cropped')

# Best result kt=1 kmax=1000 : 0.9880580902099609
# Best result kt=1 : 0.9880714416503906
pass