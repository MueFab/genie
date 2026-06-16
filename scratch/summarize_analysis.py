import os

def main():
    analysis_dir = "/home/adhisant/workspace/genie-part6/docs/analysis"
    files = sorted(os.listdir(analysis_dir))
    
    for filename in files:
        if not filename.endswith(".md"):
            continue
        filepath = os.path.join(analysis_dir, filename)
        size = os.path.getsize(filepath)
        print(f"=== File: {filename} (Size: {size} bytes) ===")
        with open(filepath, 'r') as f:
            lines = f.readlines()
        
        # Print first 20 lines or headers
        header_count = 0
        for i, line in enumerate(lines):
            if i < 15:
                print(f"  {line.strip()}")
            elif line.startswith("#"):
                print(f"  {line.strip()}")
                header_count += 1
                if header_count > 10:
                    break
        print("\n")

if __name__ == "__main__":
    main()
