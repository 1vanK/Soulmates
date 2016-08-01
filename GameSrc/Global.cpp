#include "Global.h"
#include "Urho3DAliases.h"
#include "UIManager.h"

Global::Global(Context* context) :
    Object(context)
{
    soundRoot_ = new Node(context);
    musicNode_ = new Node(context);
}

static const String GameStates[]
{
    "StartMenu",
    "Gameplay",
    "GameOver"
};

String Global::GameStateToString()
{
    return GameStates[gameState_];
}

void Global::PlaySound(const String& fileName)
{
    // Имя ноды равно имени проигрываемого звукового файла.
    Node* soundNode = soundRoot_->GetChild(fileName);
    if (!soundNode)
        soundNode = soundRoot_->CreateChild(fileName);

    SoundSource* soundSource = soundNode->GetOrCreateComponent<SoundSource>();

    if (!soundSource->IsPlaying())
        soundSource->Play(GET_SOUND(fileName));
}

void Global::PlaySound(const String& type, const String& fileNameBegin, int num_variations)
{
    Node* soundNode = soundRoot_->GetChild(type);
    if (!soundNode)
    {
        soundNode = soundRoot_->CreateChild(type);
        // Индекс последнего проигранного звука хранится в переменной ноды.
        // -1 означает, что еще ни один звук данного типа не был проигран.
        soundNode->SetVar("OldNum", -1);
    }

    SoundSource* soundSource = soundNode->GetOrCreateComponent<SoundSource>();
    if (soundSource->IsPlaying())
        return;

    if (num_variations <= 1)
    {
        soundSource->Play(GET_SOUND(fileNameBegin + "0.wav"));
        soundNode->SetVar("OldNum", 0);
        return;
    }

    int oldNum = soundNode->GetVar("OldNum").GetInt();

    while (true)
    {
        int rndNum = Random(num_variations);

        // Не проигрываем один и тот же звук дважды.
        if (rndNum == oldNum)
            continue;

        soundSource->Play(GET_SOUND(fileNameBegin + String(rndNum) + ".wav"));
        soundNode->SetVar("OldNum", rndNum);
        break;
    }
}

void Global::PlayMusic(const String& fileName)
{
    Sound* music = GET_SOUND(fileName);
    music->SetLooped(true);

    SoundSource* musicSource = musicNode_->GetOrCreateComponent<SoundSource>();
    musicSource->SetSoundType(SOUND_MUSIC);
    musicSource->Play(music);
}

void Global::ApplySoundVolume()
{
    AUDIO->SetMasterGain(SOUND_EFFECT, 0.333f * GLOBAL->soundVolume_);

    String styleName = String("VolumeButton") + String(GLOBAL->soundVolume_);
    UI_MANAGER->soundButton_->SetStyle(styleName);
}

void Global::ApplyMusicVolume()
{
    AUDIO->SetMasterGain(SOUND_MUSIC, 0.333f * GLOBAL->musicVolume_);

    String styleName = String("VolumeButton") + String(GLOBAL->musicVolume_);
    UI_MANAGER->musicButton_->SetStyle(styleName);
}
