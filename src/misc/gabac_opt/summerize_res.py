import os
import time
import itertools as it
import subprocess

import numpy as np
import pandas as pd


data_dirpath = '/home/sxperfect/Workspace/tnt/Repo/gabac_result/data'

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

hs_ga_path = '/home/sxperfect/Workspace/tnt/Repo/gabac_result/hyperparam_search_ga'
total_tests_per_exp = 200
npops = [4, 5, 10, 20, 25, 50]
ngens = [total_tests_per_exp // npop for npop in npops]
nparams = list(range(1, 6))

# datapoints = np.zeros((
#     len(npops), len(nparams), len(avail_transform), len(data_fnames), np.amax(npops)
# ))

subplot_const = (5, 6)

bash_cmd = "cut -d , -f 4".split(' ')


# for i, ngen_npop_pair in enumerate(zip(npops, ngens)):
#     for j, nparam in enumerate(nparams):

#         start_time_param = time.time()

#         for k, transform in enumerate(avail_transform):
#             start_time = time.time()

#             for l, fname in enumerate(data_fnames):

#                 ngen, npop = ngen_npop_pair
#                 hyperparam_comb = "ngen_{ngen}_npop_{npop}_nparam_{nparam}".format(
#                     ngen=ngen,
#                     npop=npop,
#                     nparam=nparam
#                 )

#                 csv_fpath = ga_csv_fpath(
#                     os.path.join(hs_ga_path, hyperparam_comb),
#                     fname,
#                     transform
#                 )

#                 # df = pd.read_csv(csv_fpath)
#                 # datapoints[i, j, k, l, :ngen] = df['best_encoded_ratio']
                

#                 p = subprocess.Popen([*bash_cmd, csv_fpath], stdout=subprocess.PIPE)
#                 output = p.communicate()[0]
#                 # numbers = [float(num) for num in output.decode().split('\n')[1:-1]]

#                 datapoints[i, j, k, l, :ngen] = np.array(output.decode().split('\n')[1:-1]).astype(float)

#             print('Time per transform {}'.format(time.time() - start_time))

#         print('Time per param {}'.format(time.time() - start_time_param))

for i, ngen_npop_pair in enumerate(zip(ngens, npops)):

    ngen, npop = ngen_npop_pair
    
    for j, nparam in enumerate(nparams):
        
        start_time_param = time.time()

        datapoints = np.zeros((
            ngen,
            len(data_fnames),
        ))

        hyperparam_comb = "ngen_{ngen}_npop_{npop}_nparam_{nparam}".format(
            ngen=ngen,
            npop=npop,
            nparam=nparam
        )

        print(hyperparam_comb)

        for k, transform in enumerate(avail_transform):
            start_time = time.time()

            for l, fname in enumerate(data_fnames):

                csv_fpath = ga_csv_fpath(
                    os.path.join(hs_ga_path, hyperparam_comb),
                    fname,
                    transform
                )


                # p = subprocess.Popen([*bash_cmd, csv_fpath], stdout=subprocess.PIPE)
                # output = p.communicate()[0]

                # datapoints[:, l] = np.array(output.decode().split('\n')[1:-1]).astype(float)

                # try:
                #     df = pd.read_csv(csv_fpath)
                #     datapoints[:, l] = df['best_encoded_ratio']
                # except:
                #     datapoints[:, l] = np.nan

                df = pd.read_csv(csv_fpath)
                datapoints[:, l] = df['best_encoded_ratio']


            print('Time per transform {}'.format(time.time() - start_time))
            df = pd.DataFrame(
                data=datapoints,
                columns=None,
            )

            df.to_csv(
                os.path.join(
                    hs_ga_path,
                    hyperparam_comb, 
                    transform + '.csv'
                ),
                index=False
            )

        print('Time per param {}'.format(time.time() - start_time_param))





