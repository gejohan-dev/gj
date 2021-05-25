# Usage:
# python^
#     generate_opengl_header.py^
#     "c:/path/to/file_with_wglGetProcAddress"^
#     "c:/path/to/file_that_should_contain_definitions"

import re

import sys
assert(len(sys.argv) == 4)
loader_file  = sys.argv[1]
defines_file = sys.argv[2]
defs_file    = sys.argv[3]

header_guard = """#if !defined(OPENGL_H)
#define OPENGL_H\n"""
header_endif = "#endif"

opengl_include = """#include <gl/gl.h>
typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
typedef u32 GLuint;
typedef u8 GLubyte;
"""

global_opengl = "global_variable OpenGL g_opengl = {};\n"

with open(defs_file, "r") as opengl_defs_file:

    opengl_defs = opengl_defs_file.read()
    opengl_defs_file.seek(0)
    
    glcorearb_defines = []
    glcorearb_typedefs = []
    with open("e:\Documents\gejo\libs\glcorearb.h", "r") as glcorearb_file:
        for line in glcorearb_file:
            if line.startswith("#define"): glcorearb_defines.append(line)
            elif line.startswith("GLAPI"):
                line = line.replace("GLAPI", "typedef")
                line = line.replace("APIENTRY", "OPENGL_APIENTRY")
                glcorearb_typedefs.append(line)

    defines_to_add = []
    typedefs_to_add = []
    
    for line in opengl_defs_file:
        if len(line) > 2:
            unstripped_line = line
            line = line.rstrip()
            
            define = list(filter(lambda x: re.search(r'\b' + line + r'\b', x), glcorearb_defines))
            if len(define) == 1:
                defines_to_add.append(define[0])
            elif len(define) != 0: raise Exception(define)
            
            typedef = list(filter(lambda x: re.search(r'\b' + line + r'\b', x), glcorearb_typedefs))
            if len(typedef) == 1:
                typedefs_to_add.append([unstripped_line, typedef[0]])
            elif len(typedef) != 0: raise Exception(typedef)
            
    output = header_guard
    output += opengl_include
    
    for define in defines_to_add:
        output += define

    ###########################################################################################################
    ## SPECIFIC TO Win32
    ###########################################################################################################
    new_signatures = []
    with open(loader_file, "r") as win32_file:
        file_contents = win32_file.read()
        if "{Start OpenGLFunction}" in file_contents and "{End OpenGLFunction}" in file_contents:
            OpenGLFunction_part = file_contents.split("{Start OpenGLFunction}")[1].split("{End OpenGLFunction}")[0]
            for signature in typedefs_to_add:
                if signature[0].rstrip() not in OpenGLFunction_part:
                    new_signatures.append(signature)
    ###########################################################################################################
    
    union = "union OpenGL\n{\n\tstruct\n\t{\n"
    for typedef in typedefs_to_add:
        type_signature = "type_"+typedef[0].rstrip()
        output += typedef[1].replace(typedef[0].rstrip(), type_signature)
        union += "\t\t" + type_signature + "* " + typedef[0].rstrip() + ";\n"
    union += "\t};\n};\n"

    output += union
    output += global_opengl
    output += header_endif

    with open(defines_file, "w") as header_file:
        header_file.write(output)
        header_file.truncate()

    print("Successfully wrote " + defines_file + " with")
    print(str(len(defines_to_add)) + " defines and " + str(len(typedefs_to_add)) + " function signatures")
    if len(new_signatures) > 0:
        print("### Remember to update win32 opengl init of new functions!!! ###")
        for new_sig in new_signatures:
            print(new_sig)
