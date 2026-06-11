import subprocess
import sys

def get_added_files():
    # Get all added files in git diff origin/develop..HEAD
    res = subprocess.run(
        ["git", "diff", "--name-status", "origin/develop..HEAD"],
        capture_output=True, text=True, check=True
    )
    added = []
    for line in res.stdout.strip().split("\n"):
        if not line:
            continue
        parts = line.split("\t")
        if len(parts) >= 2 and parts[0][0] == 'A':
            added.append(parts[-1])
    return added

def was_deleted_in_develop(filepath):
    # Check if this file has a deletion commit in origin/develop's history
    res = subprocess.run(
        ["git", "log", "origin/develop", "--oneline", "--name-status", "--", filepath],
        capture_output=True, text=True, check=True
    )
    # If the output contains 'D\t<filepath>' or similar, it was deleted in origin/develop
    for line in res.stdout.split("\n"):
        if not line:
            continue
        parts = line.split("\t")
        if len(parts) >= 2 and parts[0] == 'D' and filepath in parts[1]:
            return True
    return False

def main():
    added = get_added_files()
    revived = []
    for f in added:
        if was_deleted_in_develop(f):
            revived.append(f)
            print(f"REVIVED: {f}")
            
    print(f"\nTotal revived files: {len(revived)}")

if __name__ == "__main__":
    main()
