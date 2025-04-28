#include "EngineLoop.h"
#include "ImGuiManager.h"
#include "UnrealClient.h"
#include "WindowsPlatformTime.h"
#include "D3D11RHI/GraphicDevice.h"
#include "Engine/EditorEngine.h"
#include "LevelEditor/SLevelEditor.h"
#include "PropertyEditor/ViewportTypePanel.h"
#include "Slate/Widgets/Layout/SSplitter.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UnrealEd/UnrealEd.h"
#include "World/World.h"

#include "Engine/EditorEngine.h"
#include "Renderer/DepthPrePass.h"
#include "Renderer/TileLightCullingPass.h"
#include "InputCore/InputSystem.h"


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

FGraphicsDevice FEngineLoop::GraphicDevice;
FRenderer FEngineLoop::Renderer;
UPrimitiveDrawBatch FEngineLoop::PrimitiveDrawBatch;
FResourceMgr FEngineLoop::ResourceManager;
ScriptSystem FEngineLoop::ScriptSys;
FInputSystem FEngineLoop::InputSystem;
uint32 FEngineLoop::TotalAllocationBytes = 0;
uint32 FEngineLoop::TotalAllocationCount = 0;

FEngineLoop::FEngineLoop()
    : AppWnd(nullptr)
    , UIMgr(nullptr)
    , LevelEditor(nullptr)
    , UnrealEditor(nullptr)
    , BufferManager(nullptr)
{
}

int32 FEngineLoop::PreInit()
{
    return 0;
}

int32 FEngineLoop::Init(HINSTANCE hInstance)
{
    FPlatformTime::InitTiming();

    /* must be initialized before window. */
    WindowInit(hInstance);

    UnrealEditor = new UnrealEd();
    BufferManager = new FDXDBufferManager();
    UIMgr = new UImGuiManager;
    AppMessageHandler = std::make_unique<FSlateAppMessageHandler>();
    LevelEditor = new SLevelEditor();

    UnrealEditor->Initialize();
    GraphicDevice.Initialize(AppWnd);
    
    if (!GPUTimingManager.Initialize(GraphicDevice.Device, GraphicDevice.DeviceContext))
    {
        UE_LOG(LogLevel::Error, TEXT("Failed to initialize GPU Timing Manager!"));
    }
    EngineProfiler.SetGPUTimingManager(&GPUTimingManager);

    // @todo Table에 Tree 구조로 넣을 수 있도록 수정
    EngineProfiler.RegisterStatScope(TEXT("Renderer_Render"), FName(TEXT("Renderer_Render_CPU")), FName(TEXT("Renderer_Render_GPU")));
    EngineProfiler.RegisterStatScope(TEXT("|- DepthPrePass"), FName(TEXT("DepthPrePass_CPU")), FName(TEXT("DepthPrePass_GPU")));
    EngineProfiler.RegisterStatScope(TEXT("|- TileLightCulling"), FName(TEXT("TileLightCulling_CPU")), FName(TEXT("TileLightCulling_GPU")));
    EngineProfiler.RegisterStatScope(TEXT("|- ShadowPass"), FName(TEXT("ShadowPass_CPU")), FName(TEXT("ShadowPass_GPU")));
    EngineProfiler.RegisterStatScope(TEXT("|- StaticMeshPass"), FName(TEXT("StaticMeshPass_CPU")), FName(TEXT("StaticMeshPass_GPU")));
    EngineProfiler.RegisterStatScope(TEXT("|- WorldBillboardPass"), FName(TEXT("WorldBillboardPass_CPU")), FName(TEXT("WorldBillboardPass_GPU")));
    EngineProfiler.RegisterStatScope(TEXT("|- UpdateLightBufferPass"), FName(TEXT("UpdateLightBufferPass_CPU")), FName(TEXT("UpdateLightBufferPass_GPU")));
    EngineProfiler.RegisterStatScope(TEXT("|- FogPass"), FName(TEXT("FogPass_CPU")), FName(TEXT("FogPass_GPU")));
    EngineProfiler.RegisterStatScope(TEXT("|- PostProcessCompositing"), FName(TEXT("PostProcessCompositing_CPU")), FName(TEXT("PostProcessCompositing_GPU")));
    EngineProfiler.RegisterStatScope(TEXT("|- EditorBillboardPass"), FName(TEXT("EditorBillboardPass_CPU")), FName(TEXT("EditorBillboardPass_GPU")));
    EngineProfiler.RegisterStatScope(TEXT("|- EditorRenderPass"), FName(TEXT("EditorRenderPass_CPU")), FName(TEXT("EditorRenderPass_GPU")));
    EngineProfiler.RegisterStatScope(TEXT("|- LinePass"), FName(TEXT("LinePass_CPU")), FName(TEXT("LinePass_GPU")));
    EngineProfiler.RegisterStatScope(TEXT("|- GizmoPass"), FName(TEXT("GizmoPass_CPU")), FName(TEXT("GizmoPass_GPU")));
    EngineProfiler.RegisterStatScope(TEXT("|- CompositingPass"), FName(TEXT("CompositingPass_CPU")), FName(TEXT("CompositingPass_GPU")));
    EngineProfiler.RegisterStatScope(TEXT("SlatePass"), FName(TEXT("SlatePass_CPU")), FName(TEXT("SlatePass_GPU")));

    BufferManager->Initialize(GraphicDevice.Device, GraphicDevice.DeviceContext);
    Renderer.Initialize(&GraphicDevice, BufferManager, &GPUTimingManager);
    PrimitiveDrawBatch.Initialize(&GraphicDevice);
    UIMgr->Initialize(AppWnd, GraphicDevice.Device, GraphicDevice.DeviceContext);
    ResourceManager.Initialize(&Renderer, &GraphicDevice);
    ScriptSys.Initialize();
    
    uint32 ClientWidth = 0;
    uint32 ClientHeight = 0;
    GetClientSize(ClientWidth, ClientHeight);
    LevelEditor->Initialize(ClientWidth, ClientHeight);

    GEngine = FObjectFactory::ConstructObject<UEditorEngine>(nullptr);
    GEngine->Init();

    // ScriptSys.DoFile("main.lua");
    
    UpdateUI();

    ScriptSys.BindTypes();
    return 0;
}

void FEngineLoop::Render() const
{
    GraphicDevice.Prepare();
    
    if (LevelEditor->IsMultiViewport())
    {
        std::shared_ptr<FEditorViewportClient> ActiveViewportCache = GetLevelEditor()->GetActiveViewportClient();
        for (int i = 0; i < 4; ++i)
        {
            LevelEditor->SetActiveViewportClient(i);
            Renderer.Render(LevelEditor->GetActiveViewportClient());
        }
        
        for (int i = 0; i < 4; ++i)
        {
            LevelEditor->SetActiveViewportClient(i);
            Renderer.RenderViewport(LevelEditor->GetActiveViewportClient());
        }
        GetLevelEditor()->SetActiveViewportClient(ActiveViewportCache);
    }
    else
    {
        Renderer.Render(LevelEditor->GetActiveViewportClient());
        
        Renderer.RenderViewport(LevelEditor->GetActiveViewportClient());
    }
    
}

void FEngineLoop::Tick()
{
    LARGE_INTEGER Frequency;
    const double TargetFrameTime = 1000.0 / TargetFPS; // 한 프레임의 목표 시간 (밀리초 단위)

    QueryPerformanceFrequency(&Frequency);

    LARGE_INTEGER StartTime, EndTime;
    double ElapsedTime = 0.0;

    while (bIsExit == false)
    {
        FProfilerStatsManager::BeginFrame();    // Clear previous frame stats
        if (GPUTimingManager.IsInitialized())
        {
            GPUTimingManager.BeginFrame();      // Start GPU frame timing
        }

        QueryPerformanceCounter(&StartTime);

        MSG Msg;
        while (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Msg); // 키보드 입력 메시지를 문자메시지로 변경
            DispatchMessage(&Msg);  // 메시지를 WndProc에 전달

            if (Msg.message == WM_QUIT)
            {
                bIsExit = true;
                break;
            }
        }

        const float DeltaTime = static_cast<float>(ElapsedTime / 1000.f);
        ScriptSys.Reload();
        GEngine->Tick(DeltaTime);
        LevelEditor->Tick(DeltaTime);
        // FIXME : Update말고 Tick으로 일관되게 바꾸기
        FEngineLoop::InputSystem.Update();
        // ScriptSys.Tick(DeltaTime);
        Render();
        UIMgr->BeginFrame();
        if (GEngine->ActiveWorld->WorldType == EWorldType::Editor) {
            UnrealEditor->Render();
        }
        if (GEngine->ActiveWorld->WorldType == EWorldType::PIE) {
            if (GEngine->ActiveWorld->IsPlaying == false)
            {
                // 창 크기/위치 초기 설정 (한 번만)
                ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_Once);
                // ImGui IO 레퍼런스
                auto& io = ImGui::GetIO();
                // 화면 중앙 위치 계산
                ImVec2 center(
                    io.DisplaySize.x * 0.5f,
                    io.DisplaySize.y * 0.5f
                );
                // 창 정렬 값: 중앙(0.5,0.5) 기준으로 한 번만 설정
                ImGui::SetNextWindowPos(
                    center,
                    ImGuiCond_Once,
                    ImVec2(0.5f, 0.5f)
                );

                // 투명 배경 + 텍스트·버튼 색상 설정
                ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));           // 창 배경 투명
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); // 밝은 레드 텍스트
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.7f)); // 버튼 기본
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.8f)); // 버튼 호버
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 0.6f)); // 버튼 클릭

                if (ImGui::Begin(". . . ", nullptr,
                    ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize))
                {
                    // 중앙 정렬된 제목
                    const char* title = "Game Over";
                    float winW = ImGui::GetWindowWidth();
                    float txtW = ImGui::CalcTextSize(title).x;
                    ImGui::SetCursorPosX((winW - txtW) * 0.5f);
                    ImGui::Text("%s", title);

                    ImGui::Spacing();

                    // Restart 버튼 (가운데로 살짝 이동)
                    float btnW = 120;
                    ImGui::SetCursorPosX((winW - btnW * 2 - ImGui::GetStyle().ItemSpacing.x) * 0.5f);
                    if (ImGui::Button("Restart", ImVec2(btnW, 0)))
                    {
                        //GEngine->RestartPlayInEditor();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Quit to Editor", ImVec2(btnW, 0)))
                    {
                        //GEngine->RequestEndPlayMap();
                    }
                }
                ImGui::End();

                // 푸시한 스타일 해제
                ImGui::PopStyleColor(5);
            }
        }
        

        Console::GetInstance().Draw();
        EngineProfiler.Render(GraphicDevice.DeviceContext, GraphicDevice.ScreenWidth, GraphicDevice.ScreenHeight);

        UIMgr->EndFrame();

        // Pending 처리된 오브젝트 제거
        GUObjectArray.ProcessPendingDestroyObjects();

        if (GPUTimingManager.IsInitialized())
        {
            GPUTimingManager.EndFrame();        // End GPU frame timing
        }

        GraphicDevice.SwapBuffer();
        do
        {
            Sleep(0);
            QueryPerformanceCounter(&EndTime);
            ElapsedTime = (static_cast<double>(EndTime.QuadPart - StartTime.QuadPart) * 1000.f / static_cast<double>(Frequency.QuadPart));
        } while (ElapsedTime < TargetFrameTime);
    }
}

void FEngineLoop::GetClientSize(uint32& OutWidth, uint32& OutHeight) const
{
    RECT ClientRect = {};
    GetClientRect(AppWnd, &ClientRect);
            
    OutWidth = ClientRect.right - ClientRect.left;
    OutHeight = ClientRect.bottom - ClientRect.top;
}

void FEngineLoop::Exit()
{
    LevelEditor->Release();
    UIMgr->Shutdown();
    ResourceManager.Release(&Renderer);
    Renderer.Release();
    GraphicDevice.Release();
    
    GEngine->Release();

    delete UnrealEditor;
    delete BufferManager;
    delete UIMgr;
    delete LevelEditor;
}

void FEngineLoop::WindowInit(HINSTANCE hInstance)
{
    WCHAR WindowClass[] = L"JungleWindowClass";

    WCHAR Title[] = L"Game Tech Lab";

    WNDCLASSW wc{};
    wc.lpfnWndProc = AppWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WindowClass;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

    RegisterClassW(&wc);

    AppWnd = CreateWindowExW(
        0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1400, 1000,
        nullptr, nullptr, hInstance, nullptr
    );
}

LRESULT CALLBACK FEngineLoop::AppWndProc(HWND hWnd, uint32 Msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam))
    {
        return true;
    }

    switch (Msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            auto LevelEditor = GEngineLoop.GetLevelEditor();
            if (LevelEditor)
            {
                FEngineLoop::GraphicDevice.Resize(hWnd);
                // FEngineLoop::Renderer.DepthPrePass->ResizeDepthStencil();
                
                uint32 ClientWidth = 0;
                uint32 ClientHeight = 0;
                GEngineLoop.GetClientSize(ClientWidth, ClientHeight);
            
                LevelEditor->ResizeEditor(ClientWidth, ClientHeight);
                FEngineLoop::Renderer.TileLightCullingPass->ResizeViewBuffers(
                  static_cast<uint32>(LevelEditor->GetActiveViewportClient()->GetD3DViewport().Width),
                    static_cast<uint32>(LevelEditor->GetActiveViewportClient()->GetD3DViewport().Height)
                );
            }
        }
        GEngineLoop.UpdateUI();
        break;
    default:
        GEngineLoop.AppMessageHandler->ProcessMessage(hWnd, Msg, wParam, lParam);
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    return 0;
}

void FEngineLoop::UpdateUI()
{
    Console::GetInstance().OnResize(AppWnd);
    if (GEngineLoop.GetUnrealEditor())
    {
        GEngineLoop.GetUnrealEditor()->OnResize(AppWnd);
    }
    ViewportTypePanel::GetInstance().OnResize(AppWnd);
}
