#if !defined(WIN32_DX11_H)
#define WIN32_DX11_H

#include <d3d11.h>
#include <d3dcompiler.h>
// #include <dxgi1_2.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

struct Win32DX11
{
    ID3D11Device*            device;
    ID3D11DeviceContext*     device_context;
    IDXGISwapChain*          swap_chain;
    ID3D11RenderTargetView*  frame_buffer_view;
    ID3D11RasterizerState*   rasterizer_state;
    ID3D11DepthStencilState* depth_stencil_state;
    ID3D11DepthStencilView*  depth_buffer_view;
    ID3D11BlendState*        blend_state;
#if GJ_DEBUG
    ID3D11Debug*             debug_context;
#endif
};

global_variable Win32DX11 g_win32_dx11;

internal void
win32_init_directx11_render_views(ID3D11Device* device, IDXGISwapChain* swap_chain,
                                  ID3D11RenderTargetView** frame_buffer_view,
                                  ID3D11DepthStencilView** depth_buffer_view)
{
    ID3D11Texture2D* frame_buffer;
    HRESULT hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&frame_buffer);
    gj_Assert(SUCCEEDED(hr));

    hr = device->CreateRenderTargetView(frame_buffer, 0, frame_buffer_view);
    gj_Assert(SUCCEEDED(hr));

    D3D11_TEXTURE2D_DESC depth_buffer_desc;
    frame_buffer->GetDesc(&depth_buffer_desc);
    depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depth_buffer;
    hr = device->CreateTexture2D(&depth_buffer_desc, NULL, &depth_buffer);
    gj_Assert(SUCCEEDED(hr));

    hr = device->CreateDepthStencilView(depth_buffer, NULL, depth_buffer_view);
    gj_Assert(SUCCEEDED(hr));
        
    frame_buffer->Release();
    depth_buffer->Release();
}

internal void
win32_init_directx11(HWND window, b32 disable_alt_enter_fullscreen = true)
{
    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    D3D_FEATURE_LEVEL actual_level;
    
    u32 device_flags = 0;
#if GJ_DEBUG
    device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    {
        HRESULT hr = D3D11CreateDevice(
            NULL,                        // Specify nullptr to use the default adapter.
            D3D_DRIVER_TYPE_HARDWARE,    // Create a device using the hardware graphics driver.
            0,                           // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
            device_flags,                // Set flags.
            levels,                      // List of feature levels this app can support.
            gj_ArrayCount(levels),       // Size of the list above.
            D3D11_SDK_VERSION,           // Always set this to D3D11_SDK_VERSION for Windows Store apps.
            &g_win32_dx11.device,        // Returns the Direct3D device created.
            &actual_level,               // Returns feature level of device created.
            &g_win32_dx11.device_context // Returns the device immediate context.
        );
        gj_Assert(SUCCEEDED(hr));
    }

#if GJ_DEBUG
    {
        HRESULT hr = g_win32_dx11.device->QueryInterface(__uuidof(ID3D11Debug), (void**)&g_win32_dx11.debug_context);
        gj_Assert(SUCCEEDED(hr));
        ID3D11InfoQueue* debug_queue;
        hr = g_win32_dx11.debug_context->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&debug_queue);
        gj_Assert(SUCCEEDED(hr));
        debug_queue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
        debug_queue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR,      true);
        // debug_queue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING,    true);
        debug_queue->Release();
        // debug_context->Release();
    }
#endif

    // IDXGISwapChain
    {
        DXGI_SWAP_CHAIN_DESC desc;
        ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
        desc.Windowed = TRUE; // Sets the initial state of full-screen mode.
        desc.BufferCount = 2;
        desc.BufferDesc.Width  = 0;
        desc.BufferDesc.Height = 0;
        desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SampleDesc.Count = 1;      //multisampling setting
        desc.SampleDesc.Quality = 0;    //vendor-specific flag
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.OutputWindow = window;

        IDXGIFactory* factory;
        {
            IDXGIDevice* dxgi_device;
            HRESULT hr = g_win32_dx11.device->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxgi_device);
            gj_Assert(SUCCEEDED(hr));
    
            IDXGIAdapter* adapter;

            hr = dxgi_device->GetAdapter(&adapter);
            gj_Assert(SUCCEEDED(hr));
            dxgi_device->Release();
            
            adapter->GetParent(IID_PPV_ARGS(&factory));
            adapter->Release();
        }

        HRESULT hr = factory->CreateSwapChain(
            g_win32_dx11.device,
            &desc,
            &g_win32_dx11.swap_chain
        );
        factory->Release();
        gj_Assert(SUCCEEDED(hr));
    }
    
    // TODO: https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/work-with-dxgi
    //       https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/understand-the-directx-11-2-graphics-pipeline

    // ID3D11RenderTargetView
    // ID3D11DepthStencilView
    win32_init_directx11_render_views(g_win32_dx11.device, g_win32_dx11.swap_chain,
                                      &g_win32_dx11.frame_buffer_view, &g_win32_dx11.depth_buffer_view);
    
    // ID3D11RasterizerState
    {
        D3D11_RASTERIZER_DESC rasterizer_desc = {};
        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.CullMode = D3D11_CULL_BACK;
        rasterizer_desc.FrontCounterClockwise = false;
        HRESULT hr = g_win32_dx11.device->CreateRasterizerState(&rasterizer_desc, &g_win32_dx11.rasterizer_state);
        gj_Assert(SUCCEEDED(hr));
    }

    // ID3D11DepthStencilState
    {
        D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
        depth_stencil_desc.DepthEnable = true;
        depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        HRESULT hr = g_win32_dx11.device->CreateDepthStencilState(&depth_stencil_desc, &g_win32_dx11.depth_stencil_state);
        gj_Assert(SUCCEEDED(hr));
    }

#if 0
    // ID3D11BlendState
    {
        D3D11_RENDER_TARGET_BLEND_DESC render_target_blend_desc;
        render_target_blend_desc.BlendEnable = true;
        render_target_blend_desc.SrcBlend    = D3D11_BLEND_SRC_COLOR;
        render_target_blend_desc.DestBlend   = D3D11_BLEND_BLEND_FACTOR;
        render_target_blend_desc.BlendOp     = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
        render_target_blend_desc.DestBlendAlpha = D3D11_BLEND_ONE;
        render_target_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
  
        D3D11_BLEND_DESC blend_desc;
        blend_desc.AlphaToCoverageEnable  = false;
        blend_desc.IndependentBlendEnable = false;
        blend_desc.RenderTarget[0] = render_target_blend_desc;

        g_win32_dx11.device->CreateBlendState(&blend_desc, &g_win32_dx11.blend_state);
    }
#endif
    
    // Disable Alt+Enter fullscreen toggle, if using win32_toggle_fullscreen(window) for example
    if (disable_alt_enter_fullscreen)
    {
        IDXGIDevice* dxgi_device;
        HRESULT hr = g_win32_dx11.device->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxgi_device);
        gj_Assert(SUCCEEDED(hr));

        IDXGIAdapter* dxgi_adapter;
        hr = dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void **)&dxgi_adapter);
        gj_Assert(SUCCEEDED(hr));
        
        IDXGIFactory* dxgi_factory;
        hr = dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void **)&dxgi_factory);
        gj_Assert(SUCCEEDED(hr));
        
        hr = dxgi_factory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER);
        gj_Assert(SUCCEEDED(hr));
    }
}

#endif
