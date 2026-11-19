#pragma once

#include <string>
#include <vector>
#include "../../ui/uiRenderer.h"

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")

namespace GameUI
{

    // A small world-space NPC used to exercise the dialogue UI in the game scene.
    class NpcCube
    {
    public:
        NpcCube() = default;
        NpcCube(const NpcCube&) = delete;
        NpcCube& operator=(const NpcCube&) = delete;
        ~NpcCube() { Shutdown(); }

        bool Initialize(dx11::ID3D11Device* device, dx11::ID3D11DeviceContext* context)
        {
            Shutdown();
            device_ = device;
            context_ = context;
            const auto failInitialization = [this](const wchar_t* message) {
                Shutdown();
                return Fail(message);
            };
            if (!device_ || !context_)
            {
                return failInitialization(L"The NPC renderer requires a device and context.");
            }

            static const char shader[] = R"(
cbuffer Frame : register(b0)
{
    float2 resolution;
    float useSceneDepth;
    float padding;
};

struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float shade : COLOR0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float shade : COLOR0;
};

PixelInput VS(VertexInput input)
{
    PixelInput output;
    output.position = input.position;
    output.uv = input.uv;
    output.shade = input.shade;
    return output;
}

Texture2D<float4> image : register(t0);
Texture2D<float> sceneDepth : register(t1);
SamplerState imageSampler : register(s0);

float4 PS(PixelInput input) : SV_TARGET
{
    if (useSceneDepth > 0.5)
    {
        uint depthWidth;
        uint depthHeight;
        sceneDepth.GetDimensions(depthWidth, depthHeight);
        int2 depthSize = int2(depthWidth, depthHeight);
        int2 depthPixel = int2(input.position.xy / resolution * float2(depthSize));
        depthPixel = clamp(depthPixel, int2(0, 0), depthSize - 1);

        float sceneDepthValue = sceneDepth.Load(int3(depthPixel, 0));
        if (input.position.z > sceneDepthValue + 0.0001)
        {
            discard;
        }
    }

    float4 color = image.Sample(imageSampler, input.uv);
    return float4(color.rgb * input.shade, 1);
}
)";
            dx11::ID3DBlob* vertexShaderCode = nullptr;
            dx11::ID3DBlob* pixelShaderCode = nullptr;
            dx11::ID3DBlob* compilerMessages = nullptr;
            HRESULT result =
                dx11::D3DCompile(shader, sizeof(shader) - 1, "NpcCube", nullptr, nullptr, "VS",
                                 "vs_4_0", 0, 0, &vertexShaderCode, &compilerMessages);
            if (compilerMessages)
            {
                OutputDebugStringA(static_cast<const char*>(compilerMessages->GetBufferPointer()));
                Release(compilerMessages);
            }
            if (FAILED(result))
            {
                Release(vertexShaderCode);
                return failInitialization(L"Could not compile the NPC vertex shader.");
            }
            result = dx11::D3DCompile(shader, sizeof(shader) - 1, "NpcCube", nullptr, nullptr, "PS",
                                      "ps_4_0", 0, 0, &pixelShaderCode, &compilerMessages);
            if (compilerMessages)
            {
                OutputDebugStringA(static_cast<const char*>(compilerMessages->GetBufferPointer()));
                Release(compilerMessages);
            }
            if (FAILED(result))
            {
                Release(vertexShaderCode);
                Release(pixelShaderCode);
                return failInitialization(L"Could not compile the NPC pixel shader.");
            }

            result = device_->CreateVertexShader(vertexShaderCode->GetBufferPointer(),
                                                 vertexShaderCode->GetBufferSize(), nullptr,
                                                 &vertexShader_);
            if (SUCCEEDED(result))
            {
                result = device_->CreatePixelShader(pixelShaderCode->GetBufferPointer(),
                                                    pixelShaderCode->GetBufferSize(), nullptr,
                                                    &pixelShader_);
            }
            const dx11::D3D11_INPUT_ELEMENT_DESC elements[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
                 dx11::D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, dx11::D3D11_INPUT_PER_VERTEX_DATA,
                 0},
                {"COLOR", 0, DXGI_FORMAT_R32_FLOAT, 0, 24, dx11::D3D11_INPUT_PER_VERTEX_DATA, 0}};
            if (SUCCEEDED(result))
            {
                result =
                    device_->CreateInputLayout(elements, 3, vertexShaderCode->GetBufferPointer(),
                                               vertexShaderCode->GetBufferSize(), &inputLayout_);
            }
            Release(vertexShaderCode);
            Release(pixelShaderCode);
            if (FAILED(result))
            {
                return failInitialization(L"Could not create the NPC shaders.");
            }

            dx11::D3D11_BUFFER_DESC buffer = {};
            buffer.ByteWidth = sizeof(Vertex) * 36;
            buffer.Usage = dx11::D3D11_USAGE_DYNAMIC;
            buffer.BindFlags = dx11::D3D11_BIND_VERTEX_BUFFER;
            buffer.CPUAccessFlags = dx11::D3D11_CPU_ACCESS_WRITE;
            if (FAILED(device_->CreateBuffer(&buffer, nullptr, &vertexBuffer_)))
            {
                return failInitialization(L"Could not create the NPC vertex buffer.");
            }
            buffer.ByteWidth = 16;
            buffer.BindFlags = dx11::D3D11_BIND_CONSTANT_BUFFER;
            if (FAILED(device_->CreateBuffer(&buffer, nullptr, &frameConstants_)))
            {
                return failInitialization(L"Could not create the NPC constant buffer.");
            }

            dx11::D3D11_RASTERIZER_DESC rasterizer = {};
            rasterizer.FillMode = dx11::D3D11_FILL_SOLID;
            rasterizer.CullMode = dx11::D3D11_CULL_NONE;
            rasterizer.DepthClipEnable = TRUE;
            if (FAILED(device_->CreateRasterizerState(&rasterizer, &rasterizer_)))
            {
                return failInitialization(L"Could not create the NPC rasterizer.");
            }
            dx11::D3D11_DEPTH_STENCIL_DESC depth = {};
            depth.DepthEnable = TRUE;
            depth.DepthWriteMask = dx11::D3D11_DEPTH_WRITE_MASK_ALL;
            depth.DepthFunc = dx11::D3D11_COMPARISON_LESS_EQUAL;
            if (FAILED(device_->CreateDepthStencilState(&depth, &depthState_)))
            {
                return failInitialization(L"Could not create the NPC depth state.");
            }
            dx11::D3D11_SAMPLER_DESC sampler = {};
            sampler.Filter = dx11::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            sampler.AddressU = dx11::D3D11_TEXTURE_ADDRESS_CLAMP;
            sampler.AddressV = dx11::D3D11_TEXTURE_ADDRESS_CLAMP;
            sampler.AddressW = dx11::D3D11_TEXTURE_ADDRESS_CLAMP;
            sampler.MaxLOD = D3D11_FLOAT32_MAX;
            if (FAILED(device_->CreateSamplerState(&sampler, &sampler_)))
            {
                return failInitialization(L"Could not create the NPC sampler.");
            }
            dx11::D3D11_BLEND_DESC blend = {};
            blend.RenderTarget[0].RenderTargetWriteMask = dx11::D3D11_COLOR_WRITE_ENABLE_ALL;
            if (FAILED(device_->CreateBlendState(&blend, &blend_)))
            {
                return failInitialization(L"Could not create the NPC blend state.");
            }

            std::vector<unsigned char> pixels(64 * 64 * 4);
            for (int y = 0; y < 64; ++y)
            {
                for (int x = 0; x < 64; ++x)
                {
                    const bool lightTile = ((x / 8 + y / 8) % 2) == 0;
                    const bool eye =
                        y >= 18 && y < 29 && ((x >= 13 && x < 22) || (x >= 42 && x < 51));
                    const bool smile = y >= 43 && y < 48 && x >= 20 && x < 44;
                    const bool faceDetail = eye || smile;
                    const std::size_t offset = static_cast<std::size_t>((y * 64 + x) * 4);
                    pixels[offset] = faceDetail ? 20 : (lightTile ? 100 : 64);
                    pixels[offset + 1] = faceDetail ? 18 : (lightTile ? 210 : 148);
                    pixels[offset + 2] = faceDetail ? 30 : (lightTile ? 240 : 194);
                    pixels[offset + 3] = 255;
                }
            }
            if (!CreateNpcTexture(pixels.data(), 64, 64, &textureView_))
            {
                return failInitialization(L"Could not create the default NPC texture.");
            }
            error_.clear();
            return true;
        }

        bool LoadTexture(const wchar_t* path)
        {
            if (!device_ || !path || !path[0])
            {
                return Fail(L"No texture file was selected.");
            }
            const HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            if (FAILED(comResult) && comResult != RPC_E_CHANGED_MODE)
            {
                return Fail(L"Could not initialize the image decoder.");
            }
            IWICImagingFactory* factory = nullptr;
            IWICBitmapDecoder* decoder = nullptr;
            IWICBitmapFrameDecode* imageFrame = nullptr;
            IWICFormatConverter* converter = nullptr;
            dx11::ID3D11ShaderResourceView* loadedTexture = nullptr;
            HRESULT result = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                              CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
            if (SUCCEEDED(result))
            {
                result = factory->CreateDecoderFromFilename(path, nullptr, GENERIC_READ,
                                                            WICDecodeMetadataCacheOnLoad, &decoder);
            }
            if (SUCCEEDED(result))
            {
                result = decoder->GetFrame(0, &imageFrame);
            }
            if (SUCCEEDED(result))
            {
                result = factory->CreateFormatConverter(&converter);
            }
            if (SUCCEEDED(result))
            {
                result = converter->Initialize(imageFrame, GUID_WICPixelFormat32bppRGBA,
                                               WICBitmapDitherTypeNone, nullptr, 0,
                                               WICBitmapPaletteTypeCustom);
            }
            UINT width = 0;
            UINT height = 0;
            if (SUCCEEDED(result))
            {
                result = converter->GetSize(&width, &height);
            }
            if (SUCCEEDED(result) && (!width || !height || width > 8192 || height > 8192))
            {
                result = E_INVALIDARG;
            }
            if (SUCCEEDED(result))
            {
                std::vector<unsigned char> pixels(static_cast<std::size_t>(width) * height * 4);
                result = converter->CopyPixels(nullptr, width * 4, static_cast<UINT>(pixels.size()),
                                               pixels.data());
                if (SUCCEEDED(result) &&
                    !CreateNpcTexture(pixels.data(), width, height, &loadedTexture))
                {
                    result = E_FAIL;
                }
            }
            Release(converter);
            Release(imageFrame);
            Release(decoder);
            Release(factory);
            if (SUCCEEDED(comResult))
            {
                CoUninitialize();
            }
            if (FAILED(result))
            {
                Release(loadedTexture);
                return Fail(L"Could not read this image. Use PNG, JPEG, BMP, GIF or TIFF up to "
                            L"8192 pixels per side.");
            }
            Release(textureView_);
            textureView_ = loadedTexture;
            texturePath_ = path;
            error_.clear();
            return true;
        }

        void Render(dx11::XMVECTOR position, float halfSize, const dx11::XMMATRIX& view,
                    const dx11::XMMATRIX& projection, dx11::ID3D11ShaderResourceView* sceneDepth,
                    int width, int height)
        {
            if (!vertexBuffer_ || !textureView_ || !pixelShader_ || width <= 0 || height <= 0 ||
                !std::isfinite(halfSize) || halfSize <= 0)
            {
                return;
            }
            if (!EnsureDepth(width, height))
            {
                return;
            }
            const dx11::XMMATRIX viewProjection = dx11::XMMatrixMultiply(view, projection);
            dx11::XMFLOAT4 clipCorners[8];
            for (int i = 0; i < 8; ++i)
            {
                const dx11::XMVECTOR offset = dx11::XMVectorSet((i & 1) ? halfSize : -halfSize,
                                                                (i & 2) ? halfSize : -halfSize,
                                                                (i & 4) ? halfSize : -halfSize, 0);
                const dx11::XMVECTOR world =
                    dx11::XMVectorSetW(dx11::XMVectorAdd(position, offset), 1);
                // Retain clip-space W for perspective-correct texture interpolation.
                dx11::XMStoreFloat4(&clipCorners[i],
                                    dx11::XMVector4Transform(world, viewProjection));
            }
            const int faceCorners[6][4] = {{0, 1, 2, 3}, {5, 4, 7, 6}, {4, 0, 6, 2},
                                           {1, 5, 3, 7}, {2, 3, 6, 7}, {4, 5, 0, 1}};
            const float faceShades[6] = {1.0f, 0.76f, 0.84f, 0.68f, 1.0f, 0.58f};
            const int triangleCorners[6] = {0, 1, 2, 2, 1, 3};
            const dx11::XMFLOAT2 textureCoordinates[4] = {{0, 1}, {1, 1}, {0, 0}, {1, 0}};
            Vertex vertices[36];
            for (int face = 0; face < 6; ++face)
            {
                for (int vertex = 0; vertex < 6; ++vertex)
                {
                    const int faceCorner = triangleCorners[vertex];
                    vertices[face * 6 + vertex] = {clipCorners[faceCorners[face][faceCorner]],
                                                   textureCoordinates[faceCorner],
                                                   faceShades[face]};
                }
            }
            dx11::D3D11_MAPPED_SUBRESOURCE mapped = {};
            if (FAILED(context_->Map(vertexBuffer_, 0, dx11::D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
            {
                return;
            }
            std::memcpy(mapped.pData, vertices, sizeof(vertices));
            context_->Unmap(vertexBuffer_, 0);
            const float frameConstants[4] = {static_cast<float>(width), static_cast<float>(height),
                                             sceneDepth ? 1.0f : 0.0f, 0.0f};
            if (FAILED(
                    context_->Map(frameConstants_, 0, dx11::D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
            {
                return;
            }
            std::memcpy(mapped.pData, frameConstants, sizeof(frameConstants));
            context_->Unmap(frameConstants_, 0);

            PipelineStateGuard restore(context_);
            dx11::ID3D11RenderTargetView* targets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
            dx11::ID3D11DepthStencilView* previousDepth = nullptr;
            context_->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, targets,
                                         &previousDepth);
            // Use a private depth buffer for the cube faces; sample scene depth in the pixel
            // shader.
            context_->OMSetRenderTargets(1, targets, depthView_);
            context_->ClearDepthStencilView(depthView_, dx11::D3D11_CLEAR_DEPTH, 1, 0);
            const dx11::D3D11_VIEWPORT viewport = {
                0, 0, static_cast<float>(width), static_cast<float>(height), 0, 1};
            context_->RSSetViewports(1, &viewport);
            context_->RSSetState(rasterizer_);
            context_->OMSetDepthStencilState(depthState_, 0);
            const float blendFactor[4] = {};
            context_->OMSetBlendState(blend_, blendFactor, 0xffffffffu);
            context_->IASetInputLayout(inputLayout_);
            context_->IASetPrimitiveTopology(dx11::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            const UINT stride = sizeof(Vertex);
            const UINT offset = 0;
            context_->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);
            context_->VSSetShader(vertexShader_, nullptr, 0);
            context_->PSSetShader(pixelShader_, nullptr, 0);
            context_->GSSetShader(nullptr, nullptr, 0);
            context_->HSSetShader(nullptr, nullptr, 0);
            context_->DSSetShader(nullptr, nullptr, 0);
            context_->PSSetConstantBuffers(0, 1, &frameConstants_);
            dx11::ID3D11ShaderResourceView* textures[2] = {textureView_, sceneDepth};
            context_->PSSetShaderResources(0, 2, textures);
            context_->PSSetSamplers(0, 1, &sampler_);
            context_->Draw(36, 0);
            dx11::ID3D11ShaderResourceView* emptyTextures[2] = {};
            context_->PSSetShaderResources(0, 2, emptyTextures);
            context_->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, targets,
                                         previousDepth);
            for (auto& target : targets)
            {
                Release(target);
            }
            Release(previousDepth);
        }

        void Shutdown()
        {
            Release(vertexShader_);
            Release(pixelShader_);
            Release(inputLayout_);
            Release(vertexBuffer_);
            Release(frameConstants_);
            Release(textureView_);
            Release(rasterizer_);
            Release(depthState_);
            Release(depthView_);
            Release(sampler_);
            Release(blend_);
            device_ = nullptr;
            context_ = nullptr;
            depthWidth_ = 0;
            depthHeight_ = 0;
            texturePath_.clear();
        }

        const std::wstring& TexturePath() const { return texturePath_; }
        const std::wstring& LastError() const { return error_; }

    private:
        struct Vertex
        {
            dx11::XMFLOAT4 position;
            dx11::XMFLOAT2 uv;
            float shade;
        };
        template <class T> static void Release(T*& resource)
        {
            if (resource)
            {
                resource->Release();
                resource = nullptr;
            }
        }
        bool Fail(const wchar_t* message)
        {
            error_ = message;
            return false;
        }

        bool CreateNpcTexture(const unsigned char* pixels, UINT width, UINT height,
                           dx11::ID3D11ShaderResourceView** view)
        {
            dx11::D3D11_TEXTURE2D_DESC desc = {};
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = dx11::D3D11_USAGE_IMMUTABLE;
            desc.BindFlags = dx11::D3D11_BIND_SHADER_RESOURCE;
            dx11::D3D11_SUBRESOURCE_DATA data = {pixels, width * 4, 0};
            dx11::ID3D11Texture2D* image = nullptr;
            HRESULT result = device_->CreateTexture2D(&desc, &data, &image);
            if (SUCCEEDED(result))
            {
                result = device_->CreateShaderResourceView(image, nullptr, view);
            }
            Release(image);
            return SUCCEEDED(result);
        }

        bool EnsureDepth(int width, int height)
        {
            if (depthView_ && width == depthWidth_ && height == depthHeight_)
            {
                return true;
            }
            dx11::D3D11_TEXTURE2D_DESC desc = {};
            desc.Width = static_cast<UINT>(width);
            desc.Height = static_cast<UINT>(height);
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            desc.SampleDesc.Count = 1;
            desc.Usage = dx11::D3D11_USAGE_DEFAULT;
            desc.BindFlags = dx11::D3D11_BIND_DEPTH_STENCIL;
            dx11::ID3D11Texture2D* image = nullptr;
            dx11::ID3D11DepthStencilView* depth = nullptr;
            HRESULT result = device_->CreateTexture2D(&desc, nullptr, &image);
            if (SUCCEEDED(result))
            {
                result = device_->CreateDepthStencilView(image, nullptr, &depth);
            }
            Release(image);
            if (FAILED(result))
            {
                Release(depth);
                return false;
            }
            Release(depthView_);
            depthView_ = depth;
            depthWidth_ = width;
            depthHeight_ = height;
            return true;
        }

        // Borrowed from the application; all resources below are owned by this renderer.
        dx11::ID3D11Device* device_ = nullptr;
        dx11::ID3D11DeviceContext* context_ = nullptr;
        dx11::ID3D11VertexShader* vertexShader_ = nullptr;
        dx11::ID3D11PixelShader* pixelShader_ = nullptr;
        dx11::ID3D11InputLayout* inputLayout_ = nullptr;
        dx11::ID3D11Buffer* vertexBuffer_ = nullptr;
        dx11::ID3D11Buffer* frameConstants_ = nullptr;
        dx11::ID3D11ShaderResourceView* textureView_ = nullptr;
        dx11::ID3D11RasterizerState* rasterizer_ = nullptr;
        dx11::ID3D11DepthStencilState* depthState_ = nullptr;
        dx11::ID3D11DepthStencilView* depthView_ = nullptr;
        dx11::ID3D11SamplerState* sampler_ = nullptr;
        dx11::ID3D11BlendState* blend_ = nullptr;
        int depthWidth_ = 0;
        int depthHeight_ = 0;
        std::wstring texturePath_;
        std::wstring error_;
    };

} // namespace GameUI
