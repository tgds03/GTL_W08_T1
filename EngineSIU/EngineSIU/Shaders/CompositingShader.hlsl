
Texture2D SceneTexture : register(t100);
Texture2D PP_PostProcessTexture : register(t101);
Texture2D EditorTexture : register(t102);
Texture2D DebugTexture : register(t104);

SamplerState CompositingSampler : register(s0);

cbuffer ViewMode : register(b0)
{
    uint ViewMode; 
    float3 Padding;
}

cbuffer CameraEffectConstants : register(b1)
{
    float2 LetterboxScale;
    float2 LetterboxOffset;
    float4 FadeColor;
    float FadeAmount; // 페이드 값 (0.0 ~ 1.0)
    float3 Padding2;
}

struct PS_Input
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD;
};

PS_Input mainVS(uint VertexID : SV_VertexID)
{
    PS_Input Output;

    float2 QuadPositions[6] = {
        float2(-1,  1),  // Top Left
        float2(1,  1),  // Top Right
        float2(-1, -1),  // Bottom Left
        float2(1,  1),  // Top Right
        float2(1, -1),  // Bottom Right
        float2(-1, -1)   // Bottom Left
    };

    float2 UVs[6] = {
        float2(0, 0), float2(1, 0), float2(0, 1),
        float2(1, 0), float2(1, 1), float2(0, 1)
    };

    Output.Position = float4(QuadPositions[VertexID], 0, 1);
    Output.UV = UVs[VertexID];

    return Output;
}

float4 mainPS(PS_Input Input) : SV_TARGET
{
    float4 Scene = SceneTexture.Sample(CompositingSampler, Input.UV);
    float4 PostProcess = PP_PostProcessTexture.Sample(CompositingSampler, Input.UV);
    float4 Editor = EditorTexture.Sample(CompositingSampler, Input.UV);
    float4 Debug = DebugTexture.Sample(CompositingSampler, Input.UV);

    float4 FinalColor = float4(0, 0, 0, 0);
    if (ViewMode == 7)
    {
        FinalColor = lerp(Scene, Debug, 0.5);
        FinalColor = lerp(FinalColor, Editor, Editor.a);
    }
    else
    {
        FinalColor = lerp(Scene, PostProcess, PostProcess.a);
        FinalColor = lerp(FinalColor, Editor, Editor.a);
    }
    
     // 레터박스 적용

    // 샘플링할 UV 계산 (C++에서 받은 Scale/Offset 사용)
    // 만약 Scale=1, Offset=0 이면 sampleUV는 Input.UV와 같아짐
    float2 sampleUV = (Input.UV - LetterboxOffset) / LetterboxScale;

    // 마스크 계산 (sampleUV가 유효 범위 [0, 1] 내에 있는지)
    // 만약 Scale=1, Offset=0 이면 sampleUV = Input.UV 이므로 마스크는 항상 1.0이 됨
    float maskX = step(0.0, sampleUV.x) * step(sampleUV.x, 1.0);
    float maskY = step(0.0, sampleUV.y) * step(sampleUV.y, 1.0);
    float insideMask = maskX * maskY; // 내부면 1.0, 외부면 0.0

    // 레터박스 바 색상 정의
    float4 letterboxBarColor = float4(0.0f, 0.0f, 0.0f, 1.0f); // 검은색

    // insideMask를 사용하여 레터박스 외부 영역은 Bar 색상으로, 내부는 CompositedColor로 설정
    // 만약 Scale=1, Offset=0 이면 insideMask=1 이므로 항상 CompositedColor가 됨
    float4 LetterboxedColor = lerp(letterboxBarColor, FinalColor, insideMask);


    // --- 4. 페이드 적용 ---
    // FadeAmount: 0.0 = FadeColor, 1.0 = LetterboxedColor
    FinalColor = lerp(FadeColor, LetterboxedColor, FadeAmount);


    return FinalColor;
}
