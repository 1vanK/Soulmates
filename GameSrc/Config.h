/*
Подсистема для загрузки и сохранения настроек.
*/

#pragma once
#include <Urho3D/Urho3DAll.h>

#define CONFIG GetSubsystem<Config>()

class Config : public Object
{
    URHO3D_OBJECT(Config, Object);

public:
    Config(Context* context);

    void Load();
    void Save();

    int GetInt(const String& name, int defaultValue);
    int GetInt(const String& name, int defaultValue, int clampMin, int clampMax);
    void SetInt(const String& name, int value);

    int GetRecord(String boardMode);
    void SetRecord(String boardMode, int value);

private:
    SharedPtr<XMLFile> xmlFile_;
    HashMap<unsigned, int> records_;

    String GetConfigFileName();
};
