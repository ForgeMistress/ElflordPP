[![Build status](https://ci.appveyor.com/api/projects/status/j09x8n07u3byfky0?svg=true)](https://ci.appveyor.com/project/LukasBanana/llgl)

# Low Level Graphics Library (LLGL)

<p align="center"><img src="docu/LLGL_Logo.png"/></p>


## License

[3-Clause BSD License](https://github.com/LukasBanana/LLGL/blob/master/LICENSE.txt)


## Documentation

- [Getting Started with LLGL](docu/GettingStarted/Getting%20Started%20with%20LLGL.pdf) (PDF)
with Introduction, Hello Triangle Tutorial, and Extensibility Example with [GLFW](http://www.glfw.org/)
- [LLGL Reference Manual](docu/refman.pdf) (PDF)
- [LLGL Coding Conventions](docu/CodingConventions/Coding%20Conventions%20for%20LLGL.pdf) (PDF)
- [Tutorials and Examples for C++](examples/Cpp)
- [Examples for C#](examples/CSharp)


## Progress

* **Version**: 0.02 Beta (see [ChangeLog](docu/ChangeLog))

| Renderer | Progress | Remarks |
|----------|:--------:|---------|
| OpenGL | ~90% | |
| Direct3D 11 | ~90% | |
| Direct3D 12 | ~30% | Experimental state |
| Vulkan | ~60% | Experimental state |
| Metal | ~5% | Experimental state |

| Platform | Progress | Remarks |
|----------|:--------:|---------|
| Windows | 100% | Tested on *Windows 10* |
| GNU/Linux | 70% | Tested on *Kubuntu 16* |
| macOS | 70% | Tested on *macOS Sierra* |
| iOS | 1% | Currently not compilable |

| Language Binding | Progress | Remarks |
|------------------|:--------:|---------|
| C# | 30% | Experimental state |


## Build Notes

### Windows

**Visual Studio 2015** or later is required to build LLGL on Windows.

### macOS

**Xcode 9** or later is required to build LLGL on macOS.

### GNU/Linux

The following development libraries are required to build LLGL on GNU/Linux:
- **X11**: `libx11-dev`
- **xf86vidmode**: `libxxf86vm-dev`
- **Xrandr**: `libxrandr-dev`


## Thin Abstraction Layer

```cpp
// Unified Interface:
CommandBuffer::DrawIndexed(std::uint32_t numIndices, std::uint32_t firstIndex);

// OpenGL Implementation:
void GLCommandBuffer::DrawIndexed(std::uint32_t numIndices, std::uint32_t firstIndex) {
    const GLsizeiptr indices = firstIndex * renderState_.indexBufferStride;
    glDrawElements(
        renderState_.drawMode,
        static_cast<GLsizei>(numIndices),
        renderState_.indexBufferDataType,
        reinterpret_cast<const GLvoid*>(indices)
    );
}

// Direct3D 11 Implementation
void D3D11CommandBuffer::DrawIndexed(std::uint32_t numIndices, std::uint32_t firstIndex) {
    context_->DrawIndexed(numIndices, firstIndex, 0);
}

// Direct3D 12 Implementation
void D3D12CommandBuffer::DrawIndexed(std::uint32_t numIndices, std::uint32_t firstIndex) {
    commandList_->DrawIndexedInstanced(numIndices, 1, firstIndex, 0, 0);
}

// Vulkan Implementation
void VKCommandBuffer::DrawIndexed(std::uint32_t numIndices, std::uint32_t firstIndex) {
    vkCmdDrawIndexed(commandBuffer_, numIndices, 1, firstIndex, 0, 0);
}

// Metal implementation
void MTCommandBuffer::DrawIndexed(std::uint32_t numIndices, std::uint32_t firstIndex) {
    if (numPatchControlPoints_ > 0) {
        [renderEncoder_
            drawIndexedPatches:             numPatchControlPoints_
            patchStart:                     static_cast<NSUInteger>(firstIndex) / numPatchControlPoints_
            patchCount:                     static_cast<NSUInteger>(numIndices) / numPatchControlPoints_
            patchIndexBuffer:               nil
            patchIndexBufferOffset:         0
            controlPointIndexBuffer:        indexBuffer_
            controlPointIndexBufferOffset:  indexTypeSize_ * (static_cast<NSUInteger>(firstIndex))
            instanceCount:                  1
            baseInstance:                   0
        ];
    } else {
        [renderEncoder_
            drawIndexedPrimitives:  primitiveType_
            indexCount:             static_cast<NSUInteger>(numIndices)
            indexType:              indexType_
            indexBuffer:            indexBuffer_
            indexBufferOffset:      indexTypeSize_ * static_cast<NSUInteger>(firstIndex)
        ];
    }
}
```


