#include "UnitAnimator.h"
#include "Utils.h"

static const float UNIT_MOVE_SPEED = 20.0f;
static const float UNIT_SCALE_SPEED = 2.0f;
static const float UNIT_ROTATE_SPEED = 90.0f;

UnitAnimator::UnitAnimator(Context* context) : Component(context)
{
    SubscribeToEvent(E_ANIMATEUNIT, URHO3D_HANDLER(UnitAnimator, Animate));
}

void UnitAnimator::RegisterObject(Context* context)
{
    context->RegisterFactory<UnitAnimator>();
}

void UnitAnimator::Animate(StringHash eventType, VariantMap& eventData)
{
    float timeStep = eventData[AnimateUnit::P_TIMESTEP].GetFloat();

    if (node_->HasTag("Removed"))
        Remove(timeStep);
    else
        Move(timeStep);
}

void UnitAnimator::Move(float timeStep)
{
    // Индексы ячейки, в которой должен находиться юнит.
    // Юнит будет плавно двигаться в эту ячейку из текущего положения.
    int gridX = node_->GetVar("GridX").GetInt();
    int gridY = node_->GetVar("GridY").GetInt();
    
    // Позиция, к которой стремится юнит.
    Vector3 targetPos = BOARD_LOGIC->GetCellPos(gridX, gridY);

    Vector3 currentPos = node_->GetPosition();

    if (!currentPos.Equals(targetPos))
    {
        Vector3 newPos = ToTarget(currentPos, targetPos, UNIT_MOVE_SPEED, timeStep);
        node_->SetPosition(newPos);
        
        // В данной итерации игрового цикла пользователь не сможет кликать по юнитам.
        BOARD_LOGIC->needBreakUpdate_ = true;
    }

    // Масштабируем юнит до единицы, если нужно.
    float currentScale = node_->GetScale().x_;

    if (!Equals(currentScale, 1.0f))
    {
        float newScale = ToTarget(currentScale, 1.0f, UNIT_SCALE_SPEED, timeStep);
        node_->SetScale(newScale);

        // В данной итерации игрового цикла пользователь не сможет кликать по юнитам.
        BOARD_LOGIC->needBreakUpdate_ = true;
    }
}

void UnitAnimator::Remove(float timeStep)
{
    // Так как воспроизводится анимация удаления юнита, то в данной итерации
    // игрового цикла пользователь не сможет кликать по юнитам.
    BOARD_LOGIC->needBreakUpdate_ = true;

    // Юнит поврочачивается вокруг оси и только потом начинает улетать.
    removeTimer_ += timeStep;
    // Поворот на 180 градусов за первые пол секунды.
    if (removeTimer_ < 0.5f)
    {
        Quaternion startRot = Quaternion(0.0f, 180.0f, 0.0f);
        Quaternion endRot = Quaternion(0.0f, 0.0f, 0.0f);
        Quaternion rot = startRot.Slerp(endRot, removeTimer_ * 2.0f);
        node_->SetRotation(rot);
        return;
    }
    // Поворот еще на 180 градусов за другие пол секунды.
    if (removeTimer_ < 1.0f)
    {
        Quaternion startRot = Quaternion(0.0f, 0.0f, 0.0f);
        Quaternion endRot = Quaternion(0.0f, -180.0f, 0.0f);
        Quaternion rot = startRot.Slerp(endRot, (removeTimer_ - 0.5f) * 2.0f);
        node_->SetRotation(rot);
        return;
    }

    node_->Translate(Vector3::FORWARD * timeStep * UNIT_MOVE_SPEED);

    // Если юнит вылетел за пределы экрана, то удаляем ноду.
    StaticModel* staticModel = node_->GetComponent<StaticModel>();
    if (!staticModel->IsInView())
        node_->Remove();
}
