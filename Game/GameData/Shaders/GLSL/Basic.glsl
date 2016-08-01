// Основан на Basic.glsl.

#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"

// Добавлено:
#include "ScreenPos.glsl"

#if defined(DIFFMAP) || defined(ALPHAMAP)
    varying vec2 vTexCoord;
#endif
#ifdef VERTEXCOLOR
    varying vec4 vColor;
#endif

// Добавлено:
varying vec2 vQuadPos;

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    
    #ifdef DIFFMAP
        vTexCoord = iTexCoord;
    #endif
    #ifdef VERTEXCOLOR
        vColor = iColor;
    #endif
    
    // Добавлено:
    vQuadPos = GetQuadTexCoord(gl_Position);
}

void PS()
{
    vec4 diffColor = cMatDiffColor;

    #ifdef VERTEXCOLOR
        diffColor *= vColor;
    #endif

    #if (!defined(DIFFMAP)) && (!defined(ALPHAMAP))
        gl_FragColor = diffColor;
    #endif
    #ifdef DIFFMAP
        vec4 diffInput = texture2D(sDiffMap, vTexCoord);
        #ifdef ALPHAMASK
            if (diffInput.a < 0.5)
                discard;
        #endif
        gl_FragColor = diffColor * diffInput;
    #endif
    #ifdef ALPHAMAP
        #ifdef GL3
            float alphaInput = texture2D(sDiffMap, vTexCoord).r;
        #else
            float alphaInput = texture2D(sDiffMap, vTexCoord).a;
        #endif
        gl_FragColor = vec4(diffColor.rgb, diffColor.a * alphaInput);
    #endif
    
    // Добавлено:
    
    // Время для фрагментов зависит от их координат. Получаем таким образом диагональные линии.
    float time = cElapsedTimePS + vQuadPos.x - vQuadPos.y;
    
    // Яркость изменяется по гармоническому закону с течением времени.
    float brightness = cos(time);
    
    // Приводим яркость из диапазона [-1.0, 1.0] к диапазону [0.0, 1.0].
    brightness = brightness * 0.5 + 0.5;
    
    // Сжимаем яркие полосы к их центрам. Это дает мягкие края.
    // По идее это быстрее, чем возведение в степень.
    brightness = smoothstep(0.999, 1.0, brightness);
    
    // Предыдущая операция возвращает результат в диапазоне [0.0 - 1.0]. Уменьшаем яркость.
    brightness *= 0.05;
    
    // Модифицируем цвет интерфейса.
    gl_FragColor.rgb = gl_FragColor.rgb + brightness;
}
