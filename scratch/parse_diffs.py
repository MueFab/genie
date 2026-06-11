import re

def main():
    filepath = "tmp/core-diff-analysis.txt"
    with open(filepath, "r") as f:
        content = f.read()

    files = content.split("File: ")
    print(f"Total entries: {len(files) - 1}")

    semantic_files = []
    cosmetic_files = []

    for entry in files[1:]:
        lines = entry.strip().split("\n")
        filename = lines[0].strip()
        diff_lines = lines[1:]

        # Filter added lines that are not just idx_i or comments or simple formatting
        has_semantic = False
        added_lines = []
        for line in diff_lines:
            if line.startswith("+") and not line.startswith("+++"):
                # Clean line
                clean = line[1:].strip()
                # Skip comments
                if clean.startswith("//") or clean.startswith("/*") or clean.startswith("*"):
                    continue
                # Skip lines that only rename variable index to idx_*
                if re.search(r'\bidx_[a-zA-Z0-9_]+\b', clean) and not re.search(r'\b(class|struct|enum|void|int|float|double|bool|AnnotDesc|AlgoID|DataType)\b', clean):
                    # Check if it's just a loop or assignment with idx_
                    if "for" in clean or "while" in clean or "=" in clean or "idx_" in clean:
                        # Let's count it as likely cosmetic if it's just renaming
                        # but we can look closer. For now, let's flag as cosmetic if it's ONLY renaming.
                        pass
                
                # Check for significant words
                if len(clean) > 0:
                    added_lines.append(clean)

        # Let's analyze the added lines to see if they are functional
        functional_additions = []
        for line in added_lines:
            # If it's just changing i to idx_i, e.g. "for (size_t idx_i = 0; ...)"
            # Let's check if the corresponding removed line exists with 'i'
            # We can simplify: if the line contains control flow, class definitions, new functions, or calls new APIs, it is semantic.
            # Otherwise, if it's just variable rename, we can ignore.
            # For robustness, let's print all added lines for each file, but filter out pure idx_i renaming
            if "idx_" in line and len(line) < 100:
                # check if it looks like a rename
                # if it contains other terms like AnnotDesc, new class names, etc., keep it
                if not any(kw in line for kw in ["AnnotDesc", "AlgoID", "DataType", "Variant", "Record", "annotation", "backend"]):
                    continue
            functional_additions.append(line)

        if functional_additions:
            semantic_files.append((filename, functional_additions))
        else:
            cosmetic_files.append(filename)

    print(f"Cosmetic files (no semantic changes, only renames/style/comments): {len(cosmetic_files)}")
    for f in cosmetic_files:
        print(f"  - {f}")

    print(f"\nSemantic files with actual functional changes: {len(semantic_files)}")
    for f, additions in semantic_files:
        print(f"\n{f} ({len(additions)} additions):")
        # Print top 10 additions
        for add in additions[:10]:
            print(f"  + {add}")
        if len(additions) > 10:
            print(f"  ... and {len(additions) - 10} more additions")

if __name__ == "__main__":
    main()
