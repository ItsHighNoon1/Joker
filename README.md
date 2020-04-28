# Joker
Low-level game engine using OpenGL and OpenAL, designed with low overhead in mind.

### Usage
All you have to do is build all this code into a library, then link it to your project and include the header file. There will be actual usage notes later.

### Help! It won't compile!
There are several header files that you need. They should be inside your include directory like so:
- GLFW 3.3+
  - GLFW/glfw3.h
  - GLFW/glfw3native.h
- glm
  - glm/*.hpp
- glad
  - glad/glad.h
- OpenAL
  - AL/al.h
  - AL/alc.h
- stb
  - stb/stb_image.h

In addition, you need to link the binaries for:
- GLFW (glfw3.lib or platform equivalent)
- OpenAL (OpenAL32.lib or platform equivalent)

Finally, your compiler should be using the latest C++ standard (C++17 or later)
