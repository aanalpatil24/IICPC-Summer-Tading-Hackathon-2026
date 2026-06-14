import os
import re

def extract_project(input_file="monolithic_code.txt"):
    if not os.path.exists(input_file):
        print(f"❌ Error: Could not find '{input_file}' in the current directory.")
        print("Please save the code block into a file named 'monolithic_code.txt' and try again.")
        return

    current_filepath = None
    current_content = []
    
    # Regex pattern to exactly match the delimiter line
    delimiter_pattern = re.compile(r"^==================== FILE:\s*(.+?)\s*====================$")

    print("⏳ Starting extraction...\n")

    with open(input_file, "r", encoding="utf-8") as f:
        for line in f:
            match = delimiter_pattern.match(line.strip())
            
            if match:
                # If we were already reading a file, save it before moving to the next one
                if current_filepath:
                    save_file(current_filepath, current_content)
                
                # Update the tracker to the new file path and reset content
                current_filepath = match.group(1).strip()
                current_content = []
            else:
                # If we are currently inside a file block, append the line
                if current_filepath is not None:
                    current_content.append(line)

    # Save the very last file in the document
    if current_filepath:
        save_file(current_filepath, current_content)

    print("\n✅ Project extraction complete! Your folder structure is ready.")

def save_file(filepath, content):
    # Ensure the directory exists
    directory = os.path.dirname(filepath)
    if directory:
        os.makedirs(directory, exist_ok=True)
        
    # Write the content to the file
    with open(filepath, "w", encoding="utf-8") as f:
        f.writelines(content)
        
    print(f"Created: {filepath}")

if __name__ == "__main__":
    extract_project("monolithic_code.txt")