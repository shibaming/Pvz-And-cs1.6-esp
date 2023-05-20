
#include "memory.h"
#include"imgui/imgui.h"
#include <d3d11.h>
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "dwmapi.h"
#include "class.h"
#include <string>
#include "util.h"
#include "esp.h"

Memory mem;
std::vector<Zombie> v(12);
std::vector<Person> preson(20);
uintptr_t loat = 0;
bool bl = false;
Esp esp;
void updata()
{

    while (true)
    {
        if (bl)
        {
            for (size_t i = 0; i < v.size(); i++)
            {
                v[i].pos = mem.read<ImVec2>((loat + 0x2c) + 0x15c * i);
                v[i].hp = mem.read<int>((loat + 0xc8) + 0x15c * i);
            }
        }
        else
        {
            for (size_t i = 0; i < preson.size(); i++)
            {
                preson[i].pos = mem.read<Vec3>((loat + 0x88) + 0x324 * i);
                preson[i].Headupangle = mem.read<float>((loat + 0x88 + 0x6C) + 0x324 * i);
                preson[i].Rotationangle = mem.read<float>((loat + 0x88 + 0x70) + 0x324 * i);
                preson[i].isalive = (bool)mem.read<float>((loat + 0x88 + 0x130) + 0x324 * i);
                preson[i].hp = (int)mem.read<float>((loat + 0x88 + 0x158) + 0x324 * i);
                preson[i].killcount = (int)mem.read<float>((loat + 0x88 + 0x15C) + 0x324 * i);
                preson[i].flag = mem.read<short>(0x1A256AA + 0x68 * i);//匪是1警察是2
                esp.GetMatrix(mem.m_handle);
            }
        }
        
    }
}

static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
    HWND hd;
    if (FindWindowA(NULL, "植物大战僵尸中文版"))
    {
        hd = FindWindowA(NULL, "植物大战僵尸中文版");
        bl = true;
    }
    else
    {
        hd = FindWindowA(NULL, "Counter-Strike");
    }
	DWORD pid;
	GetWindowThreadProcessId(hd, &pid);
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    DWORD modBaseAddr;  
    mem = handle;
    if (bl)
    {
        modBaseAddr = GetModuleBaseAddress(pid, L"PlantsVsZombies.exe");
        std::vector<uintptr_t> ptr = { 0x2A9EC0 ,0x768,0x90 };
        mem.read_chain(modBaseAddr, ptr, loat);
    }
    else
    {
        modBaseAddr = GetModuleBaseAddress(pid, L"cstrike.exe");
        loat = mem.read<uintptr_t>(modBaseAddr + 0x11069BC);
    }
    std::cout << modBaseAddr << std::endl;
    
    
    
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)updata, NULL, 0, NULL);
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    Rect rect;
    GetWindowRect(hd, &rect);
    esp.m_screenSize = rect;
    HWND hwnd = ::CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_VISIBLE | WS_POPUP, rect.left + 2, rect.top + 26,rect.width(), rect.height() - 26, nullptr, nullptr, wc.hInstance, nullptr);
    MARGINS margins;
    margins.cxLeftWidth = rect.left;
    margins.cxRightWidth = rect.top;
    margins.cyTopHeight = rect.width();
    margins.cyBottomHeight = rect.height();
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();        
        //ImGui::ShowDemoWindow();
        Rect rect;
        if (bl)
        {
            for (auto &i :v)
            {
            
                GetWindowRect(hd, &rect);
                float x = rect.x() + rect.width() / 2;
                if (i.hp == 0 || i.pos.x > 800.f || i.pos.x == 0.f)
                {
                    continue;
                }
                ImVec2 v2 = { rect.x() + i.pos.x + 15 ,rect.y() + i.pos.y + 65 };
                esp.DrawLine({ x,rect.y() + 80 }, v2);
                esp.DrawText({ rect.x() + i.pos.x + 45 ,rect.y() + i.pos.y + 45 }, std::to_string(i.hp).c_str());
                esp.DrawRect(v2, { v2.x + 90,v2.y + 130 });
            }//90*130
        } 
        else
        {
            GetWindowRect(hd, &rect);
            float x = rect.x() + rect.width() / 2;
            for (int i = 1; i < preson.size(); i++)
            {
                if (preson[i].pos.x == 0.f)
                {
                    continue;                  
                }
                if (!preson[i].isalive)
                {
                    continue;
                }
                if (preson[i].flag != preson[0].flag)
                {
                    ImVec2 pos = esp.WorldToScreen(preson[i].pos);

                   esp.DrawLine({ x,rect.y() },pos );
                   esp.DrawBox(preson[i].pos);
                   esp.DrawText({ pos.x,pos.y - 36 }, std::to_string(preson[i].hp).c_str());
                }
            }
        }
       
        

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
	system("pause");
	return 0;

}

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            //const int dpi = HIWORD(wParam);
            //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}