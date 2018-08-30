
ToDo List
=========

Common
------

| Subject | Progress | Priority | Remarks |
|---------|:--------:|:--------:|---------|
| Depth textures | 50% | Very High | Depth buffers from render targets can currently *not* be used as textures (only supported with GL renderer) |
| Mobile surface | 50% | High | Special interface for mobile platforms is required (`Surface` -> `Canvas`/`Window` interfaces) |
| Stream outputs | 90% | High | An interface for stream outputs (transform feedback) is required |
| Copy functions | 0% | Medium | Functions for Buffer and Texture copying are required |
| Query arrays | 0% | Low | Queries shall be grouped to arrays with a "QueryArray" interface |
| Atomic counter | 0% | Low | Add "AtomicCounter" interface (GL_ATOMIC_COUNTER_BUFFER, ID3D11Counter) |

| Planned Features | Relevance | Remarks |
|-----------------|:---------:|---------|
| OpenGL ES 3 | High | Since GL and GLES share portions of their API, porting to GLES3 should be feasible |
| OpenGL ES 2 | Low | GLES 2 is rather obsolete |
| Android | High | The most common mobile OS is highly desired |
| Direct3D 9 | Middle | D3D11 is only supported on WinVista+, D3D9 is supported on WinXP+, so it's also worth considering |
| Direct3D 10 | Low | D3D11 and D3D10 are both supported on WinVista+, but D3D11 supports feature levels, so D3D10 has not much relevance |

| Rejected Features | Remarks |
|-------------------|---------|
| Shader class interfaces | Interface for shader classes (also "Subroutines") are considered obsolete (removed from Vulkan GLSL) |
