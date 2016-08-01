// Юнит может находиться в двух состояниях:
// 1) Юниту назначена одна из клеток игрового поля. При этом юнит плавно движется
//    из текущего положения в свою клетку. Для апдейта используется функция Move.
// 2) Юнит не принадлежит ни одной из ячеек сетки и улетает с игрового поля.
//    Для апдейта используется функция Remove.
// В каком именно состоянии находится юнит, можно узнать по наличию
// или отсутствию тега Removed.

#pragma once
#include "Global.h"
#include "BoardLogic.h"

class UnitAnimator : public Component
{
    URHO3D_OBJECT(UnitAnimator, Component);

public:
    UnitAnimator(Context* context);
    static void RegisterObject(Context* context);

private:
    // Счетчик времени используется в функции Remove.
    float removeTimer_ = 0.0f;

    void Animate(StringHash eventType, VariantMap& eventData);
    void Move(float timeStep);
    void Remove(float timeStep);
};
