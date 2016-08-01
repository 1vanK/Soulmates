#pragma once
#include <Urho3D/Urho3DAll.h>
#include "BoardLogic.h"

#define GLOBAL GetSubsystem<Global>()

#define DEFAULT_VOLUME 3
#define MAX_VOLUME 3

// Состояния игры.
enum GameState
{
    GS_START_MENU,
    GS_GAMEPLAY,
    GS_GAME_OVER
};

class Global : public Object
{
    URHO3D_OBJECT(Global, Object);

public:
    // Громкость звуков. Принимает значения от 0 до 3 (0%, 33%, 66%, 100%).
    int soundVolume_ = DEFAULT_VOLUME;
    // Громкость музыки. Принимает значения от 0 до 3.
    int musicVolume_ = DEFAULT_VOLUME;

    // Игра использует только одну сцену.
    Scene* scene_ = nullptr;
    // Быстрый доступ к ноде игровой доски.
    Node* boardNode_ = nullptr;
    
    // Корневая нода для всех источников звука. Не принадлежит ни одной сцене.
    SharedPtr<Node> soundRoot_;
    // Нода для музыкального проигрывателя. Не принадлежит ни одной сцене.
    SharedPtr<Node> musicNode_;

    // Текущее игровое состояние.
    GameState gameState_ = GS_START_MENU;
    // Игровое состояние, которое будет установлено в начале
    // следующей итерации игрового цикла.
    GameState neededGameState_ = GS_START_MENU;

    Global(Context* context);

    // Одинаковые звуки не будут воспроизводиться одновременно.
    void PlaySound(const String& fileName);
    
    // Однотипные звуки не будут воспроизводиться одновременно.
    // Будут проигрываться файлы с именами fileNameBegin + Random(num_variations) + ".wav".
    // Один и тот же файл не будет проигрываться два раза подряд.
    void PlaySound(const String& type, const String& fileNameBegin, int num_variations);
    
    void PlayMusic(const String& fileName);

    // Игровое состояние в виде строки.
    String GameStateToString();

    // Изменяет громкость звуков в соответствии со значением soundVolume_
    // и обновляет внешний вид регулятора громкости.
    void ApplySoundVolume();

    // Изменяет громкость музыки в соответствии со значением musicVolume_
    // и обновляет внешний вид регулятора громкости.
    void ApplyMusicVolume();
};
