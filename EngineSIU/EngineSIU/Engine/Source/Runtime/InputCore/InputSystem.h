#include "InputCore/InputCoreTypes.h"

class FInputSystem
{
public:
    void Initialize();
    void Update();                          // 매 프레임 키 상태 갱신
    bool isKeyDown(EKeys::Type Key) const;

private:
    TMap<EKeys::Type, bool> KeyStates;
};
