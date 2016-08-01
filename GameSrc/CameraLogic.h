#pragma once
#include <Urho3D/Urho3DAll.h>

// Значение сигмы, при которой шейдер размытия отключается.
// Сигма не должна быть равна нулю, иначе произойдет деление
// на ноль и экран будет черным.
#define MIN_BLUR_SIGMA 0.3f

// Экспериментально определено, что изменения в силе размытия
// визуально заметны при значениях сигмы в диапазоне 0.3f - 1.5f.
#define MAX_BLUR_SIGMA 1.5f

// Этот компонент прикрепляется к ноде с камерой.
class CameraLogic : public LogicComponent
{
    URHO3D_OBJECT(CameraLogic, LogicComponent);

public:
    CameraLogic(Context* context);
    static void RegisterObject(Context* context);
    void Update(float timeStep);

private:
    // Плавное изменение силы размытия.
    void AnimateScreenBlur(float timeStep);
};
