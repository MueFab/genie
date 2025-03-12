import os


# Function to search for files containing the exact line '* @file'
def find_files_with_exact_at_file(directory):
    matching_files = []

    for root, _, files in os.walk(directory):
        for file in files:
            file_path = os.path.join(root, file)
            try:
                with open(file_path, 'r') as f:
                    lines = f.readlines()
                    if any(line.strip() == '* @file' for line in lines):
                        matching_files.append(file_path)
            except (UnicodeDecodeError, IOError):
                # Skip files that can't be read
                continue

    return matching_files


if __name__ == "__main__":
    # Change this to your target directory
    DIRECTORY_TO_SEARCH = "."

    files = find_files_with_exact_at_file(DIRECTORY_TO_SEARCH)
    if files:
        print("Files containing the exact line '* @file':")
        for file in files:
            print(file)
    else:
        print("No files containing the exact line '* @file' were found.")
