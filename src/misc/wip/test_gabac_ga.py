import os
import json
import math

import matplotlib.pyplot as plt

from gabac_api import libgabac
from gabac_api import gabac_io_config
from gabac_api import gabac_data_block
from gabac_api import GABAC_OPERATION, GABAC_RETURN, GABAC_TRANSFORM
from gabac_api import root_path

from gabac_ga import GeneticAlgorithmForGabac

with open(os.path.join(root_path, 'resources', 'input_files', 'one_mebibyte_random'), 'rb') as f:
    # with open(os.path.join(root_path, 'resources', 'input_files', 'one_million_zero_bytes'), 'rb') as f:
    # with open(os.path.join(root_path, 'tmp', 'gabac', 'mpeg-g-descriptor-streams', 'cropped_to_1_MiB_max', '9827_2#49.bam_filtered.gpair.cropped'), 'rb') as f:
    data = f.read()

ga = GeneticAlgorithmForGabac(
    data,
    # GABAC_TRANSFORM.RLE,
    GABAC_TRANSFORM.EQUALITY,
    # GABAC_TRANSFORM.MATCH,
    # GABAC_TRANSFORM.NONE,
    mutation_nparam=2,
    num_populations=10,
    num_generations=10,
    ena_roundtrip=True,
    debug=False,
    verbose=False
)

s, E = ga.start()

print(E)
print(json.dumps(s, indent=4))
with open('best_config.json', 'w') as f:
    json.dump(s, f, indent=4)

ga.show_plot()
ga.result_as_csv('res.csv', 'one_mebibyte_random')

# Best result kt=1 kmax=1000 : 0.9880580902099609
# Best result kt=1 : 0.9880714416503906
pass
