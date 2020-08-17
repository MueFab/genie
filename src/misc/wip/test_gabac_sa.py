import os

from gabac_api import GABAC_TRANSFORM
from gabac_api import root_path

from gabac_sa import SimulatedAnnealingForGabac

# gc = GabacConfiguration(GABAC_TRANSFORM.RLE)

# config = gc.generate_random_config()

# r_conf = gc.generate_random_neighbor(config)

# with open(os.path.join(root_path, 'resources', 'input_files', 'one_mebibyte_random'), 'rb') as f:
# with open(os.path.join(root_path, 'resources', 'input_files', 'one_million_zero_bytes'), 'rb') as f:
with open(os.path.join(root_path, 'tmp', 'gabac', 'mpeg-g-descriptor-streams', 'cropped_to_1_MiB_max',
                       '9827_2#49.bam_filtered.gpair.cropped'), 'rb') as f:
    data = f.read()

sa = SimulatedAnnealingForGabac(
    data,
    GABAC_TRANSFORM.RLE,
    # GABAC_TRANSFORM.EQUALITY,
    # GABAC_TRANSFORM.MATCH,
    # GABAC_TRANSFORM.NONE,
    kmax=100,
    kt=1,
    ena_roundtrip=True,
    verbose=True,
    debug=True
)

s, E = sa.start()

print(E)
# print(json.dumps(s, indent=4))
# with open('best_config.json', 'w') as f:
#     json.dump(s, f, indent=4)

sa.show_plot()
sa.result_as_csv('res.csv', '9827_2#49.bam_filtered.gpair.cropped')

# Best result kt=1 kmax=1000 : 0.9880580902099609
# Best result kt=1 : 0.9880714416503906
pass
