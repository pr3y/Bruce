import sys

def replace_placeholder(index_file, generated_file, placeholder="<!-- BOARDS -->"):
    with open(index_file, 'r', encoding='utf-8') as f:
        index_content = f.read()

    with open(generated_file, 'r', encoding='utf-8') as f:
        generated_content = f.read()

    if placeholder in index_content:
        new_content = index_content.replace(placeholder, generated_content)
        with open(index_file, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"Placeholder '{placeholder}' substituted successfully.")
    else:
        print(f"Placeholder '{placeholder}' not found in {index_file}.")
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python replace_boards.py <index_file> <generated_file>")
        sys.exit(1)

    index_file = sys.argv[1]
    generated_file = sys.argv[2]

    replace_placeholder(index_file, generated_file)
