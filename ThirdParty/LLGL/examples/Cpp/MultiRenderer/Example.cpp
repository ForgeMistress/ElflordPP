/*
 * Example.cpp (Example_MultiRenderer)
 *
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifdef _WIN32

#include <ExampleBase.h>

#include <LLGL/Platform/NativeHandle.h>


static void RecordCommandBuffer(
    LLGL::RenderSystem*     renderer,
    LLGL::RenderContext*    context,
    LLGL::CommandQueue*     cmdQueue,
    LLGL::CommandBufferExt* cmdBuffer,
    LLGL::GraphicsPipeline* pipeline,
    LLGL::Buffer*           constantBuffer,
    LLGL::Buffer*           vertexBuffer,
    LLGL::Buffer*           indexBuffer,
    LLGL::Sampler*          sampler,
    LLGL::Texture*          texture,
    const LLGL::Viewport&   viewport,
    const Gs::Matrix4f&     wvpMatrix)
{
    // Update constant buffer
    renderer->WriteBuffer(*constantBuffer, 0, &wvpMatrix, sizeof(wvpMatrix));

    cmdBuffer->Begin();
    {
        cmdBuffer->SetVertexBuffer(*vertexBuffer);
        cmdBuffer->SetIndexBuffer(*indexBuffer);

        cmdBuffer->BeginRenderPass(*context);
        {
            // Clear color buffer
            cmdBuffer->SetClearColor({ 0.1f, 0.1f, 0.4f });
            cmdBuffer->Clear(LLGL::ClearFlags::ColorDepth);

            // Set viewport
            cmdBuffer->SetViewport(viewport);

            // Set graphics pipeline and vertex buffer
            cmdBuffer->SetGraphicsPipeline(*pipeline);

            cmdBuffer->SetConstantBuffer(*constantBuffer, 0, LLGL::StageFlags::VertexStage);
            cmdBuffer->SetSampler(*sampler, 0, LLGL::StageFlags::FragmentStage);
            cmdBuffer->SetTexture(*texture, 0, LLGL::StageFlags::FragmentStage);

            // Draw triangulated cube
            cmdBuffer->DrawIndexed(36, 0);
        }
        cmdBuffer->EndRenderPass();
    }
    cmdBuffer->End();
    cmdQueue->Submit(*cmdBuffer);

    // Present the result on the screen
    context->Present();
}

int main(int argc, char* argv[])
{
    try
    {
        // Create main window
        LLGL::WindowDescriptor mainWindowDesc;
        {
            mainWindowDesc.title    = L"LLGL Example: Multi Renderer ( OpenGL and Direct3D 11 )";
            mainWindowDesc.size     = { 800, 600 };
            mainWindowDesc.centered = true;
        }
        auto mainWindow = LLGL::Window::Create(mainWindowDesc);

        // Get native handle (HWND for Win32) from main window
        LLGL::NativeHandle mainWindowHandle;
        mainWindow->GetNativeHandle(&mainWindowHandle);

        // Copy native handle from main window into context handle as parent window
        LLGL::NativeContextHandle mainWindowContextHandle;
        mainWindowContextHandle.parentWindow = mainWindowHandle.window;

        // Create sub window for 1st renderer
        LLGL::WindowDescriptor subWindow0Desc;
        {
            subWindow0Desc.position         = {   0,   0 };
            subWindow0Desc.size             = { 400, 600 };
            subWindow0Desc.borderless       = true;
            subWindow0Desc.visible          = true;
            subWindow0Desc.windowContext    = (&mainWindowContextHandle);
        }
        std::shared_ptr<LLGL::Window> subWindow0 = LLGL::Window::Create(subWindow0Desc);

        // Create sub window for 2nd renderer
        LLGL::WindowDescriptor subWindow1Desc;
        {
            subWindow1Desc.position         = { 400,   0 };
            subWindow1Desc.size             = { 400, 600 };
            subWindow1Desc.borderless       = true;
            subWindow1Desc.visible          = true;
            subWindow1Desc.windowContext    = (&mainWindowContextHandle);
        }
        std::shared_ptr<LLGL::Window> subWindow1 = LLGL::Window::Create(subWindow1Desc);

        // Load render systems
        auto rendererGL = LLGL::RenderSystem::Load("OpenGL");
        auto rendererD3D = LLGL::RenderSystem::Load("Direct3D11");

        // Create render contexts
        LLGL::RenderContextDescriptor contextDesc;
        {
            contextDesc.videoMode.resolution    = { 400, 600 };
            contextDesc.multiSampling           = LLGL::MultiSamplingDescriptor(8);
            contextDesc.profileOpenGL.contextProfile = LLGL::OpenGLContextProfile::CoreProfile;
        }
        auto contextGL = rendererGL->CreateRenderContext(contextDesc, subWindow0);
        auto contextD3D = rendererD3D->CreateRenderContext(contextDesc, subWindow1);

        // Show main window
        mainWindow->Show();

        // Vertex data (3 vertices for our triangle)
        auto cubeVertices = GenerateTexturedCubeVertices();
        auto cubeIndices = GenerateTexturedCubeTriangleIndices();

        // Vertex format
        LLGL::VertexFormat vertexFormat;
        vertexFormat.AppendAttribute({ "position", LLGL::Format::RGB32Float });
        vertexFormat.AppendAttribute({ "texCoord", LLGL::Format::RG32Float  });

        // Create vertex buffers
        const auto vertexBufferDesc = LLGL::VertexBufferDesc(sizeof(VertexPos3Tex2) * cubeVertices.size(), vertexFormat);
        auto vertexBufferGL = rendererGL->CreateBuffer(vertexBufferDesc, cubeVertices.data());
        auto vertexBufferD3D = rendererD3D->CreateBuffer(vertexBufferDesc, cubeVertices.data());

        // Create index buffers
        const auto indexBufferDesc = LLGL::IndexBufferDesc(sizeof(std::uint32_t) * cubeIndices.size(), LLGL::DataType::UInt32);
        auto indexBufferGL = rendererGL->CreateBuffer(indexBufferDesc, cubeIndices.data());
        auto indexBufferD3D = rendererD3D->CreateBuffer(indexBufferDesc, cubeIndices.data());

        // Create constant buffers
        Gs::Matrix4f wvpMatrix;

        const auto constBufferDesc = LLGL::ConstantBufferDesc(sizeof(wvpMatrix));
        auto constBufferGL = rendererGL->CreateBuffer(constBufferDesc);
        auto constBufferD3D = rendererD3D->CreateBuffer(constBufferDesc);

        auto CreateSceneShader = [&vertexFormat](LLGL::RenderSystem* renderer)
        {
            // Create shaders
            LLGL::Shader* vertShader = nullptr;
            LLGL::Shader* fragShader = nullptr;

            const auto& languages = renderer->GetRenderingCaps().shadingLanguages;

            if (std::find(languages.begin(), languages.end(), LLGL::ShadingLanguage::HLSL) != languages.end())
            {
                vertShader = renderer->CreateShader({ LLGL::ShaderType::Vertex,   "Example.hlsl", "VS", "vs_4_0" });
                fragShader = renderer->CreateShader({ LLGL::ShaderType::Fragment, "Example.hlsl", "PS", "ps_4_0" });
            }
            else
            {
                vertShader = renderer->CreateShader({ LLGL::ShaderType::Vertex,   "Example.vert" });
                fragShader = renderer->CreateShader({ LLGL::ShaderType::Fragment, "Example.frag" });
            }

            // Print info log (warnings and errors)
            for (auto shader : { vertShader, fragShader })
            {
                std::string log = shader->QueryInfoLog();
                if (!log.empty())
                    std::cerr << log << std::endl;
            }

            // Create shader program which is used as composite
            LLGL::ShaderProgramDescriptor shaderProgramDesc;
            {
                shaderProgramDesc.vertexFormats     = { vertexFormat };
                shaderProgramDesc.vertexShader      = vertShader;
                shaderProgramDesc.fragmentShader    = fragShader;
            }
            auto shaderProgram = renderer->CreateShaderProgram(shaderProgramDesc);

            if (shaderProgram->HasErrors())
                throw std::runtime_error(shaderProgram->QueryInfoLog());

            return shaderProgram;
        };

        // Create textures
        auto textureGL = LoadTextureWithRenderer(*rendererGL, "../../Media/Textures/Logo_OpenGL.png");
        auto textureD3D = LoadTextureWithRenderer(*rendererD3D, "../../Media/Textures/Logo_Direct3D.png");

        // Create samplers
        LLGL::SamplerDescriptor samplerDesc;
        {
            samplerDesc.maxAnisotropy = 8;
        }
        auto samplerGL = rendererGL->CreateSampler(samplerDesc);
        auto samplerD3D = rendererD3D->CreateSampler(samplerDesc);

        // Create shader programs
        auto shaderProgramGL = CreateSceneShader(rendererGL.get());
        auto shaderProgramD3D = CreateSceneShader(rendererD3D.get());

        // Create graphics pipelines
        LLGL::GraphicsPipelineDescriptor pipelineDescGL;
        {
            pipelineDescGL.shaderProgram            = shaderProgramGL;
            pipelineDescGL.depth.testEnabled        = true;
            pipelineDescGL.depth.writeEnabled       = true;
            pipelineDescGL.rasterizer.multiSampling = contextDesc.multiSampling;
        }
        auto pipelineGL = rendererGL->CreateGraphicsPipeline(pipelineDescGL);

        LLGL::GraphicsPipelineDescriptor pipelineDescD3D;
        {
            pipelineDescD3D.shaderProgram               = shaderProgramD3D;
            pipelineDescD3D.depth.testEnabled           = true;
            pipelineDescD3D.depth.writeEnabled          = true;
            pipelineDescD3D.rasterizer.multiSampling    = contextDesc.multiSampling;
        }
        auto pipelineD3D = rendererD3D->CreateGraphicsPipeline(pipelineDescD3D);

        // Get command queue
        auto commandQueueGL = rendererGL->GetCommandQueue();
        auto commandQueueD3D = rendererD3D->GetCommandQueue();

        // Create command buffers
        auto commandsGL = rendererGL->CreateCommandBufferExt();
        auto commandsD3D = rendererD3D->CreateCommandBufferExt();

        // Set viewports (this guaranteed to be a persistent state)
        const LLGL::Viewport viewportGL  {    0, 0, 800, 600 };
        const LLGL::Viewport viewportD3D { -400, 0, 800, 600 };

        // Initialize matrices (OpenGL needs a unit-cube NDC-space)
        Gs::Matrix4f projMatrixGL, projMatrixD3D, viewMatrix, worldMatrix;

        const float aspectRatio = static_cast<float>(mainWindowDesc.size.width) / static_cast<float>(mainWindowDesc.size.height);
        const float nearPlane   = 0.1f;
        const float farPlane    = 100.0f;
        const float fieldOfView = 45.0f;

        projMatrixGL  = Gs::ProjectionMatrix4f::Perspective(aspectRatio, nearPlane, farPlane, Gs::Deg2Rad(fieldOfView), Gs::ProjectionFlags::UnitCube).ToMatrix4();
        projMatrixD3D = Gs::ProjectionMatrix4f::Perspective(aspectRatio, nearPlane, farPlane, Gs::Deg2Rad(fieldOfView)).ToMatrix4();

        Gs::Translate(viewMatrix, Gs::Vector3f(0, 0, 5));

        // Enter main loop
        while (mainWindow->ProcessEvents())
        {
            // Update scene transformation
            Gs::RotateFree(worldMatrix, Gs::Vector3f(0, 1, 0), Gs::Deg2Rad(0.005f));

            // Draw scene for OpenGL
            {
                // Set transformation matrix for OpenGL
                wvpMatrix = projMatrixGL * viewMatrix * worldMatrix;

                // Record command buffer and present result on screen for OpenGL renderer
                RecordCommandBuffer(
                    rendererGL.get(),
                    contextGL,
                    commandQueueGL,
                    commandsGL,
                    pipelineGL,
                    constBufferGL,
                    vertexBufferGL,
                    indexBufferGL,
                    samplerGL,
                    textureGL,
                    viewportGL,
                    wvpMatrix
                );
            }

            // Draw scene for Direct3D
            {
                // Set transformation matrix for Direct3D
                wvpMatrix = projMatrixD3D * viewMatrix * worldMatrix;

                // Record command buffer and present result on screen for OpenGL renderer
                RecordCommandBuffer(
                    rendererD3D.get(),
                    contextD3D,
                    commandQueueD3D,
                    commandsD3D,
                    pipelineD3D,
                    constBufferD3D,
                    vertexBufferD3D,
                    indexBufferD3D,
                    samplerD3D,
                    textureD3D,
                    viewportD3D,
                    wvpMatrix
                );
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        system("pause");
    }
    return 0;
}

#else

#include <iostream>

int main()
{
    std::cerr << "this tutorial is only available for the Win32 platform" << std::endl;
    return 0;
}

#endif

