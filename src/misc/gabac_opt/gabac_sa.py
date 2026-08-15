# import copy
import json
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import random

from gabac_conf_gen import GabacConfiguration


# import math
# import ctypes as ct
# from collections import OrderedDict


class SimulatedAnnealingForGabac(object):
    r"""
    Source:
        https://en.wikipedia.org/wiki/Simulated_annealing

    Notations :
        T : temperature. Decreases to 0.
        s : a system state
        E(s) : Energy at s. The function we want to minimize
        ∆E : variation of E, from state s to state s_next
        P(∆E , T) : Probability to move from s to s_next. 
            if  ( ∆E < 0 ) P = 1
                else P = exp ( - ∆E / T) . Decreases as T →  0
    
    Pseudo-code:
        Let s = s0  -- initial state
        For k = 0 through kmax (exclusive):
            T ← temperature(k , kmax)
            Pick a random neighbour state , s_next ← neighbour(s)
            ∆E ← E(s) - E(s_next) 
            If P(∆E , T) ≥ random(0, 1), move to the new state:
                s ← s_next
        Output: the final state s
    """

    def __init__(
            self,
            data,
            seq_transform_id: int,
            kmax: int = 50,
            kt: float = 1,
            mutation_nparam: int = 1,
            ena_roundtrip: bool = True,
            verbose: bool = True,
            debug: bool = False,
    ):
        self.verbose = verbose
        self.debug = debug

        # Gabac-specific parameter
        self.data = data
        self.seq_transform_id = seq_transform_id

        # Hyperparameters
        self.kmax = kmax
        self.kt = kt
        self.mutation_nparam = mutation_nparam

        # Init
        self.gc = GabacConfiguration(
            self.seq_transform_id,
            data,
            ena_roundtrip=ena_roundtrip
        )

        self.result = np.zeros((self.kmax + 1, 8))

        # while True:
        #     first_config = self.gc.generate_random_config()
        #     return_val, enc_length, enc_time = self.gc.run_gabac(self.data, self.gc.json_to_cchar(first_config))
        #     if return_val == GABAC_RETURN.SUCCESS:
        #         break

        first_config = self.gc.generate_random_config()
        return_val, enc_length, enc_time = self.gc.run_gabac(self.data, self.gc.json_to_cchar(first_config))

        self.s0 = first_config
        self.enc_length = enc_length
        self.E0 = enc_length / len(self.data)

        self.result[0, :] = [
            enc_length,
            enc_length,
            enc_length,
            self.E0,
            self.E0,
            self.E0,
            enc_time,
            enc_time]

        if self.verbose:
            print('E{:03d}: {:.3f}'.format(0, self.E0))

    def _temperature(
            self,
            k,
    ):
        return (1 - k / self.kmax) * self.kt

    def start(self):
        s = self.s0
        el = self.enc_length
        E = self.E0

        best_s = s
        best_el = el
        best_E = E

        for k in range(self.kmax):
            T = self._temperature(k)

            while True:
                new_s = self.gc.mutate_nparams(s, nparams=self.mutation_nparam)
                # new_s = self.gc.generate_random_neighbor(s)

                if self.debug:
                    with open('prev_config.json', 'w') as f:
                        json.dump(s, f, indent=4)

                    with open('curr_config.json', 'w') as f:
                        json.dump(new_s, f, indent=4)

                return_val, enc_length, enc_time = self.gc.run_gabac(self.data, self.gc.json_to_cchar(new_s))
                # if return_val == GABAC_RETURN.SUCCESS:

                # if return_val == GABAC_RETURN.FAILURE:
                #     raise ValueError

                if True:
                    break

            new_E = enc_length / len(self.data)

            dE = new_E - E

            self.result[k + 1, 0] = enc_length
            self.result[k + 1, 3] = new_E
            self.result[k + 1, 6] = enc_time
            self.result[k + 1, 7] = enc_time + self.result[k, 7]

            if dE > 0.0:
                if self.verbose:
                    print('E{:3d}: {:.3f} dE {:.02e} T {:.2e} '.format(
                        k, new_E, dE, T
                    ), end='')

                if np.exp(-dE / T) > random.random():
                    # if np.exp(dE/T) - 1 < random.random():
                    if self.verbose:
                        print('{:.2f} accept'.format(np.exp(-dE / T)), end='')

                    s = new_s
                    el = enc_length
                    E = new_E

                else:
                    if self.verbose:
                        print('{:.2f}'.format(np.exp(-dE / T)), end='')
                    pass

                if self.verbose:
                    print()

            else:
                if self.verbose:
                    print('E{:3d}: {:.3f} dE {:.02e} (<) '.format(
                        k, new_E, dE
                    ))

                s = new_s
                el = enc_length
                E = new_E

            self.result[k + 1, 1] = el
            self.result[k + 1, 4] = E

            if E < best_E:
                best_s = s
                best_el = el
                best_E = E

            self.result[k + 1, 2] = best_el
            self.result[k + 1, 5] = best_E

        return best_s, best_E

    def show_plot(self):
        plt.plot(np.arange(self.kmax + 1), self.result[:, 3], 'b')
        plt.plot(np.arange(self.kmax + 1), self.result[:, 4], 'r')
        plt.plot(np.arange(self.kmax + 1), self.result[:, 5], 'k')
        plt.show()

    def result_as_csv(self, path, filename):
        df = pd.DataFrame(
            data=self.result,
            index=np.arange(self.kmax + 1),
            columns=[
                'encoded_length',
                'current_encoded_length',
                'best_encoded_length',
                'encoded_ratio',
                'current_encoded_ratio',
                'best_encoded_ratio',
                'encoding_time',
                'total_encoding_time']
        )

        df['filename'] = pd.Series([filename for _ in range(self.kmax + 1)], index=df.index)
        df['iter'] = pd.Series([curr_iter for curr_iter in range(self.kmax + 1)], index=df.index)

        df.to_csv(
            path,
            index=False,
            # header=None,
        )
