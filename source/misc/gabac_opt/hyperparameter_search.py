import sys
import subprocess

process = subprocess.Popen("git rev-parse --show-toplevel".split(), stdout=subprocess.PIPE)
output, error = process.communicate()
root_path = output.strip().decode("utf-8")
sys.path.append(root_path)

import os
import time
import json
import random
import argparse
import itertools as it
import multiprocessing as mp

import tqdm
import numpy as np

from gabac_api import GABAC_TRANSFORM
from gabac_sa import SimulatedAnnealingForGabac
from gabac_ga import GeneticAlgorithmForGabac

def run_ga(
    data,
    exp_params,
    gabac_transform,
    data_fname,
    result_dirpath,
):

    iters_per_data, hyperparameters = exp_params
    ngen, npop = iters_per_data
    nparam = hyperparameters

    transform_name, transform_id = gabac_transform

    ga = GeneticAlgorithmForGabac(
        data, 
        transform_id,
        mutation_nparam=nparam,
        num_populations=npop,
        num_generations=ngen,
        ena_roundtrip=False,
        verbose=False,
        debug=False
    )

    best_config_json, best_encoding_ratio = ga.start()

    with open(
        os.path.join(
            result_dirpath,
            'config', 
            "{fname}_{trans_name}.json".format(fname=data_fname, trans_name=transform_name)
        ),
        'w'
    ) as f:
        json.dump(best_config_json, f, indent=4)

    ga.result_as_csv(
        os.path.join(
            result_dirpath,
            'log', 
            "{fname}_{trans_name}.csv".format(fname=data_fname, trans_name=transform_name)
        ),
        os.path.basename(data_fname)
    )

def run_sa(
        data,
        exp_params,
        gabac_transform,
        data_fname,
        result_dirpath,
    ):

    iters_per_data, hyperparameters = exp_params
    nparam, kt = hyperparameters

    transform_name, seq_transform_id = gabac_transform

    sa = SimulatedAnnealingForGabac(
        data,
        seq_transform_id,
        kmax=iters_per_data,
        kt=kt,
        mutation_nparam=nparam,
        ena_roundtrip=False,
        verbose=False,
        debug=False
    )

    best_config_json, best_encoding_ratio = sa.start()

    with open(
        os.path.join(
            result_dirpath,
            'config', 
            "{fname}_{trans_name}.json".format(fname=data_fname, trans_name=transform_name)
        ),
        'w'
    ) as f:
        json.dump(best_config_json, f, indent=4)

    sa.result_as_csv(
        os.path.join(
            result_dirpath,
            'log', 
            "{fname}_{trans_name}.csv".format(fname=data_fname, trans_name=transform_name)
        ),
        os.path.basename(data_fname)
    )

def worker(lock, queue, callback_f):

    end_proc = False
    while True:
        lock.acquire(block=True)
        
        try:
            if queue.empty():
                end_proc = True
            else:
                queue_entry = queue.get()
                exp_params, gabac_transform, data_dirpath, data_fname, result_dirpath, = queue_entry

                with open(os.path.join(data_dirpath, data_fname), 'rb') as f:
                    data = f.read()

        finally:
            lock.release()

        if end_proc:
            break
        else:
            callback_f(data, exp_params, gabac_transform, data_fname, result_dirpath)

            del data
        

def update_progressbar(lock, queue, total_size, update_time=1):
    end_proc = False

    pbar = tqdm.tqdm(total=total_size, unit='exp', unit_scale=False)
    
    num_remaining_exps = total_size

    while True:
        
        lock.acquire(block=True)
        
        try:
            curr_num_remaining_exps = queue.qsize()

            pbar.update(n=num_remaining_exps-curr_num_remaining_exps)
            num_remaining_exps = curr_num_remaining_exps

            if queue.empty():
                end_proc = True

        finally:
            lock.release()
        
        if end_proc:
            pbar.close()
            break

        time.sleep(update_time)

def main(args):

    if args.num_processes is None:
        num_procs = os.cpu_count()
    else:
        num_procs = args.num_processes

    total_tests_per_exp = 200

    root_result_dirpath = args.result_dirpath
    data_dirpath = args.data_dirpath

    data_fnames = [entry.name for entry in os.scandir(data_dirpath) if entry.is_file()]

    print("Test data found: {} files".format(len(data_fnames)))

    lock = mp.Lock()
    queue = mp.Queue()

    if args.algorithm == 'ga':
        callback_f = run_ga
        npops = [4, 5, 10, 20, 25, 50]
        ngens = [total_tests_per_exp // npop for npop in npops]
        list_iters_per_data = list(zip(ngens, npops))

        # nparams
        list_hyperparams = range(1, 6)

        for iters_per_data, hyperparams in it.product(list_iters_per_data, list_hyperparams):
            ngen, npop = iters_per_data
            nparam = hyperparams

            result_dirpath_format = os.path.join(root_result_dirpath, "ngen_{ngen}_npop_{npop}_nparam_{nparam}")
            result_dirpath = result_dirpath_format.format(ngen=ngen, npop=npop, nparam=nparam)
            os.makedirs(os.path.join(result_dirpath, 'log'), exist_ok=True)
            os.makedirs(os.path.join(result_dirpath, 'config'), exist_ok=True)

    if args.algorithm == 'sa':
        callback_f = run_sa
        list_iters_per_data = [total_tests_per_exp]

        # nparams
        list_nparams = range(1,6)
        list_kt = 10**np.arange(-1, 2, 0.25)
        list_hyperparams = list(it.product(list_nparams, list_kt))

        for iters_per_data, hyperparams in it.product(list_iters_per_data, list_hyperparams):
            niter = iters_per_data
            nparam, kt = hyperparams

            result_dirpath_format = os.path.join(root_result_dirpath, "niter_{niter}_nparam_{nparam}_kt_{kt:.2e}")
            result_dirpath = result_dirpath_format.format(niter=niter, nparam=nparam, kt=kt)
            os.makedirs(os.path.join(result_dirpath, 'log'), exist_ok=True)
            os.makedirs(os.path.join(result_dirpath, 'config'), exist_ok=True)

    exp_params = it.product(list_iters_per_data, list_hyperparams)

    avail_transform = {
        "NONE"      : GABAC_TRANSFORM.NONE,
        "RLE"       : GABAC_TRANSFORM.RLE,
        "EQUALITY"  : GABAC_TRANSFORM.EQUALITY,
        "MATCH"     : GABAC_TRANSFORM.MATCH
    }

    for exp_params in exp_params:
        for gabac_transform in avail_transform.items():
            for data_fname in data_fnames:
                queue.put([
                    exp_params, 
                    gabac_transform,
                    data_dirpath,
                    data_fname,
                    result_dirpath,
                ])

    procs = [ 
        mp.Process(
            target=update_progressbar, 
            args=(lock, queue, queue.qsize()), 
            kwargs={'update_time' : 15}
        )
    ]

    for _ in range(num_procs):
        procs.append(
            mp.Process(
                target=worker, 
                args=(lock, queue, callback_f)
            )
        )

    for proc in procs:
        proc.start()

    for proc in procs:
        proc.join()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Hyperparameter Search')

    parser.add_argument('-j' , '--num_processes', required=False, default=None, type=int, help='Number of processes')
    parser.add_argument('-i' , '--data_dirpath', required=True, type=str, help='Path to data directory')
    parser.add_argument('-o' , '--result_dirpath', required=True, type=str, help='Path to result directory')
    parser.add_argument('-a' , '--algorithm', required=True, type=str, choices=['ga', 'sa'], help='Algorithm used for searching the best config')

    args = parser.parse_args()

    main(args)