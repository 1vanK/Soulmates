#include "Utils.h"

float ToTarget(float currentValue, float targetValue, float speed, float timeStep)
{
    if (currentValue < targetValue)
    {
        float result = currentValue + speed * timeStep;
        
        // Проскочили целевое значение.
        if (result > targetValue)
            result = targetValue;

        return result;
    }
    else if (currentValue > targetValue)
    {
        float result = currentValue - speed * timeStep;

        if (result < targetValue)
            result = targetValue;

        return result;
    }
    else // currentValue == targetValue
    {
        return targetValue;
    }
}

Vector3 ToTarget(const Vector3& currentPosition, const Vector3& targetPosition, float speed, float timeStep)
{
    if (currentPosition.Equals(targetPosition))
        return targetPosition;

    // Кстати, таким же способом можно корректно анимировать цвет, если представить его как Vector4.
    Vector3 direction = (targetPosition - currentPosition).Normalized();
    Vector3 newPosition = currentPosition + direction * timeStep * speed;

    // Проверка перед попыткой определения нового направления, так как нормализация
    // вектора с нулевой длиной - плохая идея.
    if (newPosition.Equals(targetPosition))
        return targetPosition;

    // Если направление от новой позиции до целевой точки изменилось на противоположное,
    // значит мы проскочили пункт назначения.
    Vector3 newDirection = (targetPosition - newPosition).Normalized();
    if (newDirection.DotProduct(direction) < 0.0f)
        return targetPosition;
    
    return newPosition;
}

float DistanceSquared(const IntVector2& p1, const IntVector2& p2)
{
    Vector2 v1 = Vector2((float)p1.x_, (float)p1.y_);
    Vector2 v2 = Vector2((float)p2.x_, (float)p2.y_);
    return (v1 - v2).LengthSquared();
}
