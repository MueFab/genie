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

from gabac_api import GABAC_TRANSFORM
from gabac_sa import SimulatedAnnealingForGabac
from gabac_ga import GeneticAlgorithmForGabac

# def dummy_ga(exp_desc, subexp_desc, paths, *args):

#     ngen, npop, nparam = exp_desc

#     trans_name, trans_id = subexp_desc

#     input_file, result_path = paths

#     # print("ngen:{ngen}_npop_{npop}_nparam_{nparam} {trans_name}-{trans_id} {fname}".format(ngen=ngen, npop=npop, nparam=nparam, trans_name=trans_name, trans_id=trans_id, fname=fname))

#     path_to_subexp = os.path.join(
#         result_path,
#         "ngen_{ngen}_npop_{npop}_nparam_{nparam}".format(ngen=ngen, npop=npop, nparam=nparam)
#     )

#     try:
#         os.makedirs(os.path.join(path_to_subexp, 'log'))
#     except:
#         pass

#     try:
#         os.makedirs(os.path.join(path_to_subexp, 'config'))
#     except:
#         pass

#     with open(os.path.join(path_to_subexp, 'log', "{fname}_{trans_name}.csv".format(fname=fname, trans_name=trans_name)), 'w') as f:
#         f.write('test')
    
#     with open(os.path.join(path_to_subexp, 'config', "{fname}_{trans_name}.json".format(fname=fname, trans_name=trans_name)), 'w') as f:
#         f.write('test')

#     time.sleep(random.random() * 30 + 80)

def run_ga(data, exp_desc, subexp_desc, paths, *args):

    ngen, npop, nparam = exp_desc
    trans_name, trans_id = subexp_desc
    input_file, result_path = paths

    # with open(input_file, 'rb') as f:
    #     data = f.read()

    ga = GeneticAlgorithmForGabac(
        data, 
        trans_id,
        mutation_nparam=nparam,
        num_populations=npop,
        num_generations=ngen,
        ena_roundtrip=False,
        verbose=False,
        debug=False
    )

    s,E = ga.start()

    try:
        os.makedirs(os.path.join(result_path, 'log'))
    except:
        pass

    try:
        os.makedirs(os.path.join(result_path, 'config'))
    except:
        pass

    del data
        
    with open(
        os.path.join(
            result_path,
            'config', 
            "{fname}_{trans_name}.json".format(fname=os.path.basename(input_file), trans_name=trans_name)
        ),
        'w'
    ) as f:
        json.dump(s, f, indent=4)

    ga.result_as_csv(
        os.path.join(
            result_path,
            'log', 
            "{fname}_{trans_name}.csv".format(fname=os.path.basename(input_file), trans_name=trans_name)
        ),
        os.path.basename(input_file)
    )

    del ga

def worker(lock, queue, callback_f):

    end_proc = False
    while True:
        lock.acquire(block=True)
        
        try:
            if queue.empty():
                end_proc = True
            else:
                queue_entry = queue.get()

                exp_desc, subexp_desc, paths = queue_entry

                with open(paths[0], 'rb') as f:
                    data = f.read()

        finally:
            lock.release()

        # callback_f(*queue_entry)
        callback_f(data, exp_desc, subexp_desc, paths)
        del data
        
        if end_proc:
            break

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
    npops = [4, 5, 10, 20, 25, 50]
    ngens = [total_tests_per_exp // npop for npop in npops]
    nparams =  range(1, 6)

    result_path = args.result_path
    data_path = args.data_path

    fnames = []
    for entry in os.scandir(args.data_path):
        if entry.is_file():
            fnames.append(entry.name)

    print("Test data found: {} files".format(len(fnames)))

    lock = mp.Lock()
    queue = mp.Queue()

    if args.algorithm == 'ga':
        callback_f = run_ga
    elif args.algorith == 'sa':
        raise NotImplementedError('SA is not implemented yet')
        callback_f = run_sa
    # callback_f = dummy_ga

    avail_transform = {
        "NONE"      : GABAC_TRANSFORM.NONE,
        "RLE"       : GABAC_TRANSFORM.RLE,
        "EQUALITY"  : GABAC_TRANSFORM.EQUALITY,
        "MATCH"     : GABAC_TRANSFORM.MATCH
    }

    for [ngen, npop], nparam in it.product( zip(ngens, npops), nparams):
        
        exp_param = [ngen, npop, nparam]

        for subexp_param in avail_transform.items():
            for fname in fnames:
                queue.put([
                    exp_param, 
                    subexp_param, 
                    [
                        os.path.join(data_path, fname), 
                        os.path.join(result_path, "ngen_{ngen}_npop_{npop}_nparam_{nparam}".format(ngen=ngen, npop=npop, nparam=nparam))
                    ]
                ])

    procs = [ 
        mp.Process(
            target=update_progressbar, 
            args=(lock, queue, queue.qsize()), 
            kwargs={'update_time' : 1}
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
    parser.add_argument('-i' , '--data_path', required=True, type=str, help='Path to data')
    parser.add_argument('-o' , '--result_path', required=True, type=str, help='Path to result')
    parser.add_argument('-a' , '--algorithm', required=True, type=str, choices=['ga', 'sa'], help='Algorithm used for searching the best config')

    args = parser.parse_args()

    main(args)