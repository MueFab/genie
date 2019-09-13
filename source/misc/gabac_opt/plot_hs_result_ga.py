import os
import time
import itertools as it
import subprocess

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

data_dirpath = '/home/sxperfect/Workspace/tnt/Repo/gabac_result/data'
hs_ga_path = '/home/sxperfect/Workspace/tnt/Repo/gabac_result/hyperparam_search_ga'

data_fnames = [entry.name for entry in os.scandir(data_dirpath) if entry.is_file()]
print("Test data found: {} files".format(len(data_fnames)))

avail_transform = [
    "NONE",
    "RLE",
    "EQUALITY",
    "MATCH",
]

ga_csv_fpath = lambda result_dirpath, data_fname, transform : os.path.join(
    result_dirpath,
    'log', 
    "{fname}_{trans_name}.csv".format(fname=data_fname, trans_name=transform)
)

total_tests_per_exp = 200
npops = [4, 5, 10, 20, 25, 50]
ngens = [total_tests_per_exp // npop for npop in npops]
nparams = list(range(1, 6))
color = ['r', 'g', 'b', 'k', 'y']

datapoints = np.ones((len(ngens), len(nparams), len(avail_transform), max(ngens), 955)) * np.inf

for i, ngen_npop_pair in enumerate(zip(ngens, npops)):
    for j, nparam in enumerate(nparams):
        ngen, npop = ngen_npop_pair

        hyperparam_comb = "ngen_{ngen}_npop_{npop}_nparam_{nparam}".format(
            ngen=ngen,
            npop=npop,
            nparam=nparam
        )

        for k, transform in enumerate(avail_transform):
            csv_path = os.path.join(
                hs_ga_path,
                hyperparam_comb, 
                transform + '.csv'
            )

            print(hyperparam_comb + " " + transform)
            df = pd.read_csv(csv_path)

            datapoints[i, j, k, :ngen, :] = df.values

for file_idx in range(955):
    datapoints[:, :, :, :, file_idx] = datapoints[:, :, :, :, file_idx] / np.amin(datapoints[:, :, :, :, file_idx])

    # not_nan = ~ np.isnan(datapoints[:, :, :, :, file_idx])
    # datapoints[not_nan] = datapoints[not_nan] / np.amin(datapoints[not_nan])

fig = plt.figure(num=None, figsize=(16, 9), dpi=120, facecolor='w', edgecolor='k')
for i, ngen_npop_pair in enumerate(zip(ngens, npops)):
    for j, nparam in enumerate(nparams):
        ngen, npop = ngen_npop_pair

        ax = plt.subplot(len(ngens), len(nparams), i + j*len(ngens) + 1)
        hyperparam_comb = "ngen_{ngen}_npop_{npop}_nparam_{nparam}".format(
            ngen=ngen,
            npop=npop,
            nparam=nparam
        )
        
        mean_vals = np.zeros((ngen, len(avail_transform)))

        for k, transform in enumerate(avail_transform):
            mean_vals[:, k] = np.mean(datapoints[i, j, k, :ngen, k])

        ax.plot(
            np.arange(1, ngen+1) /ngen,
            np.mean(mean_vals, axis=1),
            color[k],
            label=hyperparam_comb
        )

        ax.set_title(hyperparam_comb)    

        print('Plotted')

        # ax.legend()
plt.tight_layout()
plt.show()

pass