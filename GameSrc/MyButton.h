/*
Основано на Button.h.
Отличие от оригинала в том, что вместо разных изображений для разных
состояний кнопки используется плавное изменение цвета. По этой причине
текстура кнопки должна быть в серых тонах.

Другие отличия от оригинала:
1) Класс выведен из пространства имен Urho3D.
2) Удалены оригинальные комментарии.

Эту анимацию можно сделать без создания отдельного класса:
https://github.com/urho3d/Urho3D/issues/1453.
Но при этом будут заняты события, что не очень удобно, так как
они могут потребоваться для других целей (в Urho3D новый обработчик
события перезаписывает старый).
*/

#pragma once
#include <Urho3D/Urho3DAll.h>

class URHO3D_API MyButton : public BorderImage
{
    URHO3D_OBJECT(MyButton, BorderImage);

public:
    MyButton(Context* context);
    virtual ~MyButton();
    static void RegisterObject(Context* context);

    virtual void Update(float timeStep);
    virtual void OnClickBegin
        (const IntVector2& position, const IntVector2& screenPosition, int button, int buttons, int qualifiers, Cursor* cursor);
    virtual void OnClickEnd
        (const IntVector2& position, const IntVector2& screenPosition, int button, int buttons, int qualifiers, Cursor* cursor,
            UIElement* beginElement);
    virtual void OnDragMove
        (const IntVector2& position, const IntVector2& screenPosition, const IntVector2& deltaPos, int buttons, int qualifiers,
            Cursor* cursor);
    virtual void OnKey(int key, int buttons, int qualifiers);
    virtual void GetBatches(PODVector<UIBatch>& batches, PODVector<float>& vertexData, const IntRect& currentScissor);

    void SetPressedChildOffset(const IntVector2& offset);
    void SetPressedChildOffset(int x, int y);
    void SetRepeat(float delay, float rate);
    void SetRepeatDelay(float delay);
    void SetRepeatRate(float rate);

    const IntVector2& GetPressedChildOffset() const { return pressedChildOffset_; }
    float GetRepeatDelay() const { return repeatDelay_; }
    float GetRepeatRate() const { return repeatRate_; }
    bool IsPressed() const { return pressed_; }

    // Добавленные методы.
    void SetPressedColor(const Color& color);
    const Color& GetPressedColor() const { return pressedColor_; }
    void SetHoverColor(const Color& color);
    const Color& GetHoverColor() const { return hoverColor_; }
    void SetNormalColor(const Color& color);
    const Color& GetNormalColor() const { return normalColor_; }

protected:
    void SetPressed(bool enable);

    IntVector2 pressedChildOffset_;
    float repeatDelay_;
    float repeatRate_;
    float repeatTimer_;
    bool pressed_;

    // Добавленные члены.
    Color pressedColor_;
    Color hoverColor_;
    Color normalColor_;

    // Актуальное состояние IsHovering и IsPressed гарантированно доступно
    // только в функции Update (см https://github.com/urho3d/Urho3D/issues/1453).
    // Поэтому вводим переменные для детектирования смены состояния.
    bool oldHover_;
    bool oldPressed_;
};
