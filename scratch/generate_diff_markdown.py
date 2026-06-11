import subprocess
import os
import re

def get_git_diff():
    # Run git diff --name-status origin/develop..HEAD
    result = subprocess.run(
        ["git", "diff", "--name-status", "origin/develop..HEAD"],
        capture_output=True, text=True, check=True
    )
    return result.stdout.strip().split("\n")

def categorize_files(diff_lines):
    categories = {
        "Added": {},
        "Modified": {},
        "Deleted": {}
    }
    
    # Module/component mapping patterns
    patterns = [
        ("Core Library (Core)", r"^src/genie/core/"),
        ("Backend Abstractions (Backend)", r"^src/genie/backend/"),
        ("Annotation Module (Annotation)", r"^src/genie/annotation/"),
        ("Contact Module (Contact)", r"^src/genie/contact/"),
        ("Genotype Module (Genotype)", r"^src/genie/genotype/"),
        ("Likelihood Module (Likelihood)", r"^src/genie/likelihood/"),
        ("Utility Library (Util)", r"^src/genie/util/"),
        ("Entropy Codecs (Entropy)", r"^src/genie/entropy/"),
        ("Format Handlers (Format)", r"^src/genie/format/"),
        ("Quality Metrics (Quality)", r"^src/genie/quality/"),
        ("Read Interfaces (Read)", r"^src/genie/read/"),
        ("Name Tokenizers (Name)", r"^src/genie/name/"),
        ("Module Setup (Module)", r"^src/genie/module/"),
        ("Applications (Apps)", r"^src/apps/"),
        ("Third-Party Libraries (Thirdparty)", r"^thirdparty/"),
        ("Tests & Validation (Test)", r"^test/"),
        ("Build System & Configuration (Build)", r"^(CMakeLists\.txt|cmake/|ci/|Doxyfile\.in|\.clang-format|\.clang-tidy|\.gitignore|Doxyfile)"),
        ("Data & Documentation (Docs)", r"^(data/|docs/|README\.md|USAGE\.md|CONTRIBUTING\.md|LICENSE|GEMINI\.md|TASKS\.md|TASKS_ARCHIVE\.md)"),
    ]
    
    for line in diff_lines:
        if not line:
            continue
        parts = line.split("\t")
        if len(parts) < 2:
            continue
        status = parts[0][0] # Get 'A', 'M', 'D'
        file_path = parts[-1]
        
        status_name = "Modified"
        if status == "A":
            status_name = "Added"
        elif status == "D":
            status_name = "Deleted"
            
        matched_group = "Other / Uncategorized"
        for group_name, pattern in patterns:
            if re.search(pattern, file_path):
                matched_group = group_name
                break
                
        if matched_group not in categories[status_name]:
            categories[status_name][matched_group] = []
        categories[status_name][matched_group].append(file_path)
        
    return categories

def generate_markdown(categories):
    md = []
    md.append("# Complete Comparison Checklist: `origin/develop` vs Local `develop`\n")
    md.append("This document provides a detailed list of all files added, modified, or deleted in the local `develop` branch compared to the original upstream `origin/develop` branch, reflecting the full merge of the **Part 6** codebase.\n")
    
    for status in ["Added", "Deleted", "Modified"]:
        md.append(f"## {status} Files\n")
        groups = categories[status]
        if not groups:
            md.append("*No files in this category.*\n")
            continue
            
        # Sort groups logically
        sorted_group_names = sorted(groups.keys())
        for group_name in sorted_group_names:
            file_paths = sorted(groups[group_name])
            md.append(f"### {group_name} ({len(file_paths)} files)\n")
            for fp in file_paths:
                md.append(f"- [ ] `{fp}`")
            md.append("")
            
    return "\n".join(md)

def main():
    diff_lines = get_git_diff()
    categories = categorize_files(diff_lines)
    markdown_content = generate_markdown(categories)
    
    output_path = "develop_merge_changes.md"
    with open(output_path, "w") as f:
        f.write(markdown_content)
    print(f"Generated merge checklist to: {output_path}")

if __name__ == "__main__":
    main()
