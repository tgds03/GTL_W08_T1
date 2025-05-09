#include "PropertyEditorPanel.h"

#include <algorithm>

#include "World/World.h"
#include "Actors/Player.h"
#include "Components/Light/LightComponent.h"
#include "Components/Light/PointLightComponent.h"
#include "Components/Light/SpotLightComponent.h"
#include "Components/Light/DirectionalLightComponent.h"
#include "Components/Light/AmbientLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextComponent.h"
#include "Components/ScriptableComponent.h" 
#include <filesystem>
#include <Windows.h>
#include <shellapi.h>

#include "ImGuiBezier.h"
#include "Actors/Camera.h"
#include "Engine/EditorEngine.h"
#include "Engine/FLoaderOBJ.h"
#include "UnrealEd/ImGuiWidget.h"
#include "UObject/Casts.h"
#include "UObject/ObjectFactory.h"
#include "Engine/Engine.h"
#include "Components/HeightFogComponent.h"
#include "Components/ProjectileMovementComponent.h"
#include "Components/ScriptableComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/AssetManager.h"
#include "LevelEditor/SLevelEditor.h"
#include "Math/JungleMath.h"
#include "Renderer/ShadowManager.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/UObjectIterator.h"

void PropertyEditorPanel::Render()
{
    /* Pre Setup */
    float PanelWidth = (Width) * 0.2f - 6.0f;
    float PanelHeight = (Height) * 0.65f;

    float PanelPosX = (Width) * 0.8f + 5.0f;
    float PanelPosY = (Height) * 0.3f + 15.0f;

    ImVec2 MinSize(140, 370);
    ImVec2 MaxSize(FLT_MAX, 900);

    /* Min, Max Size */
    ImGui::SetNextWindowSizeConstraints(MinSize, MaxSize);

    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    /* Panel Flags */
    ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    /* Render Start */
    ImGui::Begin("Detail", nullptr, PanelFlags);

    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
    if (!Engine)
    {
        return;
    }

    AEditorPlayer* Player = Engine->GetEditorPlayer();
    AActor* PickedActor = Engine->GetSelectedActor();


    if (PickedActor)
    {
        ImGui::SetItemDefaultFocus();
        // TreeNode 배경색을 변경 (기본 상태)
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        ImGui::Text("UUID: %d", PickedActor->GetUUID());
        if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
        {
            Location = PickedActor->GetActorLocation();
            Rotation = PickedActor->GetActorRotation();
            Scale = PickedActor->GetActorScale();

            FImGuiWidget::DrawVec3Control("Location", Location, 0, 85);
            ImGui::Spacing();

            FImGuiWidget::DrawRot3Control("Rotation", Rotation, 0, 85);
            ImGui::Spacing();

            FImGuiWidget::DrawVec3Control("Scale", Scale, 0, 85);
            ImGui::Spacing();

            PickedActor->SetActorLocation(Location);
            PickedActor->SetActorRotation(Rotation);
            PickedActor->SetActorScale(Scale);

            std::string coordiButtonLabel;
            if (Player->GetCoordMode() == ECoordMode::CDM_WORLD)
            {
                coordiButtonLabel = "World";
            }
            else if (Player->GetCoordMode() == ECoordMode::CDM_LOCAL)
            {
                coordiButtonLabel = "Local";
            }

            if (ImGui::Button(coordiButtonLabel.c_str(), ImVec2(ImGui::GetWindowContentRegionMax().x * 0.9f, 32)))
            {
                Player->AddCoordiMode();
            }

            ImGui::TreePop(); // 트리 닫기
        }

        ImGui::PopStyleColor();
    }

 

    if (PickedActor)
    {
        if (ImGui::Button("Duplicate"))
        {
            AActor* NewActor = Engine->ActiveWorld->DuplicateActor(Engine->GetSelectedActor());
            Engine->SelectActor(NewActor);
        }
    }

            

    if (PickedActor)
    {
        if (UScriptableComponent* scriptableComponent = Cast<UScriptableComponent>(PickedActor->GetComponentByClass<UScriptableComponent>()))
        {
            RenderLuaScriptEdit(scriptableComponent);

            if (ImGui::BeginCombo("Script", GetData(scriptableComponent->ScriptName)))
            {
                FEngineLoop::ScriptSys.Reload();
                for (const auto& pair: FEngineLoop::ScriptSys.LoadScripts)
                {
                    std::string filename = (pair.Key.Len() > 0) ? GetData(pair.Key) : "##";
                    if (ImGui::Selectable(filename.c_str()))
                    {
                        scriptableComponent->ScriptName = pair.Key;
                    }
                }
                ImGui::EndCombo();
            }
        } else
        {
            if (ImGui::Button("Add ScriptableComp"))
            {
                PickedActor->AddComponent(UScriptableComponent::StaticClass());
            }
        }
    }
    //// TODO: 추후에 RTTI를 이용해서 프로퍼티 출력하기
    //if (PickedActor)
    //    if (ULightComponent* lightObj = PickedActor->GetComponentByClass<ULightComponent>())
    //    {
    //        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    //        if (ImGui::TreeNodeEx("Light Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
    //        {
    //            /*  DrawColorProperty("Ambient Color",
    //                  [&]() { return lightObj->GetAmbientColor(); },
    //                  [&](FVector4 c) { lightObj->SetAmbientColor(c); });
    //              */
    //            DrawColorProperty("Base Color",
    //                [&]() { return lightObj->GetDiffuseColor(); },
    //                [&](FLinearColor c) { lightObj->SetDiffuseColor(c); });

    //            float Intensity = lightObj->GetIntensity();
    //            if (ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 100.0f, "%1.f"))
    //                lightObj->SetIntensity(Intensity);

    //             /*  
    //            float falloff = lightObj->GetFalloff();
    //            if (ImGui::SliderFloat("Falloff", &falloff, 0.1f, 10.0f, "%.2f")) {
    //                lightObj->SetFalloff(falloff);
    //            }

    //            TODO : For SpotLight
    //            */

    //            float attenuation = lightObj->GetAttenuation();
    //            if (ImGui::SliderFloat("Attenuation", &attenuation, 0.01f, 10000.f, "%.1f")) {
    //                lightObj->SetAttenuation(attenuation);
    //            }

    //            float AttenuationRadius = lightObj->GetAttenuationRadius();
    //            if (ImGui::SliderFloat("Attenuation Radius", &AttenuationRadius, 0.01f, 10000.f, "%.1f")) {
    //                lightObj->SetAttenuationRadius(AttenuationRadius);
    //            }

    //            ImGui::TreePop();
    //        }

    //        ImGui::PopStyleColor();
    //    }

    if (PickedActor)
    {
        if (UPointLightComponent* PointlightComponent = PickedActor->GetComponentByClass<UPointLightComponent>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("PointLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                DrawColorProperty("Light Color",
                    [&]() { return PointlightComponent->GetLightColor(); },
                    [&](FLinearColor c) { PointlightComponent->SetLightColor(c); });

                float Intensity = PointlightComponent->GetIntensity();
                if (ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 160.0f, "%.1f"))
                {
                    PointlightComponent->SetIntensity(Intensity);
                }

                float Radius = PointlightComponent->GetRadius();
                if (ImGui::SliderFloat("Radius", &Radius, 0.01f, 200.f, "%.1f"))
                {
                    PointlightComponent->SetRadius(Radius);
                }
                // --- Cast Shadows 체크박스 추가 ---
                bool bCastShadows = PointlightComponent->GetCastShadows(); // 현재 상태 가져오기
                if (ImGui::Checkbox("Cast Shadows", &bCastShadows)) // 체크박스 UI 생성 및 상호작용 처리
                {
                    PointlightComponent->SetCastShadows(bCastShadows); // 변경된 상태 설정
                    // 필요하다면, 상태 변경에 따른 즉각적인 렌더링 업데이트 요청 로직 추가
                    // 예: PointlightComponent->MarkRenderStateDirty();
                }

                ImGui::Text("ShadowMap");

                FShadowCubeMapArrayRHI* pointRHI = FEngineLoop::Renderer.ShadowManager->GetPointShadowCubeMapRHI();
                const char* faceNames[] = { "+X", "-X", "+Y", "-Y", "+Z", "-Z" };
                float imageSize = 128.0f;
                int index =  PointlightComponent->GetPointLightInfo().ShadowMapArrayIndex;
                // CubeMap이므로 6개의 ShadowMap을 그립니다.
                for (int i = 0; i < 6; ++i)
                {
                    ID3D11ShaderResourceView* faceSRV = pointRHI->ShadowFaceSRVs[index][i];
                    if (faceSRV)
                    {
                        ImGui::Image(reinterpret_cast<ImTextureID>(faceSRV), ImVec2(imageSize, imageSize));
                        ImGui::SameLine(); 
                        ImGui::Text("%s", faceNames[i]);
                    }
                }

                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }
    }

    if (PickedActor)
    {
        if (USpotLightComponent* SpotLightComponent = PickedActor->GetComponentByClass<USpotLightComponent>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("SpotLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                DrawColorProperty("Light Color",
                    [&]() { return SpotLightComponent->GetLightColor(); },
                    [&](FLinearColor c) { SpotLightComponent->SetLightColor(c); });

                float Intensity = SpotLightComponent->GetIntensity();
                if (ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 160.0f, "%.1f"))
                {
                    SpotLightComponent->SetIntensity(Intensity);
                }

                float Radius = SpotLightComponent->GetRadius();
                if (ImGui::SliderFloat("Radius", &Radius, 0.01f, 200.f, "%.1f"))
                {
                    SpotLightComponent->SetRadius(Radius);
                }

                LightDirection = SpotLightComponent->GetDirection();
                FImGuiWidget::DrawVec3Control("Direction", LightDirection, 0, 85);

                float InnerConeAngle = SpotLightComponent->GetInnerDegree();
                float OuterConeAngle = SpotLightComponent->GetOuterDegree();
                if (ImGui::DragFloat("InnerConeAngle", &InnerConeAngle, 0.5f, 0.0f, 80.0f))
                {
                    OuterConeAngle = std::max(InnerConeAngle, OuterConeAngle);

                    SpotLightComponent->SetInnerDegree(InnerConeAngle);
                    SpotLightComponent->SetOuterDegree(OuterConeAngle);
                }

                if (ImGui::DragFloat("OuterConeAngle", &OuterConeAngle, 0.5f, 0.0f, 80.0f))
                {
                    InnerConeAngle = std::min(OuterConeAngle, InnerConeAngle);

                    SpotLightComponent->SetOuterDegree(OuterConeAngle);
                    SpotLightComponent->SetInnerDegree(InnerConeAngle);
                }

                // --- Cast Shadows 체크박스 추가 ---
                bool bCastShadows = SpotLightComponent->GetCastShadows(); // 현재 상태 가져오기
                if (ImGui::Checkbox("Cast Shadows", &bCastShadows)) // 체크박스 UI 생성 및 상호작용 처리
                {
                    SpotLightComponent->SetCastShadows(bCastShadows); // 변경된 상태 설정
                    // 필요하다면, 상태 변경에 따른 즉각적인 렌더링 업데이트 요청 로직 추가
                    // 예: PointlightComponent->MarkRenderStateDirty();
                }

                ImGui::Text("ShadowMap");
                ImGui::Image(reinterpret_cast<ImTextureID>(FEngineLoop::Renderer.ShadowManager->GetSpotShadowDepthRHI()->ShadowSRVs[0]), ImVec2(200, 200));

                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }
    }

    if (PickedActor)
    {
        if (UDirectionalLightComponent* DirectionalLightComponent = PickedActor->GetComponentByClass<UDirectionalLightComponent>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("DirectionalLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                DrawColorProperty("Light Color",
                    [&]() { return DirectionalLightComponent->GetLightColor(); },
                    [&](FLinearColor c) { DirectionalLightComponent->SetLightColor(c); });

                float Intensity = DirectionalLightComponent->GetIntensity();
                if (ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 150.0f, "%.1f"))
                {
                    DirectionalLightComponent->SetIntensity(Intensity);
                }

                LightDirection = DirectionalLightComponent->GetDirection();
                FImGuiWidget::DrawVec3Control("Direction", LightDirection, 0, 85);


                // --- Cast Shadows 체크박스 추가 ---
                bool bCastShadows = DirectionalLightComponent->GetCastShadows(); // 현재 상태 가져오기
                if (ImGui::Checkbox("Cast Shadows", &bCastShadows)) // 체크박스 UI 생성 및 상호작용 처리
                {
                    DirectionalLightComponent->SetCastShadows(bCastShadows); // 변경된 상태 설정
                    // 필요하다면, 상태 변경에 따른 즉각적인 렌더링 업데이트 요청 로직 추가
                    // 예: PointlightComponent->MarkRenderStateDirty();
                }
                ImGui::Text("ShadowMap");

                // 분할된 개수만큼 CSM 해당 SRV 출력
                const uint32& NumCascades = FEngineLoop::Renderer.ShadowManager->GetNumCasCades();
                for (uint32 i = 0; i < NumCascades; ++i)
                {
                    ImGui::Image(reinterpret_cast<ImTextureID>(FEngineLoop::Renderer.ShadowManager->GetDirectionalShadowCascadeDepthRHI()->ShadowSRVs[i]), ImVec2(200, 200));
                    //ImGui::SameLine();
                }
                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }
    }

    if (PickedActor)
    {
        if (UAmbientLightComponent* AmbientLightComponent = PickedActor->GetComponentByClass<UAmbientLightComponent>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("AmbientLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                DrawColorProperty("Light Color",
                    [&]() { return AmbientLightComponent->GetLightColor(); },
                    [&](FLinearColor c) { AmbientLightComponent->SetLightColor(c); });
                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }
    }
    if (PickedActor)
    {
        if (ULightComponentBase* LightComponent = PickedActor->GetComponentByClass<ULightComponentBase>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            // --- "Override Camera" 버튼 추가 ---
            if (ImGui::Button("Override Camera with Light's Perspective"))
            {
                // 1. 라이트의 월드 위치 및 회전 가져오기
                FVector LightLocation = LightComponent->GetWorldLocation();

                FVector Forward = FVector(1.f, 0.f, 0.0f);
                Forward = JungleMath::FVectorRotate(Forward, LightLocation);
                FVector LightForward = Forward;
                FRotator LightRotation = LightComponent->GetWorldRotation();
                FVector LightRotationVecter;
                LightRotationVecter.X = LightRotation.Roll;
                LightRotationVecter.Y = -LightRotation.Pitch;
                LightRotationVecter.Z = LightRotation.Yaw;

                // 2. 활성 에디터 뷰포트 클라이언트 가져오기 (!!! 엔진별 구현 필요 !!!)
                std::shared_ptr<FEditorViewportClient> ViewportClient = GEngineLoop.GetLevelEditor()->GetActiveViewportClient(); // 위에 정의된 헬퍼 함수 사용 (또는 직접 구현)

                // 3. 뷰포트 클라이언트가 유효하면 카메라 설정
                if (ViewportClient)
                {
                    ViewportClient->PerspectiveCamera.SetLocation(LightLocation + LightForward); // 카메라 위치 설정 함수 호출
                    ViewportClient->PerspectiveCamera.SetRotation(LightRotationVecter); // 카메라 회전 설정 함수 호출

                    // 필요시 뷰포트 강제 업데이트/다시 그리기 호출
                    // ViewportClient->Invalidate();
                }
                else
                {
                    // 뷰포트 클라이언트를 찾을 수 없음 (오류 로그 등)
                    // UE_LOG(LogTemp, Warning, TEXT("Active Viewport Client not found."));
                }
            }
            ImGui::PopStyleColor();
        }
    }

    if (PickedActor)
    {
        if (UProjectileMovementComponent* ProjectileComp = (PickedActor->GetComponentByClass<UProjectileMovementComponent>()))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

            if (ImGui::TreeNodeEx("Projectile Movement Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
            {
                float InitialSpeed = ProjectileComp->GetInitialSpeed();
                if (ImGui::InputFloat("InitialSpeed", &InitialSpeed, 0.f, 10000.0f, "%.1f"))
                {
                    ProjectileComp->SetInitialSpeed(InitialSpeed);
                }

                float MaxSpeed = ProjectileComp->GetMaxSpeed();
                if (ImGui::InputFloat("MaxSpeed", &MaxSpeed, 0.f, 10000.0f, "%.1f"))
                {
                    ProjectileComp->SetMaxSpeed(MaxSpeed);
                }

                float Gravity = ProjectileComp->GetGravity();
                if (ImGui::InputFloat("Gravity", &Gravity, 0.f, 10000.f, "%.1f"))
                {
                    ProjectileComp->SetGravity(Gravity);
                }

                float ProjectileLifetime = ProjectileComp->GetLifetime();
                if (ImGui::InputFloat("Lifetime", &ProjectileLifetime, 0.f, 10000.f, "%.1f"))
                {
                    ProjectileComp->SetLifetime(ProjectileLifetime);
                }

                FVector CurrentVelocity = ProjectileComp->GetVelocity();

                float Velocity[3] = { CurrentVelocity.X, CurrentVelocity.Y, CurrentVelocity.Z };

                if (ImGui::InputFloat3("Velocity", Velocity, "%.1f"))
                {
                    ProjectileComp->SetVelocity(FVector(Velocity[0], Velocity[1], Velocity[2]));
                }

                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }
    }
    // TODO: 추후에 RTTI를 이용해서 프로퍼티 출력하기
    if (PickedActor)
    {
        if (UTextComponent* TextComponent = Cast<UTextComponent>(PickedActor->GetRootComponent()))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            if (ImGui::TreeNodeEx("Text Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
            {
                if (TextComponent)
                {
                    TextComponent->SetTexture(L"Assets/Texture/font.png");
                    TextComponent->SetRowColumnCount(106, 106);
                    FWString wText = TextComponent->GetText();
                    int Len = WideCharToMultiByte(CP_UTF8, 0, wText.c_str(), -1, nullptr, 0, nullptr, nullptr);
                    std::string u8Text(Len, '\0');
                    WideCharToMultiByte(CP_UTF8, 0, wText.c_str(), -1, u8Text.data(), Len, nullptr, nullptr);

                    static char Buf[256];
                    strcpy_s(Buf, u8Text.c_str());

                    ImGui::Text("Text: ", Buf);
                    ImGui::SameLine();
                    ImGui::PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);
                    if (ImGui::InputText("##Text", Buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        TextComponent->ClearText();
                        int wLen = MultiByteToWideChar(CP_UTF8, 0, Buf, -1, nullptr, 0);
                        FWString wNewText(wLen, L'\0');
                        MultiByteToWideChar(CP_UTF8, 0, Buf, -1, wNewText.data(), wLen);
                        TextComponent->SetText(wNewText.c_str());
                    }
                    ImGui::PopItemFlag();
                }
                ImGui::TreePop();
            }
            ImGui::PopStyleColor();
        }
    }

    // TODO: 추후에 RTTI를 이용해서 프로퍼티 출력하기
    if (PickedActor)
    {
        if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(PickedActor->GetRootComponent()))
        {
            RenderForStaticMesh(StaticMeshComponent);
            RenderForMaterial(StaticMeshComponent);
        }
    }

    if (PickedActor)
    {
        if (ACamera* cam = Cast<ACamera>(PickedActor))
        {
            float p[5] = {};
            p[0] = cam->TransitionCurve.Handle1.X;
            p[1] = cam->TransitionCurve.Handle1.Y;
            p[2] = cam->TransitionCurve.Handle2.X;
            p[3] = cam->TransitionCurve.Handle2.Y;
            ImGui::Bezier("Transition Curve", p);
            cam->TransitionCurve.Handle1.X = p[0];
            cam->TransitionCurve.Handle1.Y = p[1];
            cam->TransitionCurve.Handle2.X = p[2];
            cam->TransitionCurve.Handle2.Y = p[3];
        }
    }
    
    if (PickedActor)
    {
        if (UHeightFogComponent* FogComponent = Cast<UHeightFogComponent>(PickedActor->GetRootComponent()))
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
            if (ImGui::TreeNodeEx("Exponential Height Fog", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
            {
                FLinearColor CurrColor = FogComponent->GetFogColor();

                float R = CurrColor.R;
                float G = CurrColor.G;
                float B = CurrColor.B;
                float A = CurrColor.A;
                float H, S, V;
                float LightColor[4] = { R, G, B, A };

                // Fog Color
                if (ImGui::ColorPicker4("##Fog Color", LightColor,
                    ImGuiColorEditFlags_DisplayRGB |
                    ImGuiColorEditFlags_NoSidePreview |
                    ImGuiColorEditFlags_NoInputs |
                    ImGuiColorEditFlags_Float))
                {

                    R = LightColor[0];
                    G = LightColor[1];
                    B = LightColor[2];
                    A = LightColor[3];
                    FogComponent->SetFogColor(FLinearColor(R, G, B, A));
                }
                RGBToHSV(R, G, B, H, S, V);
                // RGB/HSV
                bool ChangedRGB = false;
                bool ChangedHSV = false;

                // RGB
                ImGui::PushItemWidth(50.0f);
                if (ImGui::DragFloat("R##R", &R, 0.001f, 0.f, 1.f)) ChangedRGB = true;
                ImGui::SameLine();
                if (ImGui::DragFloat("G##G", &G, 0.001f, 0.f, 1.f)) ChangedRGB = true;
                ImGui::SameLine();
                if (ImGui::DragFloat("B##B", &B, 0.001f, 0.f, 1.f)) ChangedRGB = true;
                ImGui::Spacing();

                // HSV
                if (ImGui::DragFloat("H##H", &H, 0.1f, 0.f, 360)) ChangedHSV = true;
                ImGui::SameLine();
                if (ImGui::DragFloat("S##S", &S, 0.001f, 0.f, 1)) ChangedHSV = true;
                ImGui::SameLine();
                if (ImGui::DragFloat("V##V", &V, 0.001f, 0.f, 1)) ChangedHSV = true;
                ImGui::PopItemWidth();
                ImGui::Spacing();

                if (ChangedRGB && !ChangedHSV)
                {
                    // RGB -> HSV
                    RGBToHSV(R, G, B, H, S, V);
                    FogComponent->SetFogColor(FLinearColor(R, G, B, A));
                }
                else if (ChangedHSV && !ChangedRGB)
                {
                    // HSV -> RGB
                    HSVToRGB(H, S, V, R, G, B);
                    FogComponent->SetFogColor(FLinearColor(R, G, B, A));
                }

                float FogDensity = FogComponent->GetFogDensity();
                if (ImGui::SliderFloat("Density", &FogDensity, 0.00f, 3.0f))
                {
                    FogComponent->SetFogDensity(FogDensity);
                }

                float FogDistanceWeight = FogComponent->GetFogDistanceWeight();
                if (ImGui::SliderFloat("Distance Weight", &FogDistanceWeight, 0.00f, 3.0f))
                {
                    FogComponent->SetFogDistanceWeight(FogDistanceWeight);
                }

                float FogHeightFallOff = FogComponent->GetFogHeightFalloff();
                if (ImGui::SliderFloat("Height Fall Off", &FogHeightFallOff, 0.001f, 0.15f))
                {
                    FogComponent->SetFogHeightFalloff(FogHeightFallOff);
                }

                float FogStartDistance = FogComponent->GetStartDistance();
                if (ImGui::SliderFloat("Start Distance", &FogStartDistance, 0.00f, 50.0f))
                {
                    FogComponent->SetStartDistance(FogStartDistance);
                }

                float FogEndtDistance = FogComponent->GetEndDistance();
                if (ImGui::SliderFloat("End Distance", &FogEndtDistance, 0.00f, 50.0f))
                {
                    FogComponent->SetEndDistance(FogEndtDistance);
                }

                ImGui::TreePop();
            }
            ImGui::PopStyleColor();
        }
    }
    

    ImGui::End();
}

void PropertyEditorPanel::RGBToHSV(const float R, const float G, const float B, float& H, float& S, float& V)
{
    const float MX = FMath::Max(R, FMath::Max(G, B));
    const float MN = FMath::Min(R, FMath::Min(G, B));
    const float Delta = MX - MN;

    V = MX;

    if (MX == 0.0f) {
        S = 0.0f;
        H = 0.0f;
        return;
    }
    else {
        S = Delta / MX;
    }

    if (Delta < 1e-6) {
        H = 0.0f;
    }
    else {
        if (R >= MX) {
            H = (G - B) / Delta;
        }
        else if (G >= MX) {
            H = 2.0f + (B - R) / Delta;
        }
        else {
            H = 4.0f + (R - G) / Delta;
        }
        H *= 60.0f;
        if (H < 0.0f) {
            H += 360.0f;
        }
    }
}

void PropertyEditorPanel::HSVToRGB(const float H, const float S, const float V, float& R, float& G, float& B)
{
    // h: 0~360, s:0~1, v:0~1
    const float C = V * S;
    const float Hp = H / 60.0f;             // 0~6 구간
    const float X = C * (1.0f - fabsf(fmodf(Hp, 2.0f) - 1.0f));
    const float M = V - C;

    if (Hp < 1.0f) { R = C;  G = X;  B = 0.0f; }
    else if (Hp < 2.0f) { R = X;  G = C;  B = 0.0f; }
    else if (Hp < 3.0f) { R = 0.0f; G = C;  B = X; }
    else if (Hp < 4.0f) { R = 0.0f; G = X;  B = C; }
    else if (Hp < 5.0f) { R = X;  G = 0.0f; B = C; }
    else { R = C;  G = 0.0f; B = X; }

    R += M;  G += M;  B += M;
}

void PropertyEditorPanel::RenderForStaticMesh(UStaticMeshComponent* StaticMeshComp)
{
    if (StaticMeshComp->GetStaticMesh() == nullptr)
    {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (ImGui::TreeNodeEx("Static Mesh", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        ImGui::Text("StaticMesh");
        ImGui::SameLine();

        FString PreviewName = StaticMeshComp->GetStaticMesh()->GetRenderData()->DisplayName;
        const TMap<FName, FAssetInfo> Assets = UAssetManager::Get().GetAssetRegistry();

        if (ImGui::BeginCombo("##StaticMesh", GetData(PreviewName), ImGuiComboFlags_None))
        {
            for (const auto& Asset : Assets)
            {
                if (ImGui::Selectable(GetData(Asset.Value.AssetName.ToString()), false))
                {
                    FString MeshName = Asset.Value.PackagePath.ToString() + "/" + Asset.Value.AssetName.ToString();
                    UStaticMesh* StaticMesh = FManagerOBJ::GetStaticMesh(MeshName.ToWideString());
                    if (StaticMesh)
                    {
                        StaticMeshComp->SetStaticMesh(StaticMesh);
                    }
                }
            }
            ImGui::EndCombo();
        }

        ImGui::TreePop();
    }
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (ImGui::TreeNodeEx("Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        ImGui::Text("Add");
        ImGui::SameLine();

        TArray<UClass*> CompClasses;
        GetChildOfClass(UActorComponent::StaticClass(), CompClasses);

        if (ImGui::BeginCombo("##AddComponent", "Components", ImGuiComboFlags_None))
        {
            for (UClass* Class : CompClasses)
            {
                if (ImGui::Selectable(GetData(Class->GetName()), false))
                {
                    USceneComponent* NewComp = Cast<USceneComponent>(StaticMeshComp->GetOwner()->AddComponent(Class));
                    if (NewComp)
                    {
                        NewComp->SetupAttachment(StaticMeshComp);
                    }
                    // 추후 Engine으로부터 SelectedComponent 받아서 선택된 Comp 아래로 붙일 수있으면 붙이기.
                }
            }
            ImGui::EndCombo();
        }

        ImGui::TreePop();
    }
    ImGui::PopStyleColor();
}


void PropertyEditorPanel::RenderForMaterial(UStaticMeshComponent* StaticMeshComp)
{
    if (StaticMeshComp->GetStaticMesh() == nullptr)
    {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (ImGui::TreeNodeEx("Materials", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        for (uint32 i = 0; i < StaticMeshComp->GetNumMaterials(); ++i)
        {
            if (ImGui::Selectable(GetData(StaticMeshComp->GetMaterialSlotNames()[i].ToString()), false, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    std::cout << GetData(StaticMeshComp->GetMaterialSlotNames()[i].ToString()) << std::endl;
                    SelectedMaterialIndex = i;
                    SelectedStaticMeshComp = StaticMeshComp;
                }
            }
        }

        if (ImGui::Button("    +    ")) {
            IsCreateMaterial = true;
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("SubMeshes", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        const auto Subsets = StaticMeshComp->GetStaticMesh()->GetRenderData()->MaterialSubsets;
        for (uint32 i = 0; i < Subsets.Num(); ++i)
        {
            std::string temp = "subset " + std::to_string(i);
            if (ImGui::Selectable(temp.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    StaticMeshComp->SetselectedSubMeshIndex(i);
                    SelectedStaticMeshComp = StaticMeshComp;
                }
            }
        }
        const std::string Temp = "clear subset";
        if (ImGui::Selectable(Temp.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
        {
            if (ImGui::IsMouseDoubleClicked(0))
                StaticMeshComp->SetselectedSubMeshIndex(-1);
        }

        ImGui::TreePop();
    }

    ImGui::PopStyleColor();

    if (SelectedMaterialIndex != -1)
    {
        RenderMaterialView(SelectedStaticMeshComp->GetMaterial(SelectedMaterialIndex));
    }
    if (IsCreateMaterial) {
        RenderCreateMaterialView();
    }
}

void PropertyEditorPanel::RenderMaterialView(UMaterial* Material)
{
    ImGui::SetNextWindowSize(ImVec2(380, 400), ImGuiCond_Once);
    ImGui::Begin("Material Viewer", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav);

    static ImGuiSelectableFlags BaseFlag = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoAlpha;

    const FVector MatDiffuseColor = Material->GetMaterialInfo().Diffuse;
    const FVector MatSpecularColor = Material->GetMaterialInfo().Specular;
    const FVector MatAmbientColor = Material->GetMaterialInfo().Ambient;
    const FVector MatEmissiveColor = Material->GetMaterialInfo().Emissive;

    const float DiffuseR = MatDiffuseColor.X;
    const float DiffuseG = MatDiffuseColor.Y;
    const float DiffuseB = MatDiffuseColor.Z;
    constexpr float DiffuseA = 1.0f;
    float DiffuseColorPick[4] = { DiffuseR, DiffuseG, DiffuseB, DiffuseA };

    ImGui::Text("Material Name |");
    ImGui::SameLine();
    ImGui::Text(*Material->GetMaterialInfo().MaterialName);
    ImGui::Separator();

    ImGui::Text("  Diffuse Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Diffuse##Color", reinterpret_cast<float*>(&DiffuseColorPick), BaseFlag))
    {
        const FVector NewColor = { DiffuseColorPick[0], DiffuseColorPick[1], DiffuseColorPick[2] };
        Material->SetDiffuse(NewColor);
    }

    const float SpecularR = MatSpecularColor.X;
    const float SpecularG = MatSpecularColor.Y;
    const float SpecularB = MatSpecularColor.Z;
    constexpr float SpecularA = 1.0f;
    float SpecularColorPick[4] = { SpecularR, SpecularG, SpecularB, SpecularA };

    ImGui::Text("Specular Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Specular##Color", reinterpret_cast<float*>(&SpecularColorPick), BaseFlag))
    {
        const FVector NewColor = { SpecularColorPick[0], SpecularColorPick[1], SpecularColorPick[2] };
        Material->SetSpecular(NewColor);
    }

    const float AmbientR = MatAmbientColor.X;
    const float AmbientG = MatAmbientColor.Y;
    const float AmbientB = MatAmbientColor.Z;
    constexpr float AmbientA = 1.0f;
    float AmbientColorPick[4] = { AmbientR, AmbientG, AmbientB, AmbientA };

    ImGui::Text("Ambient Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Ambient##Color", reinterpret_cast<float*>(&AmbientColorPick), BaseFlag))
    {
        const FVector NewColor = { AmbientColorPick[0], AmbientColorPick[1], AmbientColorPick[2] };
        Material->SetAmbient(NewColor);
    }

    const float EmissiveR = MatEmissiveColor.X;
    const float EmissiveG = MatEmissiveColor.Y;
    const float EmissiveB = MatEmissiveColor.Z;
    constexpr float EmissiveA = 1.0f;
    float EmissiveColorPick[4] = { EmissiveR, EmissiveG, EmissiveB, EmissiveA };

    ImGui::Text("Emissive Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Emissive##Color", reinterpret_cast<float*>(&EmissiveColorPick), BaseFlag))
    {
        const FVector NewColor = { EmissiveColorPick[0], EmissiveColorPick[1], EmissiveColorPick[2] };
        Material->SetEmissive(NewColor);
    }

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Text("Choose Material");
    ImGui::Spacing();

    ImGui::Text("Material Slot Name |");
    ImGui::SameLine();
    ImGui::Text(GetData(SelectedStaticMeshComp->GetMaterialSlotNames()[SelectedMaterialIndex].ToString()));

    ImGui::Text("Override Material |");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(160);
    // 메테리얼 이름 목록을 const char* 배열로 변환
    std::vector<const char*> MaterialChars;
    for (const auto& Material : FManagerOBJ::GetMaterials()) {
        MaterialChars.push_back(*Material.Value->GetMaterialInfo().MaterialName);
    }

    //// 드롭다운 표시 (currentMaterialIndex가 범위를 벗어나지 않도록 확인)
    //if (currentMaterialIndex >= FManagerOBJ::GetMaterialNum())
    //    currentMaterialIndex = 0;

    if (ImGui::Combo("##MaterialDropdown", &CurMaterialIndex, MaterialChars.data(), FManagerOBJ::GetMaterialNum())) {
        UMaterial* Material = FManagerOBJ::GetMaterial(MaterialChars[CurMaterialIndex]);
        SelectedStaticMeshComp->SetMaterial(SelectedMaterialIndex, Material);
    }

    if (ImGui::Button("Close"))
    {
        SelectedMaterialIndex = -1;
        SelectedStaticMeshComp = nullptr;
    }

    ImGui::End();
}

void PropertyEditorPanel::RenderCreateMaterialView()
{
    ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Once);
    ImGui::Begin("Create Material Viewer", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav);

    static ImGuiSelectableFlags BaseFlag = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoAlpha;

    ImGui::Text("New Name");
    ImGui::SameLine();
    static char MaterialName[256] = "New Material";
    // 기본 텍스트 입력 필드
    ImGui::SetNextItemWidth(128);
    if (ImGui::InputText("##NewName", MaterialName, IM_ARRAYSIZE(MaterialName))) {
        tempMaterialInfo.MaterialName = MaterialName;
    }

    const FVector MatDiffuseColor = tempMaterialInfo.Diffuse;
    const FVector MatSpecularColor = tempMaterialInfo.Specular;
    const FVector MatAmbientColor = tempMaterialInfo.Ambient;
    const FVector MatEmissiveColor = tempMaterialInfo.Emissive;

    const float DiffuseR = MatDiffuseColor.X;
    const float DiffuseG = MatDiffuseColor.Y;
    const float DiffuseB = MatDiffuseColor.Z;
    constexpr float DiffuseA = 1.0f;
    float DiffuseColorPick[4] = { DiffuseR, DiffuseG, DiffuseB, DiffuseA };

    ImGui::Text("Set Property");
    ImGui::Indent();

    ImGui::Text("  Diffuse Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Diffuse##Color", reinterpret_cast<float*>(&DiffuseColorPick), BaseFlag))
    {
        const FVector NewColor = { DiffuseColorPick[0], DiffuseColorPick[1], DiffuseColorPick[2] };
        tempMaterialInfo.Diffuse = NewColor;
    }

    const float SpecularR = MatSpecularColor.X;
    const float SpecularG = MatSpecularColor.Y;
    const float SpecularB = MatSpecularColor.Z;
    constexpr float SpecularA = 1.0f;
    float SpecularColorPick[4] = { SpecularR, SpecularG, SpecularB, SpecularA };

    ImGui::Text("Specular Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Specular##Color", reinterpret_cast<float*>(&SpecularColorPick), BaseFlag))
    {
        const FVector NewColor = { SpecularColorPick[0], SpecularColorPick[1], SpecularColorPick[2] };
        tempMaterialInfo.Specular = NewColor;
    }

    const float AmbientR = MatAmbientColor.X;
    const float AmbientG = MatAmbientColor.Y;
    const float AmbientB = MatAmbientColor.Z;
    constexpr float AmbientA = 1.0f;
    float AmbientColorPick[4] = { AmbientR, AmbientG, AmbientB, AmbientA };

    ImGui::Text("Ambient Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Ambient##Color", reinterpret_cast<float*>(&AmbientColorPick), BaseFlag))
    {
        const FVector NewColor = { AmbientColorPick[0], AmbientColorPick[1], AmbientColorPick[2] };
        tempMaterialInfo.Ambient = NewColor;
    }

    const float EmissiveR = MatEmissiveColor.X;
    const float EmissiveG = MatEmissiveColor.Y;
    const float EmissiveB = MatEmissiveColor.Z;
    constexpr float EmissiveA = 1.0f;
    float EmissiveColorPick[4] = { EmissiveR, EmissiveG, EmissiveB, EmissiveA };

    ImGui::Text("Emissive Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Emissive##Color", reinterpret_cast<float*>(&EmissiveColorPick), BaseFlag))
    {
        const FVector NewColor = { EmissiveColorPick[0], EmissiveColorPick[1], EmissiveColorPick[2] };
        tempMaterialInfo.Emissive = NewColor;
    }
    ImGui::Unindent();

    ImGui::NewLine();
    if (ImGui::Button("Create Material")) {
        FManagerOBJ::CreateMaterial(tempMaterialInfo);
    }

    ImGui::NewLine();
    if (ImGui::Button("Close"))
    {
        IsCreateMaterial = false;
    }

    ImGui::End();
}

void PropertyEditorPanel::RenderLuaScriptEdit(UScriptableComponent* LuaScriptComp)
{
    namespace fs = std::filesystem; // Use alias for brevity

    if (!LuaScriptComp) return;

    AActor* OwnerActor = Cast<AActor>(LuaScriptComp->GetOwner());
    if (!OwnerActor) return;

    // --- Configuration ---
    // TEXT() macro resolves to L"" on Windows Unicode builds (standard for Unreal)
    // std::filesystem::path works correctly with wide strings.
    const fs::path ScriptSaveFolder = fs::path(TEXT("Saved")) / TEXT("LuaScripts");
    const fs::path TemplateScriptPath = ScriptSaveFolder / TEXT("template.lua");
    const std::wstring DefaultExtension = L".lua";

    // --- ImGui Styling ---
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.15f, 0.1f, 1.0f));

    if (ImGui::TreeNodeEx("Lua Scripting", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
    {
        // --- State Variables ---
        FString CurrentScriptNameFStr = LuaScriptComp->ScriptName;
        std::wstring CurrentScriptNameWStr = CurrentScriptNameFStr.ToWideString();
        fs::path FullScriptPath; // Full path to the *currently assigned* script
        bool bHasScriptName = !CurrentScriptNameWStr.empty();

        // Buffer for the new script name input field
        static char NewScriptNameBuffer[256] = ""; // Keep state between frames

        // Calculate full path if a script name exists
        if (bHasScriptName) {
            try {
                // Ensure we only take the filename part from the stored string,
                // just in case it somehow contains path separators.
                FullScriptPath = ScriptSaveFolder / fs::path(CurrentScriptNameWStr).filename();
            }
            catch (const std::exception& e) {
                // Log using Unreal's logging system
                UE_LOG(LogLevel::Error, TEXT("Error creating full script path from existing name '%s': %s"),
                    *CurrentScriptNameFStr, *FString("Generic path error"));
                // If path creation fails, treat as if no script name exists for safety
                bHasScriptName = false;
                CurrentScriptNameWStr.clear();
                LuaScriptComp->ScriptName = FString(); // Clear invalid script name
            }
        }

        // --- UI Elements ---

        // 1. Create Script Button (Always Visible)
        if (ImGui::Button("Create Script"))
        {
            std::string NewScriptNameUtf8 = NewScriptNameBuffer;
            if (!NewScriptNameUtf8.empty())
            {
                // Convert UTF-8 input from ImGui to wstring
                std::wstring NewScriptNameBaseWStr;
                int BufferSize = MultiByteToWideChar(CP_UTF8, 0, NewScriptNameUtf8.c_str(), -1, NULL, 0);
                if (BufferSize > 0) {
                    NewScriptNameBaseWStr.resize(BufferSize - 1); // Exclude null terminator
                    MultiByteToWideChar(CP_UTF8, 0, NewScriptNameUtf8.c_str(), -1, &NewScriptNameBaseWStr[0], BufferSize);
                }

                if (!NewScriptNameBaseWStr.empty())
                {
                    // Append extension
                    std::wstring NewScriptNameWithExtWStr = NewScriptNameBaseWStr + DefaultExtension;
                    fs::path DestPath;

                    try {
                        DestPath = ScriptSaveFolder / NewScriptNameWithExtWStr;

                        // Create directory if it doesn't exist
                        if (!fs::exists(ScriptSaveFolder)) {
                            std::error_code ec;
                            if (!fs::create_directories(ScriptSaveFolder, ec)) {
                                // Check again after attempting creation, another process might have created it
                                if (!fs::exists(ScriptSaveFolder)) {
                                    // Throw if creation failed and directory still doesn't exist
                                    throw fs::filesystem_error("Failed to create directory", ScriptSaveFolder, ec);
                                }
                            }
                            else {
                                UE_LOG(LogLevel::Display, TEXT("Created directory: %s"), *FString(ScriptSaveFolder.wstring().c_str()));
                            }
                        }

                        // Check if template exists before copying
                        if (fs::exists(TemplateScriptPath)) {
                            std::error_code copy_ec;
                            fs::copy_file(TemplateScriptPath, DestPath, fs::copy_options::skip_existing, copy_ec); // Use skip_existing

                            if (copy_ec) {
                                // Log error but continue, maybe the file existed and skip_existing worked as intended
                                UE_LOG(LogLevel::Warning, TEXT("Filesystem warning during script copy (template to '%s'): Code=%d, Message=%hs"),
                                    *FString(DestPath.wstring().c_str()), copy_ec.value(), copy_ec.message().c_str());
                                // It's okay if it already exists, we still want to assign the name
                            }
                            else {
                                UE_LOG(LogLevel::Display, TEXT("Copied template to create Lua script: %s"), *FString(DestPath.wstring().c_str()));
                            }

                            // --- Success: Update component and UI state ---
                            LuaScriptComp->ScriptName = FString(DestPath.c_str()); // Assign the new name with extension

                            // Update local state for immediate UI feedback
                            CurrentScriptNameFStr = LuaScriptComp->ScriptName;
                            CurrentScriptNameWStr = NewScriptNameWithExtWStr;
                            FullScriptPath = DestPath;
                            bHasScriptName = true;

                            // Clear the input buffer after successful creation
                            NewScriptNameBuffer[0] = '\0';

                            // Optional: Reload the script if the component supports it
                            // LuaScriptComp->ReloadScript();

                        }
                        else {
                            UE_LOG(LogLevel::Error, TEXT("Template script file not found, cannot create new script: %s"), *FString(TemplateScriptPath.wstring().c_str()));
                            // Optionally show an ImGui error popup
                            // ImGui::OpenPopup("Template Missing");
                        }
                    }
                    catch (const fs::filesystem_error& e) {
                        UE_LOG(LogLevel::Error, TEXT("Filesystem error during script creation: Path1='%s', Path2='%s', Code=%d, Message=%hs"),
                            *FString(e.path1().wstring().c_str()), *FString(e.path2().wstring().c_str()), e.code().value(), e.what());
                        // Optionally show an ImGui error popup
                    }
                    catch (const std::exception& e) {
                        UE_LOG(LogLevel::Error, TEXT("Generic error during script creation: %hs"), e.what());
                        // Optionally show an ImGui error popup
                    }
                }
                else {
                    UE_LOG(LogLevel::Warning, TEXT("Cannot create script with an empty name."));
                    // Optionally show an ImGui warning
                }
            }
            else {
                UE_LOG(LogLevel::Warning, TEXT("New script name input is empty."));
                // Optionally show an ImGui warning
            }
        } // End Create Script Button

        // 2. Edit Script Button (Only if ScriptName exists)
        if (bHasScriptName)
        {
            ImGui::SameLine(); // Place next to Create Script button
            if (ImGui::Button("Edit Script"))
            {
                try {
                    // Re-check existence right before opening
                    if (fs::exists(FullScriptPath))
                    {
                        std::wstring PathToOpenWStr = fs::absolute(FullScriptPath).wstring(); // Use absolute path for ShellExecute

                        // Use ShellExecuteW for wide strings on Windows
                        HINSTANCE InstanceHandle = ShellExecuteW(
                            NULL,        // Owner window handle (optional)
                            L"open",     // Operation (open, edit, print, etc.)
                            PathToOpenWStr.c_str(), // File path
                            NULL,        // Parameters (if any)
                            NULL,        // Default directory (can be NULL)
                            SW_SHOWNORMAL // Show command
                        );

                        // Check ShellExecuteW result (<= 32 indicates an error)
                        if ((INT_PTR)InstanceHandle <= 32) {
                            DWORD ErrorCode = GetLastError(); // Get detailed error code
                            UE_LOG(LogLevel::Error, TEXT("Failed to open script file '%s' with ShellExecuteW. Error Code: %d"),
                                *FString(PathToOpenWStr.c_str()), ErrorCode);
                            // Consider providing more user-friendly error messages based on common error codes
                        }
                        else {
                            UE_LOG(LogLevel::Display, TEXT("Attempting to open script: %s"), *FString(PathToOpenWStr.c_str()));
                        }
                    }
                    else {
                        UE_LOG(LogLevel::Error, TEXT("Script file not found for editing, cannot open: %s"), *FString(FullScriptPath.wstring().c_str()));
                        // Consider removing the invalid script name from the component here
                        // LuaScriptComp->ScriptName = FString();
                        // bHasScriptName = false; // Update UI state
                        // CurrentScriptNameWStr.clear();
                    }
                }
                catch (const fs::filesystem_error& e) {
                    UE_LOG(LogLevel::Error, TEXT("Filesystem error checking script existence before edit: Path='%s', Code=%d, Message=%hs"),
                        *FString(e.path1().wstring().c_str()), e.code().value(), e.what());
                }
                catch (const std::exception& e) {
                    UE_LOG(LogLevel::Error, TEXT("Generic error checking script existence before edit: %hs"), e.what());
                }
            } // End Edit Script Button
        } // End if(bHasScriptName) for Edit Button

         // 1. Input field for new script name
        ImGui::Spacing();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x ); // Adjust width as needed
        ImGui::InputTextWithHint("##NewScriptName", "Enter New Script Name", NewScriptNameBuffer, sizeof(NewScriptNameBuffer));
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // 4. Display Current Script File Path (Read-Only)
        ImGui::Spacing(); // Add some vertical space
        ImGui::Text("Current Script:");
        ImGui::SameLine();

        // Convert wstring to UTF-8 for ImGui display
        std::string CurrentScriptNameUtf8;
        if (!CurrentScriptNameWStr.empty()) {
            int BufferSize = WideCharToMultiByte(CP_UTF8, 0, CurrentScriptNameWStr.c_str(), -1, NULL, 0, NULL, NULL);
            if (BufferSize > 0) {
                std::vector<char> Buffer(BufferSize); // Use vector for dynamic size
                WideCharToMultiByte(CP_UTF8, 0, CurrentScriptNameWStr.c_str(), -1, Buffer.data(), BufferSize, NULL, NULL);
                CurrentScriptNameUtf8 = Buffer.data(); // Assign from buffer data
            }
        }
        // Use a temporary buffer for ImGui, as InputText modifies its buffer argument if not ReadOnly
        char DisplayScriptNameBuffer[512];
        strncpy_s(DisplayScriptNameBuffer, sizeof(DisplayScriptNameBuffer), CurrentScriptNameUtf8.c_str(), _TRUNCATE);

        ImGui::PushItemWidth(-1); // Use full available width
        ImGui::InputText("##CurrentScriptNameDisplay", DisplayScriptNameBuffer, sizeof(DisplayScriptNameBuffer), ImGuiInputTextFlags_ReadOnly);
        ImGui::PopItemWidth();

        // --- End of TreeNode ---
        ImGui::TreePop();
    } // End TreeNode "Lua Scripting"

    // --- Pop Style ---
    ImGui::PopStyleColor();
    //if (!LuaScriptComp) return;

    //AActor* OwnerActor = Cast<AActor>(LuaScriptComp->GetOwner());
    //if (!OwnerActor) return;

    //namespace FileSystem = std::filesystem; // 네임스페이스 앨리어스

    //const FileSystem::path ScriptSaveFolder = FileSystem::path(TEXT("Saved")) / TEXT("LuaScripts");
    //const FileSystem::path TemplateScriptPath = ScriptSaveFolder / TEXT("template.lua");
    //const std::wstring DefaultExtension = L".lua";
    //
    //ImGui::Separator();
    //ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.15f, 0.1f, 1.0f));

    //if (ImGui::TreeNodeEx("Lua Scripting", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
    //{
    //    FString CurrentScriptNameFStr = LuaScriptComp->ScriptName;
    //    std::wstring CurrentScriptNameWStr = CurrentScriptNameFStr.ToWideString();
    //    FileSystem::path FullScriptPath;
    //    bool bHasScriptName = !CurrentScriptNameWStr.empty();

    //    // Buffer for the new script name input field
    //    static char NewScriptNameBuffer[256] = ""; // Keep state between frames

    //    if (bHasScriptName) {
    //        try {
    //            FullScriptPath = ScriptSaveFolder / FileSystem::path(CurrentScriptNameWStr).filename();
    //        }
    //        catch (const std::exception& e) {
    //            UE_LOG(LogLevel::Error, TEXT("Error creating script path: %s"), *FString("Generic path error"));
    //            bHasScriptName = false;
    //        }
    //    }

    //    // Input field for new script name
    //    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.6f); // Adjust width as needed
    //    ImGui::InputTextWithHint("##NewScriptName", "Enter New Script Name (e.g., MyActorLogic)", NewScriptNameBuffer, sizeof(NewScriptNameBuffer));
    //    ImGui::PopItemWidth();
    //    ImGui::SameLine();

    //    if (!bHasScriptName)
    //    {
    //        if (ImGui::Button("Create Script"))
    //        {
    //            // 스크립트 이름 생성
    //            FString SceneNameFStr = OwnerActor->GetWorld() ? OwnerActor->GetWorld()->GetName() : TEXT("UnknownScene");
    //            FString ActorNameFStr = OwnerActor->GetActorLabel().IsEmpty() ? OwnerActor->GetFName().ToString() : OwnerActor->GetActorLabel();

    //            std::wstring SceneNameWStr = SceneNameFStr.ToWideString();
    //            std::wstring ActorNameWStr = ActorNameFStr.ToWideString();
    //            std::replace(ActorNameWStr.begin(), ActorNameWStr.end(), L' ', L'_');

    //            std::wstring NewScriptNameWStr = SceneNameWStr + L"_" + ActorNameWStr + L".lua";
    //            FileSystem::path DestPath;

    //            try {
    //                DestPath = ScriptSaveFolder / NewScriptNameWStr;

    //                // 디렉토리 생성
    //                if (!FileSystem::exists(ScriptSaveFolder)) {
    //                    if (!FileSystem::create_directories(ScriptSaveFolder)) {
    //                        if (!FileSystem::exists(ScriptSaveFolder)) {
    //                            throw FileSystem::filesystem_error("Failed to create directory", ScriptSaveFolder, std::make_error_code(std::errc::operation_not_permitted));
    //                        }
    //                    }
    //                    UE_LOG(LogLevel::Display, TEXT("Created directory: %s"), *FString(ScriptSaveFolder.wstring().c_str()));
    //                }

    //                // 템플릿 파일 존재 확인
    //                if (FileSystem::exists(TemplateScriptPath)) {
    //                    // 파일 복사
    //                    FileSystem::copy_file(TemplateScriptPath, DestPath, FileSystem::copy_options::skip_existing);

    //                    // 컴포넌트에 스크립트 이름 설정
    //                    LuaScriptComp->ScriptName = FString(NewScriptNameWStr); // FString(const std::wstring&) 사용

    //                    // UI 업데이트
    //                    CurrentScriptNameWStr = NewScriptNameWStr;
    //                    FullScriptPath = DestPath;
    //                    bHasScriptName = true;
    //                    UE_LOG(LogLevel::Display, TEXT("Lua script created: %s"), *FString(DestPath.wstring().c_str()));

    //                    // 필요시 Reload 호출
    //                    // LuaComp->ReloadScript();

    //                }
    //                else {
    //                    UE_LOG(LogLevel::Error, TEXT("Template script file not found: %s"), *FString(TemplateScriptPath.wstring().c_str()));
    //                }
    //            }
    //            catch (const FileSystem::filesystem_error& e) {
    //                UE_LOG(LogLevel::Error, TEXT("Filesystem error during script creation: Path='%s', Code=%d, Message=%s"),
    //                    *FString(e.path1().wstring().c_str()), e.code().value(), *FString(e.what()));
    //            }
    //            catch (const std::exception& e) {
    //                UE_LOG(LogLevel::Error, TEXT("Error during script creation: %s"), *FString("Generic error"));
    //            }
    //        }
    //    }
    //    else // bHasScriptName == true
    //    {
    //        if (ImGui::Button("Edit Script"))
    //        {
    //            try {
    //                if (FileSystem::exists(FullScriptPath))
    //                {
    //                    std::wstring PathWStr = FullScriptPath.wstring();
    //                    std::wstring ParentPathWStr = FullScriptPath.parent_path().wstring();
    //                    HINSTANCE InstanceHandle = ShellExecuteW(
    //                        NULL, L"open", PathWStr.c_str(), NULL, NULL, SW_SHOWNORMAL);

    //                    if ((INT_PTR)InstanceHandle <= 32) {
    //                        DWORD ErrorCode = GetLastError();
    //                        UE_LOG(LogLevel::Error, TEXT("Failed to open script file '%s' with ShellExecuteW. Error Code: %d"), *FString(PathWStr.c_str()), ErrorCode);
    //                    }
    //                }
    //                else {
    //                    UE_LOG(LogLevel::Error, TEXT("Script file not found for editing: %s"), *FString(FullScriptPath.wstring().c_str()));
    //                }
    //            }
    //            catch (const FileSystem::filesystem_error& e) {
    //                UE_LOG(LogLevel::Error, TEXT("Filesystem error checking script existence: Path='%s', Code=%d, Message=%s"),
    //                    *FString(e.path1().wstring().c_str()), e.code().value(), *FString(e.what()));
    //            }
    //            catch (const std::exception& e) {
    //                UE_LOG(LogLevel::Error, TEXT("Error checking script existence: %s"), *FString("Generic error"));
    //            }
    //        }
    //    }

    //    // 2. 스크립트 파일 경로 표시
    //    ImGui::Text("Script File:");
    //    ImGui::SameLine();
    //    // wstring -> UTF-8 char* for ImGui
    //    std::string Utf8ScriptName;
    //    if (!CurrentScriptNameWStr.empty()) {
    //        int BufferSize = WideCharToMultiByte(CP_UTF8, 0, CurrentScriptNameWStr.c_str(), -1, NULL, 0, NULL, NULL);
    //        if (BufferSize > 0) {
    //            Utf8ScriptName.resize(BufferSize - 1);
    //            WideCharToMultiByte(CP_UTF8, 0, CurrentScriptNameWStr.c_str(), -1, &Utf8ScriptName[0], BufferSize, NULL, NULL);
    //        }
    //    }
    //    char ScriptNameBuffer[512];
    //    strncpy_s(ScriptNameBuffer, sizeof(ScriptNameBuffer), Utf8ScriptName.c_str(), _TRUNCATE);
    //    ImGui::PushItemWidth(-1);
    //    ImGui::InputText("##ScriptName", ScriptNameBuffer, sizeof(ScriptNameBuffer), ImGuiInputTextFlags_ReadOnly);
    //    ImGui::PopItemWidth();

    //    ImGui::TreePop(); // "Lua Scripting" TreeNode 닫기
    //}
    //ImGui::PopStyleColor();
}

void PropertyEditorPanel::OnResize(HWND hWnd)
{
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = ClientRect.right - ClientRect.left;
    Height = ClientRect.bottom - ClientRect.top;
}
