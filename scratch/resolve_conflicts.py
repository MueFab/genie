import sys
import re

def resolve_conflicts(filepath):
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Pattern to find conflict blocks
    # Favoring the part after ======= (theirs)
    pattern = re.compile(r'<<<<<<< HEAD.*?=======([\s\S]*?)>>>>>>>.*?\n', re.MULTILINE)
    
    new_content = pattern.sub(r'\1', content)
    
    with open(filepath, 'w') as f:
        f.write(new_content)

if __name__ == "__main__":
    resolve_conflicts(sys.argv[1])
