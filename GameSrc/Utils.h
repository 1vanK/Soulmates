#pragma once
#include <Urho3D/Urho3DAll.h>

// Плавное изменение числа в сторону целевого значения с определенной скоростью.
float ToTarget(float currentValue, float targetValue, float speed, float timeStep);

// Плавное изменение позиции в сторону пункта назначения с определенной скоростью.
Vector3 ToTarget(const Vector3& currentPosition, const Vector3& targetPosition, float speed, float timeStep);

// Квадрат расстояния между двумя точками на экране.
float DistanceSquared(const IntVector2& p1, const IntVector2& p2);
