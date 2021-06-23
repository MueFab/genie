# @staticmethod
# def json_to_cchar(
#     config
# ):
#     return array(ct.c_char, json.dumps(config, indent=4).encode('utf-8'))

import argparse
# @staticmethod
# def generate_random_cchar_config(
#     seq_transform_id
# ):
#     return GabacConfiguration.json_to_cchar(
#         GabacConfiguration.generate_random_config(
#             seq_transform_id
#         )
#     )import os
import json
from gabac_api import GABAC_TRANSFORM

from gabac_ga import GeneticAlgorithmForGabac
from gabac_sa import SimulatedAnnealingForGabac


def main(args):
    avail_transform = {
        "NONE": GABAC_TRANSFORM.NONE,
        "RLE": GABAC_TRANSFORM.RLE,
        "EQUALITY": GABAC_TRANSFORM.EQUALITY,
        "MATCH": GABAC_TRANSFORM.MATCH
    }

    with open(args.input, 'rb') as f:
        data = f.read()

    if args.algorithm.lower() in ['sa', 'simulated_annealing']:
        for trans_name, trans_id in avail_transform.items():
            sa = SimulatedAnnealingForGabac(
                data,
                trans_id,
                kmax=100,
                kt=1,
                mutation_nparam=2,
                ena_roundtrip=False,
                verbose=False,
                debug=False
            )

            s, E = sa.start()

            with open(os.path.join(args.result, os.path.basename(args.input) + '_' + trans_name + '.json'), 'w') as f:
                json.dump(s, f, indent=4)

            sa.result_as_csv(
                os.path.join(args.result, os.path.basename(args.input) + '_' + trans_name + '.csv'),
                os.path.basename(args.input)
            )

    elif args.algorithm.lower() in ['ga', 'genetic_algorithm']:
        for trans_name, trans_id in avail_transform.items():
            sa = GeneticAlgorithmForGabac(
                data,
                trans_id,
                mutation_nparam=2,
                num_populations=10,
                num_generations=10,
                ena_roundtrip=False,
                verbose=False,
                debug=False
            )

            s, E = sa.start()

            with open(os.path.join(args.result, os.path.basename(args.input) + '_' + trans_name + '.json'), 'w') as f:
                json.dump(s, f, indent=4)

            sa.result_as_csv(
                os.path.join(args.result, os.path.basename(args.input) + '_' + trans_name + '.csv'),
                os.path.basename(args.input)
            )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Python main for regularized auto-encoder')

    parser.add_argument('-a', '--algorithm', required=True, type=str,
                        help='Algorithm used for searching the best config')
    # parser.add_argument('--transform', required=True, type=int, help='Gabac Transform')
    parser.add_argument('-i', '--input', required=True, type=str, help='Path to data')
    parser.add_argument('-r', '--result', required=True, type=str, help='Path to result')

    args = parser.parse_args()

    main(args)
