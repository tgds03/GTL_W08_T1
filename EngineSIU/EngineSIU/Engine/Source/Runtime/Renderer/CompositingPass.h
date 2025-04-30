
#pragma once
#include <memory>

#include "IRenderPass.h"
#include "D3D11RHI/DXDShaderManager.h"
#include "Math/Color.h"

struct FCameraEffectConstants
{
    FVector2D LetterboxScale;
    FVector2D LetterboxOffset;
    FLinearColor FadeColor;
    float FadeAmount;
    FVector Padding;
};

class FCompositingPass : public IRenderPass
{
public:
    FCompositingPass();
    virtual ~FCompositingPass();
    
    virtual void Initialize(FDXDBufferManager* InBufferManager, FGraphicsDevice* InGraphics, FDXDShaderManager* InShaderManage) override;
    
    virtual void PrepareRenderArr() override;

    virtual void Render(const std::shared_ptr<FEditorViewportClient>& Viewport) override;

    virtual void ClearRenderArr() override;
    
private:
    FDXDBufferManager* BufferManager;
    FGraphicsDevice* Graphics;
    FDXDShaderManager* ShaderManager;

    ID3D11SamplerState* Sampler;

    ID3D11Buffer* ViewModeBuffer;

    // TODO: Manager로 빼야함
    float FadeAmountTest;
    float FadeDelta = 0.01f; // Fade 속도
};
