// RimLight-шейдер. Без текстур. Без тумана. Без теней. Только Forward.
// Только base проход. Источники света не учитываются. Фоновое освещение зоны не учитывается.
// Материал должен определить значение MatDiffColor.

#include "Uniforms.glsl"
#include "Transform.glsl"

varying vec3 vNormal;
varying vec4 vWorldPos;

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vNormal = GetWorldNormal(modelMatrix);
    vWorldPos = vec4(worldPos, GetDepth(gl_Position));
}

void PS()
{
    vec3 viewDir = normalize(cCameraPosPS - vWorldPos.xyz);
    
    float rim = 1.0 - dot(vNormal, viewDir);
    rim = clamp(rim, 0.0, 1.0);

    vec3 finalColor = cMatDiffColor.rgb * (1.0 - rim) + rim;

    gl_FragColor = vec4(finalColor, 1.0);
}
