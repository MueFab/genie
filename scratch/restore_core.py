import subprocess
import os

def run_cmd(args):
    result = subprocess.run(args, capture_output=True, text=True, check=True)
    return result.stdout.strip()

def main():
    repo_dir = "/home/adhisant/workspace/genie-part6"
    os.chdir(repo_dir)

    # Get list of files in src/genie/core on develop
    dev_files = run_cmd(['git', 'ls-tree', '-r', '--name-only', 'develop', 'src/genie/core']).split('\n')
    dev_files = [f for f in dev_files if f]

    # Get list of files in src/genie/core on HEAD (develop-part6 sandbox)
    head_files = run_cmd(['git', 'ls-tree', '-r', '--name-only', 'HEAD', 'src/genie/core']).split('\n')
    head_files = [f for f in head_files if f]

    common_files = list(set(dev_files) & set(head_files))
    common_files.sort()

    print(f"Checking out {len(common_files)} common files from develop...")
    for f in common_files:
        run_cmd(['git', 'checkout', 'develop', '--', f])
        print(f"  Restored: {f}")

    # Now handle access_unit.cc / h which were renamed to access_unit/access_unit.cc / h
    # We retrieve the file contents of these from develop and write them to their new locations
    print("Restoring access_unit.cc/h from develop to access_unit/ subdirectory...")
    
    au_cc_content = run_cmd(['git', 'show', 'develop:src/genie/core/access_unit.cc'])
    with open('src/genie/core/access_unit/access_unit.cc', 'w') as f:
        f.write(au_cc_content)
    print("  Restored src/genie/core/access_unit/access_unit.cc from develop:src/genie/core/access_unit.cc")

    au_h_content = run_cmd(['git', 'show', 'develop:src/genie/core/access_unit.h'])
    with open('src/genie/core/access_unit/access_unit.h', 'w') as f:
        f.write(au_h_content)
    print("  Restored src/genie/core/access_unit/access_unit.h from develop:src/genie/core/access_unit.h")

if __name__ == "__main__":
    main()
