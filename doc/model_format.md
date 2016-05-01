# Internal model format for FreeSpace Open

This is a binary format description for storing 3D model data for the FreeSpace Open engine.

All multi-byte data is stored in little endian encoding. Floating point numbers use IEEE 754 encoding.

It is using a chunk mechanism for storing data. A chunk begins with a 4 byte identifier followed by a 64 bit integer
specifying the length in bytes of this chunk. If the reader encounters an unknown chunk type the chunk must be skipped.

All 3D data are stored as independent triangles (e.g. GL_TRIANGLES in OpenGL)

## Data Types
 Name | Description
--------|-------------------------------------------
 vec*n* | An n-dimensional vector consisting of n floating point numbers
 mat*n* | A nxn matrix consisting of n * n floating point numbers. The matrix is stored in column major format
 (u)int*n* | An (un)signed n-bit integer
 string | A null terminated UTF-8 string

# Versioning
The file header contains a version number. The version number must be incremented if the internal format of a chunk is
changed. Adding new chunk types does not require incrementing the version number because unknown chunks are skipped by
the engine. If the new chunk type somehow breaks backward compatibility then the version must be incremented.
If the engine does not support the version of a model file then it should not be used by the engine!

# File Header
 Length | Description
--------|-------------------------------------------
 8      | Format identifier. Must be the ASCII string "FSOMODEL"
 4      | Version number

# Chunk types
## Vertex data
Vertex data contains multiple data channels for each vertex. The data is interleaved and is structured in the following way:
```c
struct VertexData {
    vec3 position;
    vec2 tex_coord;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
}
```
This struct is packed and not aligned in any way.

 Length | Description
--------|-------------------------------------------
 4      | Identifier ("VDAT")
 8      | Length
 var    | A collection of VertexData structs. This struct normally doesn't need special handling and can be sent directly to the GPU

## Index data
Index data can be used to generate GPU index buffers.


 Length | Description
--------|-------------------------------------------
 4      | Identifier ("INDX")
 8      | Length
 var    | Index data. These are unsigned 16-bit integers. The actual referenced vertex can only be determined with a submodel. Can also be sent directly to the GPU.
