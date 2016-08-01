/*
Краткие имена для подсистем движка.
*/

#pragma once

#define CACHE GetSubsystem<ResourceCache>()
#define RENDERER GetSubsystem<Renderer>()
#define INPUT GetSubsystem<Input>()
#define DEBUG_HUD GetSubsystem<DebugHud>()
#define FILE_SYSTEM GetSubsystem<FileSystem>()
#define UI GetSubsystem<UI>()
#define UI_ROOT UI->GetRoot()
#define GRAPHICS GetSubsystem<Graphics>()
#define ENGINE GetSubsystem<Engine>()
#define LOCALIZATION GetSubsystem<Localization>()
#define AUDIO GetSubsystem<Audio>()

#define GET_MATERIAL CACHE->GetResource<Material>
#define GET_MODEL CACHE->GetResource<Model>
#define GET_TEXTURE_2D CACHE->GetResource<Texture2D>
#define GET_SOUND CACHE->GetResource<Sound>
#define GET_FONT CACHE->GetResource<Font>
#define GET_XML_FILE CACHE->GetResource<XMLFile>
#define GET_JSON_FILE CACHE->GetResource<JSONFile>
#define GET_PARTICLE_EFFECT CACHE->GetResource<ParticleEffect>
#define GET_PARTICLE_EFFECT_2D CACHE->GetResource<ParticleEffect2D>
