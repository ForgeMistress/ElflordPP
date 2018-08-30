/*
 * Example.cpp (Example_BufferArray)
 *
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <ExampleBase.h>


class Example_BufferArray : public ExampleBase
{

    LLGL::ShaderProgram*    shaderProgram       = nullptr;

    LLGL::GraphicsPipeline* pipeline            = nullptr;

    LLGL::Buffer*           vertexBuffers[3]    = { nullptr };
    LLGL::BufferArray*      vertexBufferArray   = nullptr;

public:

    Example_BufferArray() :
        ExampleBase { L"LLGL Example: BufferArray" }
    {
        // Create all graphics objects
        auto vertexFormats = CreateBuffers();
        shaderProgram = LoadStandardShaderProgram(vertexFormats);
        CreatePipelines();
    }

    std::vector<LLGL::VertexFormat> CreateBuffers()
    {
        // Specify vertex formats
        LLGL::VertexFormat vertexFormatPositions;
        vertexFormatPositions.inputSlot = 0;
        vertexFormatPositions.AppendAttribute({ "position", LLGL::Format::RG32Float });

        LLGL::VertexFormat vertexFormatColors;
        vertexFormatColors.inputSlot = 1;
        vertexFormatColors.AppendAttribute({ "color", LLGL::Format::RGB32Float });

        LLGL::VertexFormat vertexFormatInstanceData;
        vertexFormatInstanceData.inputSlot = 2;
        vertexFormatInstanceData.AppendAttribute({ "instanceColor",  LLGL::Format::RGB32Float, 1 });
        vertexFormatInstanceData.AppendAttribute({ "instanceOffset", LLGL::Format::RG32Float,  1 });
        vertexFormatInstanceData.AppendAttribute({ "instanceScale",  LLGL::Format::R32Float,   1 });

        // Initialize buffer data
        Gs::Vector2f vertexPositions[] =
        {
            {  0,  1 },
            {  1, -1 },
            { -1, -1 },
        };

        LLGL::ColorRGBf vertexColors[] =
        {
            { 1, 0, 0 },
            { 0, 1, 0 },
            { 0, 0, 1 },
        };

        struct InstanceData
        {
            LLGL::ColorRGBf color;
            Gs::Vector2f    offset;
            float           scale;
        }
        instanceData[] =
        {
            { { 1   , 1   , 1    }, { -0.5f,  0.5f },  0.4f },
            { { 1   , 2   , 3    }, {  0.5f,  0.5f }, -0.4f },
            { { 1   , 0.2f, 0.2f }, {  0.5f, -0.5f },  0.2f },
            { { 0.2f, 1   , 0.2f }, { -0.5f, -0.5f },  0.3f },
        };

        // Create buffer for vertex positions
        LLGL::BufferDescriptor desc;

        desc.type                   = LLGL::BufferType::Vertex;
        desc.size                   = sizeof(vertexPositions);
        desc.vertexBuffer.format    = vertexFormatPositions;

        vertexBuffers[0] = renderer->CreateBuffer(desc, vertexPositions);

        // Create buffer for vertex colors
        desc.size                   = sizeof(vertexColors);
        desc.vertexBuffer.format    = vertexFormatColors;

        vertexBuffers[1] = renderer->CreateBuffer(desc, vertexColors);

        // Create buffer for instance data
        desc.size                   = sizeof(instanceData);
        desc.vertexBuffer.format    = vertexFormatInstanceData;

        vertexBuffers[2] = renderer->CreateBuffer(desc, instanceData);

        // Create vertex buffer array
        vertexBufferArray = renderer->CreateBufferArray(3, vertexBuffers);

        return { vertexFormatPositions, vertexFormatColors, vertexFormatInstanceData };
    }

    void CreatePipelines()
    {
        // Create common graphics pipeline for scene rendering
        LLGL::GraphicsPipelineDescriptor pipelineDesc;
        {
            pipelineDesc.shaderProgram              = shaderProgram;
            pipelineDesc.rasterizer.multiSampling   = LLGL::MultiSamplingDescriptor(8);
        }
        pipeline = renderer->CreateGraphicsPipeline(pipelineDesc);
    }

private:

    void OnDrawFrame() override
    {
        commands->Begin();
        {
            // Set buffer array
            commands->SetVertexBufferArray(*vertexBufferArray);

            // Set the render context as the initial render target
            commands->BeginRenderPass(*context);
            {
                // Clear color buffer
                commands->Clear(LLGL::ClearFlags::Color);

                // Set viewports
                commands->SetViewport(LLGL::Viewport{ { 0, 0 }, context->GetVideoMode().resolution });

                // Set graphics pipeline state
                commands->SetGraphicsPipeline(*pipeline);

                // Draw 4 instances of the triangle with 3 vertices each
                commands->DrawInstanced(3, 0, 4);
            }
            commands->EndRenderPass();
        }
        commands->End();
        commandQueue->Submit(*commands);

        // Present result on the screen
        context->Present();
    }

};

LLGL_IMPLEMENT_EXAMPLE(Example_BufferArray);



