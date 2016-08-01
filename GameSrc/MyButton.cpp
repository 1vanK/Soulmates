/*
Основано на Button.cpp.
*/

#include "MyButton.h"

const char* UI_CATEGORY = "UI";

// Цвета по умолчанию.
#define BUTTON_NORMAL_COLOR Color(0.5f, 0.0f, 0.0f)
#define BUTTON_HOVER_COLOR Color::RED
#define BUTTON_PRESSED_COLOR Color::YELLOW

MyButton::MyButton(Context* context) :
    BorderImage(context),
    pressedChildOffset_(IntVector2::ZERO),
    repeatDelay_(1.0f),
    repeatRate_(0.0f),
    repeatTimer_(0.0f),
    pressed_(false),
    // Добавлено.
    pressedColor_(BUTTON_PRESSED_COLOR),
    hoverColor_(BUTTON_HOVER_COLOR),
    normalColor_(BUTTON_NORMAL_COLOR),
    oldHover_(false),
    oldPressed_(false)
{
    SetEnabled(true);
    focusMode_ = FM_FOCUSABLE;

    // Добавлено.
    SetColor(normalColor_);
}

void MyButton::SetNormalColor(const Color& color)
{
    normalColor_ = color;

    // Мгновенная смена цвета. Иначе при загрузке цвет кнопки будет дефолтным тёмно-красным,
    // и только потом плавно изменится до заданного в xml-файле значения "Normal Color".
    if (!pressed_ && !hovering_)
    {
        SetColor(normalColor_);
        RemoveAttributeAnimation("Color");
    }
}

void MyButton::SetPressedColor(const Color& color)
{
    pressedColor_ = color;

    if (pressed_)
    {
        SetColor(pressedColor_);
        RemoveAttributeAnimation("Color");
    }
}

void MyButton::SetHoverColor(const Color& color)
{
    hoverColor_ = color;

    if (!pressed_ && hovering_)
    {
        SetColor(hoverColor_);
        RemoveAttributeAnimation("Color");
    }
}

void MyButton::RegisterObject(Context* context)
{
    context->RegisterFactory<MyButton>(UI_CATEGORY);

    URHO3D_COPY_BASE_ATTRIBUTES(BorderImage);
    URHO3D_UPDATE_ATTRIBUTE_DEFAULT_VALUE("Is Enabled", true);
    URHO3D_UPDATE_ATTRIBUTE_DEFAULT_VALUE("Focus Mode", FM_FOCUSABLE);
    URHO3D_ACCESSOR_ATTRIBUTE("Pressed Child Offset", GetPressedChildOffset, SetPressedChildOffset, IntVector2, IntVector2::ZERO, AM_FILE);
    URHO3D_ACCESSOR_ATTRIBUTE("Repeat Delay", GetRepeatDelay, SetRepeatDelay, float, 1.0f, AM_FILE);
    URHO3D_ACCESSOR_ATTRIBUTE("Repeat Rate", GetRepeatRate, SetRepeatRate, float, 0.0f, AM_FILE);

    // Добавлено.
    URHO3D_ACCESSOR_ATTRIBUTE("Hover Color", GetHoverColor, SetHoverColor, Color, BUTTON_HOVER_COLOR, AM_FILE);
    URHO3D_ACCESSOR_ATTRIBUTE("Pressed Color", GetPressedColor, SetPressedColor, Color, BUTTON_PRESSED_COLOR, AM_FILE);
    URHO3D_ACCESSOR_ATTRIBUTE("Normal Color", GetNormalColor, SetNormalColor, Color, BUTTON_NORMAL_COLOR, AM_FILE);
}

void MyButton::GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor)
{
    // Смещение всегда нулевое, свойство hoverOffset базового класса игнорируется.
    BorderImage::GetBatches(batches, vertexData, currentScissor, IntVector2::ZERO);
}

void MyButton::Update(float timeStep)
{
    if (!hovering_ && pressed_)
        SetPressed(false);

    // Зафиксирована смена состояния, поэтому запускаем анимацию цвета.
    if (oldPressed_ != pressed_ || oldHover_ != hovering_)
    {
        oldPressed_ = pressed_;
        oldHover_ = hovering_;

        Color currentColor = GetColor(Corner::C_TOPLEFT);
        Color targetColor = currentColor;

        if (pressed_)
            targetColor = pressedColor_;
        else if (hovering_)
            targetColor = hoverColor_;
        else
            targetColor = normalColor_;

        if (currentColor != targetColor)
        {
            SharedPtr<ValueAnimation> colorAnimation(new ValueAnimation(context_));
            colorAnimation->SetKeyFrame(0.0f, currentColor);
            colorAnimation->SetKeyFrame(0.2f, targetColor);
            SetAttributeAnimation("Color", colorAnimation, WM_ONCE);
        }
        else
        {
            // Теоретически возможно, что текущий и целевой цвет уже совпадают,
            // но запущена анимация в сторону какого-то другого цвета.
            RemoveAttributeAnimation("Color");
        }
    }

    if (pressed_ && repeatRate_ > 0.0f)
    {
        repeatTimer_ -= timeStep;
        if (repeatTimer_ <= 0.0f)
        {
            repeatTimer_ += 1.0f / repeatRate_;

            using namespace Pressed;

            VariantMap& eventData = GetEventDataMap();
            eventData[P_ELEMENT] = this;
            SendEvent(E_PRESSED, eventData);
        }
    }
}

// То, что ниже, не тронуто (за исключением имени класса).

MyButton::~MyButton()
{
}

void MyButton::OnClickBegin(const IntVector2& position, const IntVector2& screenPosition, int button, int buttons, int qualifiers,
    Cursor* cursor)
{
    if (button == MOUSEB_LEFT)
    {
        SetPressed(true);
        repeatTimer_ = repeatDelay_;
        hovering_ = true;

        using namespace Pressed;

        VariantMap& eventData = GetEventDataMap();
        eventData[P_ELEMENT] = this;
        SendEvent(E_PRESSED, eventData);
    }
}

void MyButton::OnClickEnd(const IntVector2& position, const IntVector2& screenPosition, int button, int buttons, int qualifiers,
    Cursor* cursor, UIElement* beginElement)
{
    if (pressed_ && button == MOUSEB_LEFT)
    {
        SetPressed(false);

        using namespace Released;

        VariantMap& eventData = GetEventDataMap();
        eventData[P_ELEMENT] = this;
        SendEvent(E_RELEASED, eventData);
    }
}

void MyButton::OnDragMove(const IntVector2& position, const IntVector2& screenPosition, const IntVector2& deltaPos, int buttons,
    int qualifiers, Cursor* cursor)
{
    SetPressed(true);
}

void MyButton::OnKey(int key, int buttons, int qualifiers)
{
    if (HasFocus() && (key == KEY_RETURN || key == KEY_RETURN2 || key == KEY_KP_ENTER || key == KEY_SPACE))
    {
        OnClickBegin(IntVector2(), IntVector2(), MOUSEB_LEFT, 0, 0, 0);
        OnClickEnd(IntVector2(), IntVector2(), MOUSEB_LEFT, 0, 0, 0, 0);
    }
}

void MyButton::SetPressedChildOffset(const IntVector2& offset)
{
    pressedChildOffset_ = offset;
}

void MyButton::SetPressedChildOffset(int x, int y)
{
    pressedChildOffset_ = IntVector2(x, y);
}

void MyButton::SetRepeat(float delay, float rate)
{
    SetRepeatDelay(delay);
    SetRepeatRate(rate);
}

void MyButton::SetRepeatDelay(float delay)
{
    repeatDelay_ = Max(delay, 0.0f);
}

void MyButton::SetRepeatRate(float rate)
{
    repeatRate_ = Max(rate, 0.0f);
}

void MyButton::SetPressed(bool enable)
{
    pressed_ = enable;
    SetChildOffset(pressed_ ? pressedChildOffset_ : IntVector2::ZERO);
}
