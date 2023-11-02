import os
import sys

while True:

    # Read GLSL code from a file
    fileName = input("Full path of file: ")


    # Get the parent directory of the script
    parent_dir = os.path.dirname(os.path.abspath(__file__))

    # Construct the full path to the GLSL file in the parent directory
    glsl_file_path = os.path.join(parent_dir, fileName)

    with open(glsl_file_path, "r") as glsl_file:
        glsl_code = glsl_file.read()

    # Process the GLSL code and convert it into a C string
    c_string = ""
    lines = glsl_code.splitlines()
    for line in lines:
        c_string += f'"{line}\\n"\n'

    # Print the C string
    print( c_string)
