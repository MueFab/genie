import json
import os
from gabac_conf_gen import GabacConfiguration

from ..gabac_api import (
    repo_path,
    GABAC_TRANSFORM,
)

with open(os.path.join(repo_path, 'tmp', 'gabac', 'mpeg-g-descriptor-streams', 'cropped_to_1_MiB_max',
                       '9827_2#49.bam_filtered.gpair.cropped'), 'rb') as f:
    data = f.read()

gc = GabacConfiguration(
    # GABAC_TRANSFORM.RLE,
    GABAC_TRANSFORM.EQUALITY,
    # GABAC_TRANSFORM.MATCH,
    # GABAC_TRANSFORM.NONE,
    data,
    ena_roundtrip=True
)

init_config = gc.generate_random_config()

new_config = gc.mutate_nparams(init_config, nparams=2)

with open('curr_config.json', 'r') as f:
    config = json.load(f)

gc.adjust_config(config)

print(json.dumps(config, indent=4))
