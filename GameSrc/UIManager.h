/*
Так как к UI-элементам нельзя прикрепить компоненты, то все элементы
контролируются в этой подсистеме.

Так как интерфейс один на всю игру, то необходимо управлять видимостью
элементов интерфейса, чтобы на экране были только UI-элементы,
соответствующие текущему игровому состоянию.

Игровые состояния, в которых должен отображаться элемент, указываются
с помощью тегов.

Есть встроенные элементы (UI-курсор, консоль, отладочный худ), которые
не нужно трогать, поэтому каждый управляемый элемент должен иметь тэг "Managed".

Если у элемента есть тэг "Visible", то он виден в любом состоянии.

Контролируются только дочерние элементы рута.
*/

#pragma once
#include "Global.h"
#include "MyButton.h"

#define UI_MANAGER GetSubsystem<UIManager>()

class UIManager : public Object
{
    URHO3D_OBJECT(UIManager, Object);

public:
    MyButton* musicButton_ = nullptr;
    MyButton* soundButton_ = nullptr;

    UIManager(Context* context);

    // Счет изменяется в Update, поэтому используем PostUpdate для
    // обновления соответствующего ему элемента интерфейса.
    // Заодно остальная UI-логика тоже тут.
    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);

    void HandleLangButtonClick(StringHash eventType, VariantMap& eventData);
    void HandleMusicButtonClick(StringHash eventType, VariantMap& eventData);
    void HandleSoundButtonClick(StringHash eventType, VariantMap& eventData);
    void HandleReturnButtonClick(StringHash eventType, VariantMap& eventData);
    void HandleWidthDecreaseClick(StringHash eventType, VariantMap& eventData);
    void HandleWidthIncreaseClick(StringHash eventType, VariantMap& eventData);
    void HandleHeightDecreaseClick(StringHash eventType, VariantMap& eventData);
    void HandleHeightIncreaseClick(StringHash eventType, VariantMap& eventData);
    void HandleNumColorsDecreaseClick(StringHash eventType, VariantMap& eventData);
    void HandleNumColorsIncreaseClick(StringHash eventType, VariantMap& eventData);
    void HandlePopulationDecreaseClick(StringHash eventType, VariantMap& eventData);
    void HandlePopulationIncreaseClick(StringHash eventType, VariantMap& eventData);
    void HandleLineLengthDecreaseClick(StringHash eventType, VariantMap& eventData);
    void HandleLineLengthIncreaseClick(StringHash eventType, VariantMap& eventData);
    void HandleDiagonalDecreaseClick(StringHash eventType, VariantMap& eventData);
    void HandleDiagonalIncreaseClick(StringHash eventType, VariantMap& eventData);
    void HandleStartClick(StringHash eventType, VariantMap& eventData);
    void HandleReplayButtonClick(StringHash eventType, VariantMap& eventData);

    // Оставляет на экране только те элементы, которые должны быть
    // видны в данном игровом состоянии.
    void UpdateUIVisibility();

    // Обновляет все надписи в стартовом меню в соответствии с текущим языком
    // и настройками игрового поля.
    void UpdateStartMenuTexts();

    // Возвращает элемент интерфейса под курсором мыши.
    UIElement* GetHoveredElement();

    // Отображаемый в текущий момент счет. Он стремится к реальному
    // счету с определенной скоростью.
    float showedScore_ = 0.0f;

private:
    void LoadStartMenuLayout();
    void LoadGameOverLayout();
    void PlayClick();
};
