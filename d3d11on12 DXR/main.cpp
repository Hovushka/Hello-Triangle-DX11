#include <windows.h>  
#include <d3d11.h>

#include <iostream>
#include <array>

#include "DXHelpers.h"
#include "ReadData.h"

const int WIDTH = 800, HEIGHT = 600;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY || message == WM_CLOSE || wParam == VK_ESCAPE) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.lpszClassName = L"DXRon11Class";

    RegisterClass(&wc);

    HWND window = CreateWindow(wc.lpszClassName, L"DXR on 11", WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT,
        0, 0, 0, 0);

    D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;

    std::array driverTypes = {
        D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE
    };

    std::array featureLevels = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

    DXGI_MODE_DESC displayDesc = {
        .Width = static_cast<unsigned int>(WIDTH),
        .Height = static_cast<unsigned int>(HEIGHT),
        .RefreshRate = {.Numerator = 60, .Denominator = 1, },
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
    };

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {
        .BufferDesc = displayDesc,
        .SampleDesc = {.Count = 1, .Quality = 0, },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 1,
        .OutputWindow = window,
        .Windowed = true,
    };

    unsigned int creationFlags = 0;
#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT res = E_FAIL;
    for (auto& driver : driverTypes) {
        res = D3D11CreateDeviceAndSwapChain(
            nullptr,
            driver,
            nullptr,
            creationFlags,
            featureLevels.data(), featureLevels.size(),
            D3D11_SDK_VERSION,
            &swapChainDesc, &swapChain,
            &device,
            &featureLevel,
            &deviceContext
        );

        if (SUCCEEDED(res)) {
            driverType = driver;
            break;
        }
    }
    HR(res);

    ID3D11Texture2D* backBuffer;
    HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer));
    HR(device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView));

    float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    auto vertexShaderBytecode = DX::ReadData(L"vertex_shader.cso");
    ID3D11VertexShader* vertexShader;
    HR(device->CreateVertexShader(vertexShaderBytecode.data(), vertexShaderBytecode.size(), nullptr, &vertexShader));

    auto pixelShaderBytecode = DX::ReadData(L"pixel_shader.cso");
    ID3D11PixelShader* pixelShader;
    HR(device->CreatePixelShader(pixelShaderBytecode.data(), pixelShaderBytecode.size(), nullptr, &pixelShader));

    D3D11_VIEWPORT viewport = {
        .TopLeftX = 0.0f,
        .TopLeftY = 0.0f,
        .Width = WIDTH,
        .Height = HEIGHT,
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f,
    };

    MSG msg = {};
    while (1) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                break;
            }
        }

        deviceContext->ClearRenderTargetView(renderTargetView, clearColor);

        deviceContext->IASetInputLayout(nullptr);
        deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        deviceContext->VSSetShader(vertexShader, 0, 0);

        deviceContext->RSSetViewports(1, &viewport);

        deviceContext->PSSetShader(pixelShader, 0, 0);

        deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);

        deviceContext->Draw(3, 0);

        swapChain->Present(1, 0);
    }

    return 0;
}