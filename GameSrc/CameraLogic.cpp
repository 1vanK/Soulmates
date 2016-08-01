#include "CameraLogic.h"
#include "Global.h"
#include "Urho3DAliases.h"
#include "Utils.h"

CameraLogic::CameraLogic(Context* context) :
    LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}

void CameraLogic::RegisterObject(Context* context)
{
    context->RegisterFactory<CameraLogic>();
}

void CameraLogic::Update(float timeStep)
{
    // Направление камеры зависит от положения курсора мыши.
    IntVector2 mousePos = INPUT->GetMousePosition();
    float yaw = (float)mousePos.x_ / GRAPHICS->GetWidth() - 0.5f;
    float pitch = (float)mousePos.y_ / GRAPHICS->GetHeight() - 0.5f;
    node_->SetRotation(Quaternion(pitch * 3.0f, yaw * 3.0f, 0.0f));

    // Дистанция камеры зависит от размера игрового поля.
    float distFromWidth = BOARD_LOGIC->width_ * 1.3f;
    float distFromHeight = BOARD_LOGIC->height_ * 1.6f;
    float targetZ = -max(distFromWidth, distFromHeight);
    float currentZ = node_->GetPosition().z_;
    float newZ = ToTarget(currentZ, targetZ, 10.0f, timeStep);
    node_->SetPosition(Vector3(0.0f, 0.0f, newZ));

    AnimateScreenBlur(timeStep);
}

void CameraLogic::AnimateScreenBlur(float timeStep)
{
    // В состоянии GS_GAMEPLAY размытия нет.
    float targetSigma = MIN_BLUR_SIGMA;

    if (GLOBAL->gameState_ == GS_START_MENU || GLOBAL->gameState_ == GS_GAME_OVER)
        targetSigma = MAX_BLUR_SIGMA;

    // Определяем текущую силу размытия.
    RenderPath* renderPath = RENDERER->GetViewport(0)->GetRenderPath();
    float currentSigma = renderPath->GetShaderParameter("BlurSigma").GetFloat();

    // Плавно меняем в сторону требуемой величины.
    float newSigma = ToTarget(currentSigma, targetSigma, 3.0f, timeStep);

    if (newSigma <= MIN_BLUR_SIGMA)
    {
        // Отключаем размытие, если оно достаточно мало.
        renderPath->SetEnabled("Blur", false);
        // Чтобы в следующий раз сигма стартовала с минимального значения.
        renderPath->SetShaderParameter("BlurSigma", MIN_BLUR_SIGMA);
    }
    else
    {
        renderPath->SetEnabled("Blur", true);
        renderPath->SetShaderParameter("BlurSigma", newSigma);
    }
}
