import sys

def check_braces(filename):
    with open(filename, 'r') as f:
        balance = 0
        for i, line in enumerate(f, 1):
            # Strip comments for brace counting
            stripped_line = line.split('//')[0]
            for char in stripped_line:
                if char == '{':
                    balance += 1
                elif char == '}':
                    balance -= 1
            if balance < 0:
                print(f"Negative balance at line {i}: {line.strip()}")
                return
        if balance != 0:
            print(f"Unbalanced at end: {balance}")
        else:
            print("Balanced")

check_braces('test/genotype/structure_test.cc')
