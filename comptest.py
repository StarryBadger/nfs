import difflib

def compare_files(file1, file2):
    with open(file1, 'r') as f1, open(file2, 'r') as f2:
        lines1 = f1.readlines()
        lines2 = f2.readlines()

    d = difflib.Differ()
    diff = list(d.compare(lines1, lines2))

    for i, line in enumerate(diff):
        if line.startswith('-') or line.startswith('+'):
            print(f"Difference at line {i + 1}: {line.strip()}")

if __name__ == "__main__":
    file_path1 = "out.txt"
    file_path2 = "clientout.txt"

    compare_files(file_path1, file_path2)