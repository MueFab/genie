import os
# import copy
import json
# import random
# import math
# import ctypes as ct
# from collections import OrderedDict

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

from gabac_api.c_api import (
    libgabac,
    gabac_stream,
    gabac_io_config,
    gabac_data_block
)
from gabac_api.const import (
    GABAC_BINARIZATION,
    GABAC_CONTEXT_SELECT,
    GABAC_LOG_LEVEL,
    GABAC_LOG_LEVEL,
    GABAC_OPERATION,
    GABAC_RETURN,
    GABAC_STREAM_MODE,
    GABAC_TRANSFORM
)
from gabac_api import root_path
from test_python_api import array, libc

from .gabac_conf_gen import GabacConfiguration


class GeneticAlgorithmForGabac(object):

    def __init__(
            self,
            data,
            seq_transform_id,
            mutation_prob=None,
            mutation_nparam=None,
            num_populations=100,
            num_generations=100,
            ena_roundtrip=True,
            verbose=True,
            debug=False,
    ):

        self.verbose = verbose
        self.debug = debug

        # Gabac-specific parameter
        self.data = data
        self.seq_transform_id = seq_transform_id

        # Parameter
        self.mutation_prob = mutation_prob
        self.num_populations = num_populations
        self.num_generations = num_generations

        self.mutation_prob = mutation_prob
        self.mutation_nparam = mutation_nparam

        # Init
        self.gc = GabacConfiguration(
            self.seq_transform_id,
            data,
            ena_roundtrip=ena_roundtrip
        )

    def init(self):

        # self.populations = []
        # for _ in range(self.num_populations):
        #     self.populations.append(
        #         self.gc.generate_random_config()
        #     )
        self.populations = [None for _ in range(self.num_populations)]

        # Store enc_length, enc_ratio, enc_time
        self.fitness = np.zeros((
            self.num_populations, 3
        ))

        self.result = np.zeros((self.num_generations, 6))

    def evaluate_fitness(self):

        for idx, config in enumerate(self.populations):

            if self.debug:
                with open('curr_config.json', 'w') as f:
                    json.dump(config, f, indent=4)

            __, enc_length, enc_time = self.gc.run_gabac(self.data, self.gc.json_to_cchar(config))
            enc_ratio = enc_length / len(self.data)

            # if enc_ratio > 20:
            #     raise ValueError

            self.fitness[idx, :] = [enc_length, enc_ratio, enc_time]

            # print("Gen {:03d} idx {:03d} size {:.3f}".format(curr_gen, idx, self.fitness[idx]))
            if self.debug:
                print("Idx {:03d} size {:.3f}".format(idx, self.fitness[idx, 1]))

    def start(self):
        self.init()

        local_max_config = None
        local_max_enc_len = np.Infinity
        local_max_enc_ratio = np.Infinity

        global_max_config = local_max_config
        global_max_enc_len = local_max_enc_len
        global_max_enc_ratio = local_max_enc_ratio

        curr_enc_time = 0

        for curr_gen in range(self.num_generations):
            if local_max_config is None:
                for idx in range(self.num_populations):
                    self.populations[idx] = self.gc.generate_random_config()
            else:

                if self.debug:
                    with open('prev_config.json', 'w') as f:
                        json.dump(local_max_config, f, indent=4)

                for idx in range(self.num_populations):
                    self.populations[idx] = self.gc.mutate_nparams(local_max_config, self.mutation_nparam)

            self.evaluate_fitness()
            local_max_config_idx = np.argmin(self.fitness[:, 0])

            local_max_enc_len, local_max_enc_ratio = self.fitness[local_max_config_idx, 0:2]
            local_max_config = self.populations[local_max_config_idx]

            if local_max_enc_len < global_max_enc_len:
                global_max_config = local_max_config
                global_max_enc_len = local_max_enc_len
                global_max_enc_ratio = local_max_enc_ratio

            curr_enc_time += np.sum(self.fitness[:, 2])

            self.result[curr_gen, :] = [
                local_max_enc_len,
                global_max_enc_len,
                local_max_enc_ratio,
                global_max_enc_ratio,
                np.sum(self.fitness[:, 2]),
                curr_enc_time,
            ]

        return global_max_config, global_max_enc_ratio

    def show_plot(self):
        plt.plot(np.arange(self.num_generations), self.result[:, 2], 'r')
        plt.plot(np.arange(self.num_generations), self.result[:, 3], 'k')
        plt.show()

    def result_as_csv(self, path, filename):
        df = pd.DataFrame(
            data=self.result,
            index=np.arange(self.num_generations),
            columns=[
                'current_encoded_length',
                'best_encoded_length',
                'current_encoded_ratio',
                'best_encoded_ratio',
                'encoding_time',
                'total_encoding_time']
        )

        df['filename'] = pd.Series([filename] * self.num_generations, index=df.index)
        df['iter'] = pd.Series([i_gen * self.num_populations for i_gen in range(1, self.num_generations + 1)],
                               index=df.index)

        df.to_csv(
            path,
            index=False,
            # header=None,
        )
