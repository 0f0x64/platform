#pragma once

#include "uiCore.h"
#include <algorithm>
#include <cstring>
#include <utility>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

namespace GameUI
{
    using namespace dx11;

    template <class T> inline void ReleaseResource(T*& resource)
    {
        if (resource)
        {
            resource->Release();
            resource = nullptr;
        }
    }

    class PipelineStateGuard
    {
    private:
        ID3D11DeviceContext* context;
        ID3D11InputLayout* inputLayout = nullptr;
        ID3D11Buffer* vertexBuffer = nullptr;
        ID3D11Buffer* indexBuffer = nullptr;
        UINT vertexStride = 0;
        UINT vertexOffset = 0;
        UINT indexOffset = 0;
        DXGI_FORMAT indexFormat = DXGI_FORMAT_UNKNOWN;
        D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
        ID3D11VertexShader* vertexShader = nullptr;
        ID3D11PixelShader* pixelShader = nullptr;
        ID3D11GeometryShader* geometryShader = nullptr;
        ID3D11HullShader* hullShader = nullptr;
        ID3D11DomainShader* domainShader = nullptr;
        ID3D11ClassInstance* vertexShaderClasses[256] = {};
        ID3D11ClassInstance* pixelShaderClasses[256] = {};
        ID3D11ClassInstance* geometryShaderClasses[256] = {};
        ID3D11ClassInstance* hullShaderClasses[256] = {};
        ID3D11ClassInstance* domainShaderClasses[256] = {};
        UINT vertexShaderClassCount = 256;
        UINT pixelShaderClassCount = 256;
        UINT geometryShaderClassCount = 256;
        UINT hullShaderClassCount = 256;
        UINT domainShaderClassCount = 256;
        ID3D11Buffer* vertexConstants = nullptr;
        ID3D11Buffer* pixelConstants = nullptr;
        ID3D11ShaderResourceView* shaderResources[2] = {};
        ID3D11SamplerState* samplers[2] = {};
        ID3D11BlendState* blendState = nullptr;
        FLOAT blendFactor[4] = {};
        UINT sampleMask = 0;
        ID3D11DepthStencilState* depthState = nullptr;
        UINT stencilReference = 0;
        ID3D11RasterizerState* rasterizerState = nullptr;
        D3D11_VIEWPORT viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
        D3D11_RECT scissors[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
        UINT viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
        UINT scissorCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

    public:
        explicit PipelineStateGuard(ID3D11DeviceContext* renderContext) : context(renderContext)
        {
            context->IAGetInputLayout(&inputLayout);
            context->IAGetPrimitiveTopology(&topology);
            context->IAGetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &vertexOffset);
            context->IAGetIndexBuffer(&indexBuffer, &indexFormat, &indexOffset);
            context->VSGetShader(&vertexShader, vertexShaderClasses, &vertexShaderClassCount);
            context->PSGetShader(&pixelShader, pixelShaderClasses, &pixelShaderClassCount);
            context->GSGetShader(&geometryShader, geometryShaderClasses, &geometryShaderClassCount);
            context->HSGetShader(&hullShader, hullShaderClasses, &hullShaderClassCount);
            context->DSGetShader(&domainShader, domainShaderClasses, &domainShaderClassCount);
            context->VSGetConstantBuffers(0, 1, &vertexConstants);
            context->PSGetConstantBuffers(0, 1, &pixelConstants);
            context->PSGetShaderResources(0, 2, shaderResources);
            context->PSGetSamplers(0, 2, samplers);
            context->OMGetBlendState(&blendState, blendFactor, &sampleMask);
            context->OMGetDepthStencilState(&depthState, &stencilReference);
            context->RSGetState(&rasterizerState);
            context->RSGetViewports(&viewportCount, viewports);
            context->RSGetScissorRects(&scissorCount, scissors);
        }
        PipelineStateGuard(const PipelineStateGuard&) = delete;
        PipelineStateGuard& operator=(const PipelineStateGuard&) = delete;
        ~PipelineStateGuard()
        {
            context->IASetInputLayout(inputLayout);
            context->IASetPrimitiveTopology(topology);
            context->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &vertexOffset);
            context->IASetIndexBuffer(indexBuffer, indexFormat, indexOffset);
            context->VSSetShader(vertexShader, vertexShaderClasses, vertexShaderClassCount);
            context->PSSetShader(pixelShader, pixelShaderClasses, pixelShaderClassCount);
            context->GSSetShader(geometryShader, geometryShaderClasses, geometryShaderClassCount);
            context->HSSetShader(hullShader, hullShaderClasses, hullShaderClassCount);
            context->DSSetShader(domainShader, domainShaderClasses, domainShaderClassCount);
            context->VSSetConstantBuffers(0, 1, &vertexConstants);
            context->PSSetConstantBuffers(0, 1, &pixelConstants);
            context->PSSetShaderResources(0, 2, shaderResources);
            context->PSSetSamplers(0, 2, samplers);
            context->OMSetBlendState(blendState, blendFactor, sampleMask);
            context->OMSetDepthStencilState(depthState, stencilReference);
            context->RSSetState(rasterizerState);
            context->RSSetViewports(viewportCount, viewports);
            context->RSSetScissorRects(scissorCount, scissors);
            ReleaseResource(inputLayout);
            ReleaseResource(vertexBuffer);
            ReleaseResource(indexBuffer);
            ReleaseResource(vertexShader);
            ReleaseResource(pixelShader);
            ReleaseResource(geometryShader);
            ReleaseResource(hullShader);
            ReleaseResource(domainShader);
            ReleaseResource(vertexConstants);
            ReleaseResource(pixelConstants);
            for (auto& item : shaderResources)
            {
                ReleaseResource(item);
            }
            for (auto& item : samplers)
            {
                ReleaseResource(item);
            }
            for (UINT i = 0; i < vertexShaderClassCount; ++i)
            {
                ReleaseResource(vertexShaderClasses[i]);
            }
            for (UINT i = 0; i < pixelShaderClassCount; ++i)
            {
                ReleaseResource(pixelShaderClasses[i]);
            }
            for (UINT i = 0; i < geometryShaderClassCount; ++i)
            {
                ReleaseResource(geometryShaderClasses[i]);
            }
            for (UINT i = 0; i < hullShaderClassCount; ++i)
            {
                ReleaseResource(hullShaderClasses[i]);
            }
            for (UINT i = 0; i < domainShaderClassCount; ++i)
            {
                ReleaseResource(domainShaderClasses[i]);
            }
            ReleaseResource(blendState);
            ReleaseResource(depthState);
            ReleaseResource(rasterizerState);
        }
    };

    class Renderer
    {
    private:
        struct Vertex
        {
            Vec2 position;
            Vec2 uv;
            Color color;
        };
        struct TextTexture
        {
            std::wstring text;
            int width = 0;
            int height = 0;
            int fontPixels = 0;
            ID3D11ShaderResourceView* view = nullptr;
            unsigned lastFrame = 0;
        };
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;
        ID3D11VertexShader* vertexShader = nullptr;
        ID3D11PixelShader* pixelShader = nullptr;
        ID3D11InputLayout* inputLayout = nullptr;
        ID3D11Buffer* vertexBuffer = nullptr;
        ID3D11ShaderResourceView* whiteTexture = nullptr;
        ID3D11SamplerState* samplerState = nullptr;
        ID3D11BlendState* blendState = nullptr;
        ID3D11DepthStencilState* depthState = nullptr;
        ID3D11RasterizerState* rasterizerState = nullptr;
        std::vector<TextTexture> textTextures;
        unsigned frameIndex = 0;
        float viewportWidth = 0;
        float viewportHeight = 0;

        bool CreateUiTexture(int width, int height, const void* pixels,
                             ID3D11ShaderResourceView** view)
        {
            D3D11_TEXTURE2D_DESC desc = {};
            desc.Width = static_cast<UINT>(width);
            desc.Height = static_cast<UINT>(height);
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_IMMUTABLE;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            D3D11_SUBRESOURCE_DATA data = {};
            data.pSysMem = pixels;
            data.SysMemPitch = static_cast<UINT>(width * 4);
            ID3D11Texture2D* texture = nullptr;
            if (FAILED(device->CreateTexture2D(&desc, &data, &texture)))
            {
                return false;
            }
            const HRESULT result = device->CreateShaderResourceView(texture, nullptr, view);
            ReleaseResource(texture);
            return SUCCEEDED(result);
        }

        TextTexture* GetTextTexture(const UIDrawCommand& command)
        {
            const float pixelWidth = std::fabs(command.world.scale.x) * viewportWidth;
            const float pixelHeight = std::fabs(command.world.scale.y) * viewportHeight;
            if (!std::isfinite(pixelWidth) || !std::isfinite(pixelHeight) ||
                !std::isfinite(command.fontSize) || pixelWidth < 1.0f || pixelHeight < 1.0f ||
                command.fontSize <= 0.0f)
            {
                return nullptr;
            }
            const int width = static_cast<int>(std::ceil((std::clamp)(pixelWidth, 1.0f, 4096.0f)));
            const int height =
                static_cast<int>(std::ceil((std::clamp)(pixelHeight, 1.0f, 4096.0f)));
            const float scaledFontSize = command.fontSize * viewportHeight / 1080.0f;
            const int fontPixels =
                static_cast<int>(std::round((std::clamp)(scaledFontSize, 1.0f, 512.0f)));
            for (auto& texture : textTextures)
            {
                if (texture.text == command.text && texture.width == width &&
                    texture.height == height && texture.fontPixels == fontPixels)
                {
                    texture.lastFrame = frameIndex;
                    return &texture;
                }
            }
            HDC textContext = CreateCompatibleDC(nullptr);
            if (!textContext)
            {
                return nullptr;
            }
            BITMAPINFO bitmap = {};
            bitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bitmap.bmiHeader.biWidth = width;
            bitmap.bmiHeader.biHeight = -height;
            bitmap.bmiHeader.biPlanes = 1;
            bitmap.bmiHeader.biBitCount = 32;
            bitmap.bmiHeader.biCompression = BI_RGB;
            void* bits = nullptr;
            HBITMAP surface =
                CreateDIBSection(textContext, &bitmap, DIB_RGB_COLORS, &bits, nullptr, 0);
            HFONT font = CreateFontW(-fontPixels, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                     ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            if (!surface || !bits || !font)
            {
                if (surface)
                {
                    DeleteObject(surface);
                }
                if (font)
                {
                    DeleteObject(font);
                }
                DeleteDC(textContext);
                return nullptr;
            }
            HGDIOBJ oldSurface = SelectObject(textContext, surface);
            HGDIOBJ oldFont = SelectObject(textContext, font);
            if (!oldSurface || oldSurface == HGDI_ERROR || !oldFont || oldFont == HGDI_ERROR)
            {
                if (oldFont && oldFont != HGDI_ERROR)
                {
                    SelectObject(textContext, oldFont);
                }
                if (oldSurface && oldSurface != HGDI_ERROR)
                {
                    SelectObject(textContext, oldSurface);
                }
                DeleteObject(font);
                DeleteObject(surface);
                DeleteDC(textContext);
                return nullptr;
            }
            std::memset(bits, 0, static_cast<size_t>(width) * height * 4);
            SetBkMode(textContext, TRANSPARENT);
            SetTextColor(textContext, RGB(255, 255, 255));
            RECT rect = {0, 0, width, height};
            DrawTextW(textContext, command.text.c_str(), static_cast<int>(command.text.size()),
                      &rect, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX | DT_EDITCONTROL);
            GdiFlush();
            auto* pixels = static_cast<unsigned char*>(bits);
            const size_t pixelCount = static_cast<size_t>(width) * height;
            for (size_t pixel = 0; pixel < pixelCount; ++pixel)
            {
                const size_t offset = pixel * 4;
                const unsigned char coverage = pixels[offset + 1];
                pixels[offset] = 255;
                pixels[offset + 1] = 255;
                pixels[offset + 2] = 255;
                pixels[offset + 3] = coverage;
            }
            TextTexture texture;
            texture.text = command.text;
            texture.width = width;
            texture.height = height;
            texture.fontPixels = fontPixels;
            texture.lastFrame = frameIndex;
            const bool created = CreateUiTexture(width, height, bits, &texture.view);
            SelectObject(textContext, oldFont);
            SelectObject(textContext, oldSurface);
            DeleteObject(font);
            DeleteObject(surface);
            DeleteDC(textContext);
            if (!created)
            {
                return nullptr;
            }
            textTextures.push_back(std::move(texture));
            return &textTextures.back();
        }

        void DrawQuad(const WorldTransform2D& world, Color color, ID3D11ShaderResourceView* texture)
        {
            const Vec2 topLeft = world.TransformPoint({-1, 1});
            const Vec2 topRight = world.TransformPoint({1, 1});
            const Vec2 bottomLeft = world.TransformPoint({-1, -1});
            const Vec2 bottomRight = world.TransformPoint({1, -1});
            if (!std::isfinite(topLeft.x) || !std::isfinite(topLeft.y) ||
                !std::isfinite(topRight.x) || !std::isfinite(topRight.y) ||
                !std::isfinite(bottomLeft.x) || !std::isfinite(bottomLeft.y) ||
                !std::isfinite(bottomRight.x) || !std::isfinite(bottomRight.y))
            {
                return;
            }
            const Vertex vertices[6] = {{topLeft, {0, 0}, color},    {topRight, {1, 0}, color},
                                        {bottomLeft, {0, 1}, color}, {bottomLeft, {0, 1}, color},
                                        {topRight, {1, 0}, color},   {bottomRight, {1, 1}, color}};
            D3D11_MAPPED_SUBRESOURCE mapped = {};
            if (FAILED(context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
            {
                return;
            }
            std::memcpy(mapped.pData, vertices, sizeof(vertices));
            context->Unmap(vertexBuffer, 0);
            context->PSSetShaderResources(0, 1, &texture);
            context->Draw(6, 0);
        }

        void DrawCommand(const UIDrawCommand& command)
        {
            if (command.color.a <= 0.0f)
            {
                return;
            }
            if (!command.textCommand)
            {
                DrawQuad(command.world, command.color, whiteTexture);
                return;
            }
            if (command.text.empty())
            {
                return;
            }
            if (TextTexture* texture = GetTextTexture(command))
            {
                DrawQuad(command.world, command.color, texture->view);
            }
        }

    public:
        Renderer() = default;
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        ~Renderer() { Shutdown(); }

        bool Initialize(ID3D11Device* renderDevice, ID3D11DeviceContext* immediateContext)
        {
            if (device == renderDevice && context == immediateContext && vertexShader)
            {
                return true;
            }
            Shutdown();
            if (!renderDevice || !immediateContext)
            {
                return false;
            }
            device = renderDevice;
            device->AddRef();
            context = immediateContext;
            context->AddRef();
            static const char shaderSource[] = R"(
struct VertexInput
{
    float2 position : POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

PixelInput VS(VertexInput input)
{
    PixelInput output;
    output.position = float4(input.position, 0, 1);
    output.uv = input.uv;
    output.color = input.color;
    return output;
}

Texture2D image : register(t0);
SamplerState linearSampler : register(s0);

float4 PS(PixelInput input) : SV_TARGET
{
    return image.Sample(linearSampler, input.uv) * input.color;
}
)";
            ID3DBlob* vertexShaderCode = nullptr;
            ID3DBlob* pixelShaderCode = nullptr;
            ID3DBlob* compilerMessages = nullptr;
            HRESULT result = D3DCompile(shaderSource, sizeof(shaderSource) - 1, "GameUI", nullptr,
                                        nullptr, "VS", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0,
                                        &vertexShaderCode, &compilerMessages);
            if (compilerMessages)
            {
                OutputDebugStringA(static_cast<const char*>(compilerMessages->GetBufferPointer()));
                ReleaseResource(compilerMessages);
            }
            if (SUCCEEDED(result))
            {
                result = D3DCompile(shaderSource, sizeof(shaderSource) - 1, "GameUI", nullptr,
                                    nullptr, "PS", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0,
                                    &pixelShaderCode, &compilerMessages);
            }
            if (compilerMessages)
            {
                OutputDebugStringA(static_cast<const char*>(compilerMessages->GetBufferPointer()));
                ReleaseResource(compilerMessages);
            }
            if (SUCCEEDED(result))
            {
                result = device->CreateVertexShader(vertexShaderCode->GetBufferPointer(),
                                                    vertexShaderCode->GetBufferSize(), nullptr,
                                                    &vertexShader);
            }
            if (SUCCEEDED(result))
            {
                result = device->CreatePixelShader(pixelShaderCode->GetBufferPointer(),
                                                   pixelShaderCode->GetBufferSize(), nullptr,
                                                   &pixelShader);
            }
            const D3D11_INPUT_ELEMENT_DESC elements[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA,
                 0}};
            if (SUCCEEDED(result))
            {
                result =
                    device->CreateInputLayout(elements, 3, vertexShaderCode->GetBufferPointer(),
                                              vertexShaderCode->GetBufferSize(), &inputLayout);
            }
            ReleaseResource(vertexShaderCode);
            ReleaseResource(pixelShaderCode);
            D3D11_BUFFER_DESC buffer = {};
            buffer.ByteWidth = sizeof(Vertex) * 6;
            buffer.Usage = D3D11_USAGE_DYNAMIC;
            buffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            buffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            if (SUCCEEDED(result))
            {
                result = device->CreateBuffer(&buffer, nullptr, &vertexBuffer);
            }
            D3D11_SAMPLER_DESC sampling = {};
            sampling.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            sampling.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            sampling.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            sampling.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            sampling.MaxLOD = D3D11_FLOAT32_MAX;
            if (SUCCEEDED(result))
            {
                result = device->CreateSamplerState(&sampling, &samplerState);
            }
            D3D11_BLEND_DESC blending = {};
            auto& target = blending.RenderTarget[0];
            target.BlendEnable = TRUE;
            target.SrcBlend = D3D11_BLEND_SRC_ALPHA;
            target.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            target.BlendOp = D3D11_BLEND_OP_ADD;
            target.SrcBlendAlpha = D3D11_BLEND_ONE;
            target.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            target.BlendOpAlpha = D3D11_BLEND_OP_ADD;
            target.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            if (SUCCEEDED(result))
            {
                result = device->CreateBlendState(&blending, &blendState);
            }
            D3D11_DEPTH_STENCIL_DESC depthDesc = {};
            depthDesc.DepthEnable = FALSE;
            depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            depthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
            if (SUCCEEDED(result))
            {
                result = device->CreateDepthStencilState(&depthDesc, &depthState);
            }
            D3D11_RASTERIZER_DESC raster = {};
            raster.FillMode = D3D11_FILL_SOLID;
            raster.CullMode = D3D11_CULL_NONE;
            raster.DepthClipEnable = TRUE;
            raster.ScissorEnable = TRUE;
            if (SUCCEEDED(result))
            {
                result = device->CreateRasterizerState(&raster, &rasterizerState);
            }
            const unsigned char white[] = {255, 255, 255, 255};
            if (SUCCEEDED(result) && !CreateUiTexture(1, 1, white, &whiteTexture))
            {
                result = E_FAIL;
            }
            if (FAILED(result))
            {
                OutputDebugStringA("GameUI renderer initialization failed.\n");
                Shutdown();
                return false;
            }
            return true;
        }

        void Render(const UIDrawList& commands, int width, int height)
        {
            if (!device || !context || !vertexShader || width <= 0 || height <= 0)
            {
                return;
            }
            viewportWidth = static_cast<float>(width);
            viewportHeight = static_cast<float>(height);
            ++frameIndex;
            PipelineStateGuard guard(context);
            const UINT stride = sizeof(Vertex);
            const UINT offset = 0;
            context->IASetInputLayout(inputLayout);
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
            context->VSSetShader(vertexShader, nullptr, 0);
            context->PSSetShader(pixelShader, nullptr, 0);
            context->GSSetShader(nullptr, nullptr, 0);
            context->HSSetShader(nullptr, nullptr, 0);
            context->DSSetShader(nullptr, nullptr, 0);
            context->PSSetSamplers(0, 1, &samplerState);
            context->OMSetBlendState(blendState, nullptr, 0xffffffff);
            context->OMSetDepthStencilState(depthState, 0);
            context->RSSetState(rasterizerState);
            const D3D11_VIEWPORT viewport = {0, 0, viewportWidth, viewportHeight, 0, 1};
            const D3D11_RECT clip = {0, 0, width, height};
            context->RSSetViewports(1, &viewport);
            context->RSSetScissorRects(1, &clip);
            for (const UIDrawCommand& command : commands)
            {
                DrawCommand(command);
            }
            ID3D11ShaderResourceView* empty = nullptr;
            context->PSSetShaderResources(0, 1, &empty);
            for (auto it = textTextures.begin(); it != textTextures.end();)
            {
                if (it->lastFrame != frameIndex &&
                    (frameIndex - it->lastFrame > 120 || textTextures.size() > 32))
                {
                    ReleaseResource(it->view);
                    it = textTextures.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        void Shutdown()
        {
            for (auto& texture : textTextures)
            {
                ReleaseResource(texture.view);
            }
            textTextures.clear();
            ReleaseResource(vertexShader);
            ReleaseResource(pixelShader);
            ReleaseResource(inputLayout);
            ReleaseResource(vertexBuffer);
            ReleaseResource(whiteTexture);
            ReleaseResource(samplerState);
            ReleaseResource(blendState);
            ReleaseResource(depthState);
            ReleaseResource(rasterizerState);
            ReleaseResource(context);
            ReleaseResource(device);
        }
    };
} // namespace GameUI
