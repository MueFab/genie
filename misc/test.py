import gabac_api
from gabac_opt.gabac_conf_gen import GabacConfiguration

with open("../../gabac_result/data/9827_2#49.bam_filtered.gpair.000", 'rb') as f:
    data = f.read()
gc = GabacConfiguration(gabac_api.GABAC_TRANSFORM.NONE, data, ena_roundtrip=False)

