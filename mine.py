def generate_lines(file_path, num_lines):
    with open(file_path, 'w') as file:
        for i in range(1, num_lines + 1):
            file.write(f"{i}\n")

if __name__ == "__main__":
    file_path = "out.txt"
    num_lines = 1600
    generate_lines(file_path, num_lines)
    print(f"{num_lines} lines written to {file_path}")
