#include "InputCoreTypes.h"

FInputKeyManager::FInputKeyManager()
{
    // NOTICE : 키 추가시 매핑 세팅 필요
    KeyMapVirtualToEnum.Add('W', EKeys::W);
    KeyMapVirtualToEnum.Add('A', EKeys::A);
    KeyMapVirtualToEnum.Add('S', EKeys::S);
    KeyMapVirtualToEnum.Add('D', EKeys::D);
    KeyMapVirtualToEnum.Add(VK_SPACE, EKeys::SpaceBar);

    KeyMapVirtualToEnum.Add(VK_LBUTTON, EKeys::LeftMouseButton);
    KeyMapVirtualToEnum.Add(VK_RBUTTON, EKeys::RightMouseButton);
    KeyMapVirtualToEnum.Add(VK_MBUTTON, EKeys::MiddleMouseButton);
}

FInputKeyManager& FInputKeyManager::Get()
{
    static FInputKeyManager Instance;
    return Instance;
}

EKeys::Type FInputKeyManager::GetKeyFromVirtualKey(uint32 VirtualKey) const
{
    if (auto Found = KeyMapVirtualToEnum.Find(VirtualKey))
    {
        return *Found;
    }
    return EKeys::Invalid;
}
