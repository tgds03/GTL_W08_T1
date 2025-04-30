#include "CompositingPass.h"

#include <array>

#include "Define.h"
#include "RendererHelpers.h"
#include "UnrealClient.h"
#include "UnrealEd/EditorViewportClient.h"

FCompositingPass::FCompositingPass()
    : BufferManager(nullptr)
    , Graphics(nullptr)
    , ShaderManager(nullptr)
    , FadeAmountTest(1.f)
{
}

FCompositingPass::~FCompositingPass()
{
    
}

void FCompositingPass::Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage)
{
    BufferManager = InBufferManager;
    Graphics = InGraphics;
    ShaderManager = InShaderManage;

    ShaderManager->AddVertexShader(L"Compositing", L"Shaders/CompositingShader.hlsl", "mainVS");
    ShaderManager->AddPixelShader(L"Compositing", L"Shaders/CompositingShader.hlsl", "mainPS");

    D3D11_SAMPLER_DESC SamplerDesc = {};
    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SamplerDesc.MinLOD = 0;
    SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    
    Graphics->Device->CreateSamplerState(&SamplerDesc, &Sampler);

    ViewModeBuffer = BufferManager->GetConstantBuffer("FViewModeConstants");
}

void FCompositingPass::PrepareRenderArr()
{
}

void FCompositingPass::Render(const std::shared_ptr<FEditorViewportClient>& Viewport)
{
    // Setup
    FViewportResource* ViewportResource = Viewport->GetViewportResource();
    if (!ViewportResource)
    {
        return;
    }

    const EResourceType ResourceType = EResourceType::ERT_Compositing; 
    FRenderTargetRHI* RenderTargetRHI = Viewport->GetViewportResource()->GetRenderTarget(ResourceType);
    if (!RenderTargetRHI)
    {
        return;
    }

    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_Scene), 1, &ViewportResource->GetRenderTarget(EResourceType::ERT_Scene)->SRV);
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_PostProcess), 1, &ViewportResource->GetRenderTarget(EResourceType::ERT_PP_Fog)->SRV);
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_EditorOverlay), 1, &ViewportResource->GetRenderTarget(EResourceType::ERT_Editor)->SRV);

    Graphics->DeviceContext->OMSetRenderTargets(1, &RenderTargetRHI->RTV, nullptr);
    Graphics->DeviceContext->ClearRenderTargetView(RenderTargetRHI->RTV, ViewportResource->GetClearColor(ResourceType).data());

    Graphics->DeviceContext->RSSetState(Graphics->RasterizerSolidBack);
    Graphics->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Graphics->DeviceContext->PSSetSamplers(0, 1, &Sampler);

    // 버퍼 바인딩
    Graphics->DeviceContext->PSSetConstantBuffers(0, 1, &ViewModeBuffer);
    
    // Update Constant Buffer
    FViewModeConstants ViewModeConstantData = {};
    ViewModeConstantData.ViewMode = static_cast<uint32>(Viewport->GetViewMode());
    BufferManager->UpdateConstantBuffer<FViewModeConstants>("FViewModeConstants", ViewModeConstantData);

    // 카메라 효과 상수버퍼 업데이트
    FCameraEffectConstants CameraEffectData = {};
    
    // TODO : Manager로 조건/타이밍 옮겨야함 
    FadeAmountTest += FadeDelta;
    if (FadeAmountTest <= 0.0f || FadeAmountTest >= 1.0f)
    {
        FadeDelta *= -1.0f; // 방향 반전
        FadeAmountTest = std::clamp(FadeAmountTest, 0.0f, 1.0f); // 오버슈팅 보정
    }
    

    CameraEffectData.FadeAmount = FadeAmountTest;
    CameraEffectData.FadeColor = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // 레터박스 Scale, Offset 계산
    CameraEffectData.LetterboxScale = FVector2D(1.0f, 1.0f);
    CameraEffectData.LetterboxOffset = FVector2D(0.0f, 0.0f);

    // TODO : Manager가 나오면 TargetRatio를 받아서 계산
    float TargetAspectRatio = 16.0f / 9.0f;

    D3D11_TEXTURE2D_DESC WindowTextureDesc;
    RenderTargetRHI->Texture2D->GetDesc(&WindowTextureDesc);

    // TODO : CameraManager에서 조건을 받아서 활성화 
    if (true)
    {
        float WindowWidth = static_cast<float>(WindowTextureDesc.Width);
        float WindowHeight = static_cast<float>(WindowTextureDesc.Height);
        float WindowAspectRatio = WindowWidth / WindowHeight;

        if (WindowAspectRatio < TargetAspectRatio)
        {
            CameraEffectData.LetterboxScale.Y = WindowAspectRatio / TargetAspectRatio;
            CameraEffectData.LetterboxOffset.Y = (1.0f - CameraEffectData.LetterboxScale.Y) * 0.5f;
        }
        else
        {
            CameraEffectData.LetterboxScale.X = TargetAspectRatio / WindowAspectRatio;
            CameraEffectData.LetterboxOffset.X = (1.0f - CameraEffectData.LetterboxScale.X) * 0.5f;
        }
    }


    // Render
    ID3D11VertexShader* VertexShader = ShaderManager->GetVertexShaderByKey(L"Compositing");
    ID3D11PixelShader* PixelShader = ShaderManager->GetPixelShaderByKey(L"Compositing");
    Graphics->DeviceContext->VSSetShader(VertexShader, nullptr, 0);
    Graphics->DeviceContext->PSSetShader(PixelShader, nullptr, 0);
    Graphics->DeviceContext->IASetInputLayout(nullptr);

    BufferManager->BindConstantBuffer(TEXT("FCameraEffectConstants"), 1, EShaderStage::Pixel);
    BufferManager->UpdateConstantBuffer<FCameraEffectConstants>("FCameraEffectConstants", CameraEffectData);

    Graphics->DeviceContext->Draw(6, 0);

    // Finish
    Graphics->DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

    // Clear
    ID3D11ShaderResourceView* NullSRV[1] = { nullptr };
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_Scene), 1, NullSRV);
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_PostProcess), 1, NullSRV);
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_EditorOverlay), 1, NullSRV);
    Graphics->DeviceContext->PSSetShaderResources(static_cast<UINT>(EShaderSRVSlot::SRV_Debug), 1, NullSRV);

}

void FCompositingPass::ClearRenderArr()
{
}
