#include "UIManager.h"
#include "BoardLogic.h"
#include "Urho3DAliases.h"
#include "Config.h"

UIManager::UIManager(Context* context) : Object(context)
{
    INPUT->SetMouseVisible(true);

    // Создаем отладочный худ.
    XMLFile* defaultStyle = GET_XML_FILE("UI/DefaultStyle.xml");
    DebugHud* debugHud = ENGINE->CreateDebugHud();
    debugHud->SetDefaultStyle(defaultStyle);

    // Остальные элементы используют кастомный стиль.
    XMLFile* style = GET_XML_FILE("UI/Style.xml");
    UI_ROOT->SetDefaultStyle(style);

    // Создаем текстовый элемент для отображения счета.
    Text* scoreText = UI_ROOT->CreateChild<Text>("Score");
    scoreText->SetStyle("ScoreText");

    // Создаем текстовый элемент для отображения рекорда.
    Text* recordText = UI_ROOT->CreateChild<Text>("Record");
    recordText->SetStyle("RecordText");

    // Создаем кнопку для смены языка.
    MyButton* langButton = UI_ROOT->CreateChild<MyButton>();
    langButton->SetStyle("LangButton");
    SubscribeToEvent(langButton, E_PRESSED, URHO3D_HANDLER(UIManager, HandleLangButtonClick));

    // Создаем кнопку возврата в стартовое меню.
    MyButton* returnButton = UI_ROOT->CreateChild<MyButton>();
    returnButton->SetStyle("ReturnButton");
    SubscribeToEvent(returnButton, E_PRESSED, URHO3D_HANDLER(UIManager, HandleReturnButtonClick));

    // Создаем кнопку для управления громкостью звуков.
    soundButton_ = UI_ROOT->CreateChild<MyButton>();
    soundButton_->SetStyle("SoundButton");
    SubscribeToEvent(soundButton_, E_PRESSED, URHO3D_HANDLER(UIManager, HandleSoundButtonClick));

    // Создаем кнопку для управления громкостью музыки.
    musicButton_ = UI_ROOT->CreateChild<MyButton>();
    musicButton_->SetStyle("MusicButton");
    SubscribeToEvent(musicButton_, E_PRESSED, URHO3D_HANDLER(UIManager, HandleMusicButtonClick));

    LoadStartMenuLayout();
    LoadGameOverLayout();

    UpdateUIVisibility();

    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(UIManager, HandlePostUpdate));
}

void UIManager::LoadGameOverLayout()
{
    // Стиль "UI/Style.xml" был выбран ранее.

    SharedPtr<UIElement> element = UI->LoadLayout(GET_XML_FILE("UI/GameOver.xml"));
    UI_ROOT->AddChild(element);

    UIElement* replayButton = element->GetChild("ReplayButton", false);
    SubscribeToEvent(replayButton, E_PRESSED, URHO3D_HANDLER(UIManager, HandleReplayButtonClick));
}

void UIManager::HandleReplayButtonClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();
    BOARD_LOGIC->CreateBoard();
    GLOBAL->neededGameState_ = GS_GAMEPLAY;
}

void UIManager::PlayClick()
{
    GLOBAL->PlaySound("Click", "Sounds/Click", 3);
}

void UIManager::LoadStartMenuLayout()
{
    // Стиль "UI/Style.xml" был выбран ранее.
    
    SharedPtr<UIElement> startMenu = UI->LoadLayout(GET_XML_FILE("UI/StartMenu.xml"));
    UI_ROOT->AddChild(startMenu);

    UIElement* button = startMenu->GetChild("WidthDecrease", false);
    SubscribeToEvent(button, E_PRESSED, URHO3D_HANDLER(UIManager, HandleWidthDecreaseClick));
    button = startMenu->GetChild("WidthIncrease", false);
    SubscribeToEvent(button, E_PRESSED, URHO3D_HANDLER(UIManager, HandleWidthIncreaseClick));

    button = startMenu->GetChild("HeightDecrease", false);
    SubscribeToEvent(button, E_PRESSED, URHO3D_HANDLER(UIManager, HandleHeightDecreaseClick));
    button = startMenu->GetChild("HeightIncrease", false);
    SubscribeToEvent(button, E_PRESSED, URHO3D_HANDLER(UIManager, HandleHeightIncreaseClick));

    button = startMenu->GetChild("NumColorsDecrease", false);
    SubscribeToEvent(button, E_PRESSED, URHO3D_HANDLER(UIManager, HandleNumColorsDecreaseClick));
    button = startMenu->GetChild("NumColorsIncrease", false);
    SubscribeToEvent(button, E_PRESSED, URHO3D_HANDLER(UIManager, HandleNumColorsIncreaseClick));

    button = startMenu->GetChild("PopulationDecrease", false);
    SubscribeToEvent(button, E_PRESSED, URHO3D_HANDLER(UIManager, HandlePopulationDecreaseClick));
    button = startMenu->GetChild("PopulationIncrease", false);
    SubscribeToEvent(button, E_PRESSED, URHO3D_HANDLER(UIManager, HandlePopulationIncreaseClick));

    button = startMenu->GetChild("LineLengthDecrease", false);
    SubscribeToEvent(button, E_PRESSED, URHO3D_HANDLER(UIManager, HandleLineLengthDecreaseClick));
    button = startMenu->GetChild("LineLengthIncrease", false);
    SubscribeToEvent(button, E_PRESSED, URHO3D_HANDLER(UIManager, HandleLineLengthIncreaseClick));

    button = startMenu->GetChild("DiagonalDecrease", false);
    SubscribeToEvent(button, E_PRESSED, URHO3D_HANDLER(UIManager, HandleDiagonalDecreaseClick));
    button = startMenu->GetChild("DiagonalIncrease", false);
    SubscribeToEvent(button, E_PRESSED, URHO3D_HANDLER(UIManager, HandleDiagonalIncreaseClick));

    button = startMenu->GetChild("Start", false);
    SubscribeToEvent(button, E_PRESSED, URHO3D_HANDLER(UIManager, HandleStartClick));
}

void UIManager::UpdateStartMenuTexts()
{
    UIElement* startMenu = UI_ROOT->GetChild("StartMenu", false);

    Text* widthText = static_cast<Text*>(startMenu->GetChild("WidthText", false));
    widthText->SetText(LOCALIZATION->Get("Width") + ": " + String(BOARD_LOGIC->width_));

    Text* heightText = static_cast<Text*>(startMenu->GetChild("HeightText", false));
    heightText->SetText(LOCALIZATION->Get("Height") + ": " + String(BOARD_LOGIC->height_));

    Text* numColorsText = static_cast<Text*>(startMenu->GetChild("NumColorsText", false));
    numColorsText->SetText(LOCALIZATION->Get("Num Colors") + ": " + String(BOARD_LOGIC->numColors_));

    Text* populationText = static_cast<Text*>(startMenu->GetChild("PopulationText", false));
    populationText->SetText(LOCALIZATION->Get("Population") + ": " + String(BOARD_LOGIC->initialPopulation_));

    Text* lineLengthText = static_cast<Text*>(startMenu->GetChild("LineLengthText", false));
    lineLengthText->SetText(LOCALIZATION->Get("Line Length") + ": " + String(BOARD_LOGIC->lineLength_));

    Text* diagonalText = static_cast<Text*>(startMenu->GetChild("DiagonalText", false));
    String diagonalStr = LOCALIZATION->Get("Diagonal") + ": ";
    if (BOARD_LOGIC->diagonal_)
        diagonalStr += LOCALIZATION->Get("ON");
    else
        diagonalStr += LOCALIZATION->Get("OFF");
    diagonalText->SetText(diagonalStr);
}

void UIManager::HandleLangButtonClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    int lang = LOCALIZATION->GetLanguageIndex() + 1;

    if (lang >= LOCALIZATION->GetNumLanguages())
        lang = 0;

    LOCALIZATION->SetLanguage(lang);
}

void UIManager::HandleMusicButtonClick(StringHash eventType, VariantMap& eventData)
{
    GLOBAL->musicVolume_++;
    
    if (GLOBAL->musicVolume_ > MAX_VOLUME)
        GLOBAL->musicVolume_ = 0;

    GLOBAL->ApplyMusicVolume();

    // Звук клика не проигрываем.
}

void UIManager::HandleSoundButtonClick(StringHash eventType, VariantMap& eventData)
{
    GLOBAL->soundVolume_++;
    if (GLOBAL->soundVolume_ > MAX_VOLUME)
        GLOBAL->soundVolume_ = 0;

    GLOBAL->ApplySoundVolume();

    // Проигрываем клик после изменения громкости.
    PlayClick();
}

void UIManager::HandleStartClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    GLOBAL->neededGameState_ = GS_GAMEPLAY;
}

void UIManager::HandleReturnButtonClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    GLOBAL->neededGameState_ = GS_START_MENU;

    BOARD_LOGIC->CreateBoard();
}

void UIManager::HandleWidthDecreaseClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    int newWidth = BOARD_LOGIC->width_ - 1;
    newWidth = Clamp(newWidth, MIN_BOARD_WIDTH, MAX_BOARD_WIDTH);
    
    if (newWidth != BOARD_LOGIC->width_)
    {
        BOARD_LOGIC->width_ = newWidth;
        BOARD_LOGIC->ClampPopulationAndLineLength();
        BOARD_LOGIC->CreateBoard();
    }
}

void UIManager::HandleWidthIncreaseClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    int newWidth = BOARD_LOGIC->width_ + 1;
    newWidth = Clamp(newWidth, MIN_BOARD_WIDTH, MAX_BOARD_WIDTH);

    if (newWidth != BOARD_LOGIC->width_)
    {
        BOARD_LOGIC->width_ = newWidth;
        BOARD_LOGIC->ClampPopulationAndLineLength();
        BOARD_LOGIC->CreateBoard();
    }
}

void UIManager::HandleHeightDecreaseClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    int newHeight = BOARD_LOGIC->height_ - 1;
    newHeight = Clamp(newHeight, MIN_BOARD_HEIGHT, MAX_BOARD_HEIGHT);

    if (newHeight != BOARD_LOGIC->height_)
    {
        BOARD_LOGIC->height_ = newHeight;
        BOARD_LOGIC->ClampPopulationAndLineLength();
        BOARD_LOGIC->CreateBoard();
    }
}

void UIManager::HandleHeightIncreaseClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    int newHeight = BOARD_LOGIC->height_ + 1;
    newHeight = Clamp(newHeight, MIN_BOARD_HEIGHT, MAX_BOARD_HEIGHT);

    if (newHeight != BOARD_LOGIC->height_)
    {
        BOARD_LOGIC->height_ = newHeight;
        BOARD_LOGIC->ClampPopulationAndLineLength();
        BOARD_LOGIC->CreateBoard();
    }
}

void UIManager::HandleNumColorsDecreaseClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    int newNumColors = BOARD_LOGIC->numColors_ - 1;
    newNumColors = Clamp(newNumColors, MIN_NUM_COLORS, MAX_NUM_COLORS);

    if (newNumColors != BOARD_LOGIC->numColors_)
    {
        BOARD_LOGIC->numColors_ = newNumColors;
        BOARD_LOGIC->CreateBoard();
    }
}

void UIManager::HandleNumColorsIncreaseClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    int newNumColors = BOARD_LOGIC->numColors_ + 1;
    newNumColors = Clamp(newNumColors, MIN_NUM_COLORS, MAX_NUM_COLORS);

    if (newNumColors != BOARD_LOGIC->numColors_)
    {
        BOARD_LOGIC->numColors_ = newNumColors;
        BOARD_LOGIC->CreateBoard();
    }
}

void UIManager::HandlePopulationDecreaseClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    int newPopulation = BOARD_LOGIC->initialPopulation_ - 1;
    newPopulation = Clamp(newPopulation, 0, BOARD_LOGIC->GetMaxInitialPopulation());

    if (newPopulation != BOARD_LOGIC->initialPopulation_)
    {
        BOARD_LOGIC->initialPopulation_ = newPopulation;
        BOARD_LOGIC->CreateBoard();
    }
}

void UIManager::HandlePopulationIncreaseClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    int newPopulation = BOARD_LOGIC->initialPopulation_ + 1;
    newPopulation = Clamp(newPopulation, 0, BOARD_LOGIC->GetMaxInitialPopulation());

    if (newPopulation != BOARD_LOGIC->initialPopulation_)
    {
        BOARD_LOGIC->initialPopulation_ = newPopulation;
        BOARD_LOGIC->CreateBoard();
    }
}

void UIManager::HandleLineLengthDecreaseClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    int newLineLength = BOARD_LOGIC->lineLength_ - 1;
    newLineLength = Clamp(newLineLength, MIN_LINE_LENGTH, BOARD_LOGIC->GetMaxLineLength());

    if (newLineLength != BOARD_LOGIC->lineLength_)
    {
        BOARD_LOGIC->lineLength_ = newLineLength;
        BOARD_LOGIC->CreateBoard();
    }
}

void UIManager::HandleLineLengthIncreaseClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    int newLineLength = BOARD_LOGIC->lineLength_ + 1;
    newLineLength = Clamp(newLineLength, MIN_LINE_LENGTH, BOARD_LOGIC->GetMaxLineLength());

    if (newLineLength != BOARD_LOGIC->lineLength_)
    {
        BOARD_LOGIC->lineLength_ = newLineLength;
        BOARD_LOGIC->CreateBoard();
    }
}

void UIManager::HandleDiagonalDecreaseClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    BOARD_LOGIC->diagonal_ = !BOARD_LOGIC->diagonal_;
    BOARD_LOGIC->CreateBoard();
}

void UIManager::HandleDiagonalIncreaseClick(StringHash eventType, VariantMap& eventData)
{
    PlayClick();

    BOARD_LOGIC->diagonal_ = !BOARD_LOGIC->diagonal_;
    BOARD_LOGIC->CreateBoard();
}

void UIManager::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
    float timeStep = eventData[PostUpdate::P_TIMESTEP].GetFloat();

    // Если отображаемый счет меньше реального счета, то плавно наращиваем его.
    if (showedScore_ < BOARD_LOGIC->score_)
    {
        showedScore_ += timeStep * 10.0f;
        showedScore_ = Clamp(showedScore_, 0.0f, (float)BOARD_LOGIC->score_);
    }

    String scoreStr = LOCALIZATION->Get("Score") + ": ";
    scoreStr += (int)showedScore_;
    Text* scoreText = static_cast<Text*>(UI_ROOT->GetChild("Score", false));
    scoreText->SetText(scoreStr);

    String recordStr = LOCALIZATION->Get("Record") + ": ";
    recordStr += CONFIG->GetRecord(BOARD_LOGIC->BoardModeToString());
    Text* recordText = static_cast<Text*>(UI_ROOT->GetChild("Record", false));
    recordText->SetText(recordStr);

    UpdateStartMenuTexts();

    if (INPUT->GetKeyPress(KEY_F2))
        DEBUG_HUD->ToggleAll();
}

void UIManager::UpdateUIVisibility()
{
    String gameStateStr = GLOBAL->GameStateToString();
    
    PODVector<UIElement*> managedElements;
    UI_ROOT->GetChildrenWithTag(managedElements, "Managed");
    
    foreach(UIElement* element, managedElements)
    {
        if (element->HasTag("Visible"))
        {
            element->SetVisible(true);
            continue;
        }

        element->SetVisible(element->HasTag(gameStateStr));
    }
}

UIElement* UIManager::GetHoveredElement()
{
    return UI->GetElementAt(INPUT->GetMousePosition());
}
