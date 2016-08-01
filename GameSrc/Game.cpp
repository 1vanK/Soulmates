/*
Soulmates (Родственные души).
Главный класс игры.
*/

#include "Global.h"
#include "BoardLogic.h"
#include "UnitAnimator.h"
#include "UIManager.h"
#include "MyButton.h"
#include "Config.h"
#include "Urho3DAliases.h"
#include "CameraLogic.h"


class Game : public Application
{
    URHO3D_OBJECT(Game, Application);

public:
    Game(Context* context) : Application(context)
    {
        // Самый первый обработчик для самого первого события. Используется для смены состояния
        // игры в начале итерации игрового цикла перед возникновением других событий.
        SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(Game, ApplyGameState));

        BoardLogic::RegisterObject(context_);
        UnitAnimator::RegisterObject(context_);
        MyButton::RegisterObject(context_);
        CameraLogic::RegisterObject(context_);
    }

    void Setup()
    {
        engineParameters_["WindowTitle"] = "Soulmates";
        engineParameters_["LogName"] = FILE_SYSTEM->GetAppPreferencesDir("1vanK", "Soulmates") + "Log.log";
        engineParameters_["FullScreen"] = false;
        engineParameters_["WindowWidth"] = 800;
        engineParameters_["WindowHeight"] = 600;
        // Собственная папка с ресурсами указывается перед стандартными.
        // Таким образом можно подсунуть движку свои ресурсы вместо стандартных.
        engineParameters_["ResourcePaths"] = "GameData;Data;CoreData";
    }

    void Start()
    {
        // Каждая игра будет уникальной.
        SetRandomSeed(Time::GetSystemTime());
        // Блокируем Alt+Enter.
        INPUT->SetToggleFullscreen(false);
        // Ограничиваем ФПС, чтобы снизить нагрузку на систему.
        ENGINE->SetMaxFps(60);

        context_->RegisterSubsystem(new Config(context_));
        CONFIG->Load();

        LOCALIZATION->LoadJSONFile("Strings.json");
        LOCALIZATION->SetLanguage(CONFIG->GetInt("Language", 0));

        // Создаем собственные подсистемы после инициализации встроенных,
        // так как они могут обращаться к встроенным в своих конструкторах.
        context_->RegisterSubsystem(new Global(context_));
        context_->RegisterSubsystem(new UIManager(context_));

        // Получаем громкость музыки и звуков из конфига.
        GLOBAL->musicVolume_ = CONFIG->GetInt("MusicVolume", DEFAULT_VOLUME, 0, MAX_VOLUME);
        GLOBAL->soundVolume_ = CONFIG->GetInt("SoundVolume", DEFAULT_VOLUME, 0, MAX_VOLUME);
        // Эти функции обращаются к интерфейсу, поэтому инициализация
        // подсистемы Global производится после регистрации UIManager.
        GLOBAL->ApplyMusicVolume();
        GLOBAL->ApplySoundVolume();

        // Запускаем зацикленное проигрывание фоновой музыки.
        GLOBAL->PlayMusic("Music/Music.ogg");

        CreateScene();
        SetupViewport();
    }

    void ApplyGameState(StringHash eventType, VariantMap& eventData)
    {
        if (GLOBAL->gameState_ == GLOBAL->neededGameState_)
            return;

        if (GLOBAL->neededGameState_ == GS_GAME_OVER)
            GLOBAL->PlaySound("Sounds/GameOver.wav");

        GLOBAL->gameState_ = GLOBAL->neededGameState_;
        UI_MANAGER->UpdateUIVisibility();
    }

    // В сцене нет источника света, зона тоже не нужна.
    void CreateScene()
    {
        Scene* scene = GLOBAL->scene_ = new Scene(context_);
        scene->CreateComponent<Octree>();

        Node* cameraNode = scene->CreateChild("Camera");
        cameraNode->SetPosition(Vector3(0.0f, 0.0f, -20.0f));
        cameraNode->CreateComponent<Camera>();
        cameraNode->CreateComponent<CameraLogic>();

        // Фоновую плоскость прикрепляем к камере.
        Node* skyNode = cameraNode->CreateChild();
        skyNode->SetPosition(Vector3(0.0f, 0.0f, 20.0f));
        skyNode->SetScale(Vector3(40.0f, 0.0f, 30.0f));
        skyNode->SetRotation(Quaternion(-90.0f, 0.0f, 0.0f));
        StaticModel* skyObject = skyNode->CreateComponent<StaticModel>();
        skyObject->SetModel(GET_MODEL("Models/Plane.mdl"));
        skyObject->SetMaterial(GET_MATERIAL("Materials/Sky.xml"));

        // Генератор звездочек.
        Node* particleNode = scene->CreateChild();
        ParticleEmitter* particleEmitter = particleNode->CreateComponent<ParticleEmitter>();
        particleEmitter->SetEffect(GET_PARTICLE_EFFECT("Particle/Fireflies.xml"));

        // Создаем игровое поле.
        GLOBAL->boardNode_ = scene->CreateChild();
        BoardLogic* boardLogic = GLOBAL->boardNode_->CreateComponent<BoardLogic>();
        boardLogic->width_ = CONFIG->GetInt("Width", DEFAULT_BOARD_WIDTH, 0, MAX_BOARD_WIDTH);
        boardLogic->height_ = CONFIG->GetInt("Height", DEFAULT_BOARD_HEIGHT, 0, MAX_BOARD_HEIGHT);
        boardLogic->numColors_ = CONFIG->GetInt("NumColors", DEFAULT_NUM_COLORS,
            MIN_NUM_COLORS, MAX_NUM_COLORS);
        boardLogic->initialPopulation_ = CONFIG->GetInt("Population", DEFAULT_POPULATION,
            0, boardLogic->GetMaxInitialPopulation());
        boardLogic->lineLength_ = CONFIG->GetInt("LineLength", DEFAULT_LINE_LENGTH,
            MIN_LINE_LENGTH, boardLogic->GetMaxLineLength());
        boardLogic->diagonal_ = (CONFIG->GetInt("Diagonal", (int)DEFAULT_DIAGONAL) != 0);
        boardLogic->CreateBoard();
    }

    // Показывает сцену на экране.
    void SetupViewport()
    {
        RENDERER->SetDefaultRenderPath(GET_XML_FILE("RenderPaths/MyForward.xml"));

        // Вьюпорт при создании использует дефолтный рендерпас, указанный выше.
        Camera* camera = GLOBAL->scene_->GetChild("Camera")->GetComponent<Camera>();
        SharedPtr<Viewport> viewport(new Viewport(context_, GLOBAL->scene_, camera));

        // Добавляем сглаживание.
        viewport->GetRenderPath()->Append(GET_XML_FILE("PostProcess/FXAA3.xml"));

        // Так как игра стартует в состоянии GS_START_MENU, то включаем размытие сразу.
        viewport->GetRenderPath()->Append(GET_XML_FILE("PostProcess/MyBlur.xml"));
        viewport->GetRenderPath()->SetShaderParameter("BlurSigma", MAX_BLUR_SIGMA);

        RENDERER->SetViewport(0, viewport);
    }

    void Stop()
    {
        // Сохраняем настройки при выходе из игры.
        CONFIG->SetInt("Language", LOCALIZATION->GetLanguageIndex());
        CONFIG->SetInt("MusicVolume", GLOBAL->musicVolume_);
        CONFIG->SetInt("SoundVolume", GLOBAL->soundVolume_);
        CONFIG->SetInt("Width", BOARD_LOGIC->width_);
        CONFIG->SetInt("Height", BOARD_LOGIC->height_);
        CONFIG->SetInt("NumColors", BOARD_LOGIC->numColors_);
        CONFIG->SetInt("Population", BOARD_LOGIC->initialPopulation_);
        CONFIG->SetInt("LineLength", BOARD_LOGIC->lineLength_);
        CONFIG->SetInt("Diagonal", (int)BOARD_LOGIC->diagonal_);
        CONFIG->Save();
    }
};

URHO3D_DEFINE_APPLICATION_MAIN(Game)
