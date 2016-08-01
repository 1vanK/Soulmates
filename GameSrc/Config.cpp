#include "Config.h"
#include "Urho3DAliases.h"

Config::Config(Context* context) : Object(context)
{
    xmlFile_ = new XMLFile(context);
}

String Config::GetConfigFileName()
{
    return FILE_SYSTEM->GetAppPreferencesDir("1vanK", "Soulmates") + "Config.xml";
}

void Config::Load()
{
    String fileName = GetConfigFileName();

    if (FILE_SYSTEM->FileExists(fileName))
    {
        File file(context_, fileName, FILE_READ);
        xmlFile_->Load(file);
    }

    // Если файл не был загружен, то корень будет отсутствовать.
    if (xmlFile_->GetRoot().IsNull())
        xmlFile_->CreateRoot("Config");

    // Создаем таблицу рекордов, если она отсутствует.
    XMLElement table = xmlFile_->GetRoot().GetChild("Records");
    if (table.IsNull())
        xmlFile_->GetRoot().CreateChild("Records");
}

void Config::Save()
{
    String fileName = GetConfigFileName();
    File file(context_, fileName, FILE_WRITE);
    xmlFile_->Save(file);
}

int Config::GetInt(const String& name, int defaultValue)
{
    XMLElement root = xmlFile_->GetRoot();

    if (root.HasAttribute(name))
        return root.GetInt(name);

    return defaultValue;
}

int Config::GetInt(const String& name, int defaultValue, int clampMin, int clampMax)
{
    int value = GetInt(name, defaultValue);
    return Clamp(value, clampMin, clampMax);
}

void Config::SetInt(const String& name, int value)
{
    XMLElement root = xmlFile_->GetRoot();
    root.SetInt(name, value);
}

void Config::SetRecord(String boardMode, int value)
{
    // Нет смысла хранить нулевые рекорды.
    if (value == 0)
        return;

    XMLElement table = xmlFile_->GetRoot().GetChild("Records");
    table.SetAttribute(boardMode, String(value));
}

int Config::GetRecord(String boardMode)
{
    XMLElement table = xmlFile_->GetRoot().GetChild("Records");
    return ToInt(table.GetAttribute(boardMode));
}
