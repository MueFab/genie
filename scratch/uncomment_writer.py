import os
import re

directories = [
    "src/genie/core/access_unit/annotation",
    "src/genie/core/parameter/annotation"
]

h_regex = re.compile(r'^(\s*)//\s*(void write\(core::Writer&.*)')
cc_regex_start = re.compile(r'^(\s*)//\s*(void\s+\w+::write\(core::Writer&.*)')
cc_regex_line = re.compile(r'^(\s*)//\s*(.*)')

def main():
    for directory in directories:
        for filename in os.listdir(directory):
            if not (filename.endswith(".h") or filename.endswith(".cc")):
                continue
            filepath = os.path.join(directory, filename)
            with open(filepath, 'r') as f:
                lines = f.readlines()
            
            modified = False
            new_lines = []
            in_commented_block = False
            brace_count = 0
            
            for line in lines:
                if filename.endswith(".h"):
                    m = h_regex.match(line)
                    if m:
                        line = m.group(1) + m.group(2) + "\n"
                        modified = True
                elif filename.endswith(".cc"):
                    if not in_commented_block:
                        m = cc_regex_start.match(line)
                        if m:
                            in_commented_block = True
                            line = m.group(1) + m.group(2) + "\n"
                            modified = True
                            brace_count = line.count("{") - line.count("}")
                    else:
                        m = cc_regex_line.match(line)
                        if m:
                            uncommented_line = m.group(1) + m.group(2) + "\n"
                            brace_count += uncommented_line.count("{") - uncommented_line.count("}")
                            line = uncommented_line
                            modified = True
                            if brace_count <= 0:
                                in_commented_block = False
                new_lines.append(line)
            
            if modified:
                with open(filepath, 'w') as f:
                    f.writelines(new_lines)
                print(f"Uncommented write methods in {filepath}")

if __name__ == "__main__":
    main()
