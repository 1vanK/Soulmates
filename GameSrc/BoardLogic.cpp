#include "BoardLogic.h"
#include "UnitAnimator.h"
#include "Urho3DAliases.h"
#include "Config.h"
#include "UIManager.h"
#include "Utils.h"

static const Color colors[MAX_NUM_COLORS]
{
    Color(1.0f, 0.0f, 0.0f) * 0.75f, // 0 Красный
    Color(1.0f, 0.5f, 0.0f) * 0.75f, // 1 Оранжевый
    Color(1.0f, 1.0f, 0.0f) * 0.75f, // 2 Желтый
    Color(0.0f, 1.0f, 0.0f) * 0.75f, // 3 Зеленый
    Color(0.0f, 1.0f, 1.0f) * 0.75f, // 4 Голубой
    Color(0.0f, 0.0f, 1.0f) * 0.75f, // 5 Синий
    Color(1.0f, 0.0f, 1.0f) * 0.75f  // 6 Фиолетовый
};

BoardLogic::BoardLogic(Context* context) :
    Component(context)
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(BoardLogic, HandleUpdate));
}

void BoardLogic::RegisterObject(Context* context)
{
    context->RegisterFactory<BoardLogic>();
}

void BoardLogic::CreateBoard()
{
    // Очищаем поле на случай, если оно пересоздается.
    node_->RemoveAllChildren();
    score_ = 0;
    UI_MANAGER->showedScore_ = 0.0f;
    selectedUnit_ = nullptr;

    grid_.Resize(width_ * height_);

    // Населяем края доски.
    for (int gridX = 0; gridX < width_; gridX++)
    {
        // Верхняя строка.
        CreateUnit(gridX, 0);
        // Нижняя строка.
        CreateUnit(gridX, height_ - 1);
    }

    // Последний столбец (углы уже заселены).
    for (int gridY = 1; gridY < height_ - 1; gridY++)
        CreateUnit(width_ - 1, gridY);

    PODVector<IntVector2> emptyCells;
    emptyCells.Reserve((height_ - 2) * (width_ - 1));

    // Создаем список пустых клеток.
    for (int gridX = 0; gridX < width_ - 1; gridX++)
    {
        for (int gridY = 1; gridY < height_ - 1; gridY++)
            emptyCells.Push(IntVector2(gridX, gridY));
    }

    for (int i = 0; i < initialPopulation_; i++)
    {
        int index = Random((int)emptyCells.Size());
        CreateUnit(emptyCells[index].x_, emptyCells[index].y_);
        emptyCells.Erase(index, 1);
    }
}

// Просто назначаем юниту другую клетку доски и он сам будет туда плавно перемещаться.
void BoardLogic::MoveUnit(Node* node, int gridX, int gridY)
{
    int oldGridX = node->GetVar("GridX").GetInt();
    int oldGridY = node->GetVar("GridY").GetInt();
    grid_[oldGridY * width_ + oldGridX] = nullptr;

    node->SetVar("GridX", gridX);
    node->SetVar("GridY", gridY);
    grid_[gridY * width_ + gridX] = node;

    GLOBAL->PlaySound("MoveUnit", "Sounds/MoveUnit", 3);
    needBreakUpdate_ = true;
}

// Клетка доски должна быть пустой (проверка не производится).
void BoardLogic::CreateUnit(int gridX, int gridY)
{
    int colorIndex = Random(numColors_);

    Node* node = node_->CreateChild();
    node->SetName("Unit");
    node->SetVar("GridX", gridX);
    node->SetVar("GridY", gridY);
    node->SetVar("ColorIndex", colorIndex);
    node->SetPosition(GetCellPos(gridX, gridY));
    node->SetScale(0.1f);
    node->SetRotation(Quaternion(0.0f, 180.0f, 0.0f));
    node->CreateComponent<UnitAnimator>();

    StaticModel* object = node->CreateComponent<StaticModel>();
    object->SetModel(GET_MODEL("Models/Unit.mdl"));
    SharedPtr<Material> material = GET_MATERIAL("Materials/Unit.xml")->Clone();
    material->SetShaderParameter("MatDiffColor", colors[colorIndex]);
    object->SetMaterial(material);

    grid_[gridY * width_ + gridX] = node;
    needBreakUpdate_ = true;
}

Vector3 BoardLogic::GetCellPos(int gridX, int gridY)
{
    float posX = gridX - width_ * 0.5f + 0.5f;
    float posY = -(gridY - height_ * 0.5f + 0.5f);
    return Vector3(posX, posY, 0.0f);
}

// При удалении одноцветных линий будет произведено движение крайних юнитов. После этого
// может возникнуть необходимость снова удалить линии и снова подвинуть юниты
// по периметру. Это может повторяться неоднократно, и все это время игрок
// не может кликать по юнитам.
void BoardLogic::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    float timeStep = eventData[Update::P_TIMESTEP].GetFloat();

    needBreakUpdate_ = false;

    // Анимируем юниты, если нужно. Если было произведено движение
    // хотя бы одного юнита, то пользовательский ввод будет заблокирован.
    VariantMap& animateUnitEventData = GetEventDataMap();
    animateUnitEventData[AnimateUnit::P_TIMESTEP] = timeStep;
    SendEvent(E_ANIMATEUNIT, animateUnitEventData);

    if (needBreakUpdate_)
        return;

    MoveBorderUnits();

    // Если очередь была подвинута, то нужно проиграть анимацию.
    if (needBreakUpdate_)
        return;

    // Если игровое поле видно только как фон, то играть нельзя.
    // Также в этом состоянии линии не удаляются.
    if (GLOBAL->gameState_ != GS_GAMEPLAY)
        return;

    FindAndRemoveLines();

    // Если были удалены юниты, то нужно проиграть анимацию.
    if (needBreakUpdate_)
        return;

    // Если игроку некуда ходить, то заканчиваем игру.
    if (DetectGameOver())
    {
        // Звук GameOver.wav проигрывается в файле Game.cpp просто потому что так захотелось.
        GLOBAL->neededGameState_ = GS_GAME_OVER;
        return;
    }

    UpdateSelectedUnit();

    // В итоге игрок может кликать по юнитам, только если ни один юнит не перемещается.
    if (INPUT->GetMouseButtonPress(MOUSEB_LEFT) && !UI_MANAGER->GetHoveredElement())
        OnClickUnit(selectedUnit_);
}

void BoardLogic::OnClickUnit(Node* node)
{
    int gridX = node->GetVar("GridX").GetInt();
    int gridY = node->GetVar("GridY").GetInt();

    // Если юнит не на краю доски, то его нельзя толкнуть.
    // По идее эта проверка излишня. Она осталась от старого способа выбора юнитов.
    if (gridX != width_ - 1 && gridY != 0 && gridY != height_ - 1)
        return;

    // Угловые юниты тоже нельзя толкнуть.
    if (gridX == width_ - 1 && (gridY == 0 || gridY == height_ - 1))
        return;

    // Определяем направление движения юнита.
    IntVector2 dir;
    // Если юнит на верхней границе доски, то он должен двигаться вниз
    // (координата Y для игрового поля увеличивается сверху вниз).
    if (gridY == 0)
        dir = IntVector2(0, 1);
    // Если юнит на нижней границе сетки, то он должен двигаться вверх.
    else if (gridY == height_ - 1)
        dir = IntVector2(0, -1);
    // Последний возможный случай: юнит на правой
    // границе доски и должен двигаться влево.
    else
        dir = IntVector2(-1, 0);

    // Двигаем юнит пока возможно.
    IntVector2 newPos(gridX, gridY);
    while (true)
    {
        IntVector2 tryPos = newPos + dir;
        // Юнит не может выйти за левую границу.
        if (tryPos.x_ < 0)
            break;
        // Юнит не может перейти в уже занятую ячейку.
        if (grid_[tryPos.y_ * width_ + tryPos.x_])
            break;
        newPos = tryPos;
    }

    if (newPos.x_ == gridX && newPos.y_ == gridY) // Юнит остался на месте.
        return;

    // Снимаем выделение.
    auto staticModel = selectedUnit_->GetComponent<StaticModel>();
    auto material = staticModel->GetMaterial(0);
    material->SetShaderParameter("OutlineEnable", false);
    selectedUnit_ = nullptr;

    MoveUnit(node, newPos.x_, newPos.y_);

    // Сразу же двигаем юниты по периметру доски, иначе ряд подвинется только после того,
    // как юнит завершит свою анимацию. Лишняя пауза не нужна.
    MoveBorderUnits();
}

void BoardLogic::UpdateSelectedUnit()
{
    IntVector2 mousePos = INPUT->GetMousePosition();
    Viewport* viewport = RENDERER->GetViewport(0);

    // Первая клетка игрового поля.
    IntVector2 newSelectedCell(0, 0);
    Vector3 cellWorldPos = GetCellPos(0, 0);
    IntVector2 cellScreenPos = viewport->WorldToScreenPoint(cellWorldPos);
    float minDistSquared = DistanceSquared(mousePos, cellScreenPos);

    // Список крайних клеток кроме первой и угловых.
    PODVector<IntVector2> borderCells;
    borderCells.Reserve(width_ - 2 + width_ - 1 + height_ - 2);
    for (int gridX = 1; gridX < width_ - 1; gridX++)
        borderCells.Push(IntVector2(gridX, 0));
    for (int gridY = 1; gridY < height_ - 1; gridY++)
        borderCells.Push(IntVector2(width_ - 1, gridY));
    for (int gridX = 0; gridX < width_ - 1; gridX++)
        borderCells.Push(IntVector2(gridX, height_ - 1));

    // Ищем ближайшую к курсору клетку.
    for (int i = 0; i < (int)borderCells.Size(); i++)
    {
        IntVector2 cellGridPos = borderCells[i];
        cellWorldPos = GetCellPos(cellGridPos.x_, cellGridPos.y_);
        cellScreenPos = viewport->WorldToScreenPoint(cellWorldPos);
        float distSquared = DistanceSquared(mousePos, cellScreenPos);
        if (distSquared < minDistSquared)
        {
            minDistSquared = distSquared;
            newSelectedCell = cellGridPos;
        }
    }

    Node* newSelectedUnit = grid_[newSelectedCell.y_ * width_ + newSelectedCell.x_];

    if (newSelectedUnit == selectedUnit_)
        return;

    // Снимаем старое выделение.
    if (selectedUnit_ != nullptr)
    {
        auto staticModel = selectedUnit_->GetComponent<StaticModel>();
        auto material = staticModel->GetMaterial(0);
        material->SetShaderParameter("OutlineEnable", false);
    }

    // Выделяем новый юнит.
    selectedUnit_ = newSelectedUnit;
    auto staticModel = selectedUnit_->GetComponent<StaticModel>();
    auto material = staticModel->GetMaterial(0);
    material->SetShaderParameter("OutlineEnable", true);
}

void BoardLogic::MoveBorderUnits()
{
    // Создаем список крайних клеток доски.
    Vector<IntVector2> borderCells;
    borderCells.Reserve(width_ * 2 + height_ - 2);

    // Нижняя граница слева направо.
    for (int i = 0; i < width_; i++)
        borderCells.Push(IntVector2(i, height_ - 1));

    // Правая граница снизу вверх без угловых юнитов.
    for (int i = height_ - 2; i > 0; i--)
        borderCells.Push(IntVector2(width_ - 1, i));

    // Верхняя граница справа налево.
    for (int i = width_ - 1; i >= 0; i--)
        borderCells.Push(IntVector2(i, 0));

    // Для каждой клетки из списка кроме последней,
    for (int i = 0; i < (int)borderCells.Size() - 1; i++)
    {
        int gridX = borderCells[i].x_;
        int gridY = borderCells[i].y_;

        // если клетка пуста,
        if (!grid_[gridY * width_ + gridX])
        {
            // то ищем следующий юнит в очереди.
            Node* nextUnit = nullptr;
            for (int j = i + 1; j < (int)borderCells.Size(); j++)
            {
                int nextGridX = borderCells[j].x_;
                int nextGridY = borderCells[j].y_;

                nextUnit = grid_[nextGridY * width_ + nextGridX];
                if (nextUnit)
                    break;
            }
            
            if (nextUnit) // Если удалось найти следующий юнит,
                MoveUnit(nextUnit, gridX, gridY); // то перемещаем его в текущую клетку.
            else // А если юнитов больше нет,
                break; // то прерываем цикл.
        }
    }

    // Заполняем пустые клетки в конце списка.
    for (int i = (int)borderCells.Size() - 1; i >= 0; i--)
    {
        int gridX = borderCells[i].x_;
        int gridY = borderCells[i].y_;

        if (grid_[gridY * width_ + gridX]) // Если в клетке есть юнит,
            break; // то дальше можно не смотреть.
        
        CreateUnit(gridX, gridY);
    }
}

int BoardLogic::GetLineLength(int startX, int startY, const IntVector2& dir)
{
    Node* firstUnit = grid_[startY * width_ + startX];
    if (!firstUnit) // Стартовая клетка пустая.
        return 0;

    int firstColor = firstUnit->GetVar("ColorIndex").GetInt();
    int count = 1;

    for (int gridX = startX + dir.x_, gridY = startY + dir.y_;
        gridX >= 0 && gridX < width_ && gridY >= 0 && gridY < height_;
        gridX += dir.x_, gridY += dir.y_)
    {
        Node* unit = grid_[gridY * width_ + gridX];
        if (!unit) // Клетка пустая.
            break;
        
        int color = unit->GetVar("ColorIndex").GetInt();
        if (color != firstColor)
            break;

        count++;
    }

    return count;
}

// Проверки не производятся, входные данные должны быть корректными.
void BoardLogic::MarkToRemove(PODVector<bool>& output,
    int startX, int startY, int count, const IntVector2& dir)
{
    for (int i = 0; i < count; i++)
    {
        int gridX = startX + dir.x_ * i;
        int gridY = startY + dir.y_ * i;
        output[gridY * width_ + gridX] = true;
    }
}

void BoardLogic::FindAndRemoveLines()
{
    // Клетки, которые буду очищены.
    PODVector<bool> removedCells;
    
    // Вектор заполняется false'ми.
    removedCells.Resize(grid_.Size());
    for (unsigned i = 0; i < removedCells.Size(); i++)
        removedCells[i] = false;

    for (int gridX = 0; gridX < width_; gridX++)
    {
        for (int gridY = 0; gridY < height_; gridY++)
        {
            // Проверяем юниты вправо по горизонтали, начиная с текущей клетки.
            IntVector2 dirRight(1, 0);
            int countRight = GetLineLength(gridX, gridY, dirRight);
            if (countRight >= lineLength_)
                MarkToRemove(removedCells, gridX, gridY, countRight, dirRight);

            // Проверяем юниты вниз по вертикали, начиная с текущей клетки.
            IntVector2 dirDown(0, 1);
            int countDown = GetLineLength(gridX, gridY, dirDown);
            if (countDown >= lineLength_)
                MarkToRemove(removedCells, gridX, gridY, countDown, dirDown);

            if (diagonal_)
            {
                // Проверяем юниты вправо вниз по диагонали, начиная с текущей клетки.
                IntVector2 dirRightDown(1, 1);
                int countRightDown = GetLineLength(gridX, gridY, dirRightDown);
                if (countRightDown >= lineLength_)
                    MarkToRemove(removedCells, gridX, gridY, countRightDown, dirRightDown);

                // Проверяем юниты влево вниз по диагонали, начиная с текущей клетки.
                IntVector2 dirLeftDown(-1, 1);
                int countLeftDown = GetLineLength(gridX, gridY, dirLeftDown);
                if (countLeftDown >= lineLength_)
                    MarkToRemove(removedCells, gridX, gridY, countLeftDown, dirLeftDown);
            }
        }
    }

    // Уничтожаем юниты, отмеченные для удаления.
    for (int gridX = 0; gridX < width_; gridX++)
    {
        for (int gridY = 0; gridY < height_; gridY++)
        {
            if (removedCells[gridY * width_ + gridX])
                RemoveUnit(gridX, gridY);
        }
    }
}

void BoardLogic::RemoveUnit(int gridX, int gridY)
{
    // Отвязываем юнит от сетки и запускаем в полет.
    Node* unitNode = grid_[gridY * width_ + gridX];
    unitNode->AddTag("Removed");
    grid_[gridY * width_ + gridX] = nullptr;

    // Увеличиваем счет.
    score_++;
    String modeStr = BoardModeToString();
    if (score_ > CONFIG->GetRecord(modeStr))
        CONFIG->SetRecord(modeStr, score_);

    GLOBAL->PlaySound("RemoveUnit", "Sounds/RemoveUnit", 3);
    needBreakUpdate_ = true;
}

bool BoardLogic::DetectGameOver()
{
    // Игрок может походить, если во втором ряду вдоль периметра
    // есть хотя бы одно пустое место.

    // Вторая строка.
    for (int x = 0; x < width_ - 1; x++)
    {
        int y = 1;
        if (!grid_[y * width_ + x])
            return false;
    }

    // Предпоследняя строка.
    for (int x = 0; x < width_ - 1; x++)
    {
        int y = height_ - 2;
        if (!grid_[y * width_ + x])
            return false;
    }

    // Предпоследний столбец. Угловые клетки уже проверены.
    for (int y = 2; y < height_ - 2; y++)
    {
        int x = width_ - 2;
        if (!grid_[y * width_ + x])
            return false;
    }

    return true;
}

int BoardLogic::GetMaxInitialPopulation()
{
    // Стартовое население ограничено половиной клеток (без учета крайних).
    return (height_ - 2) * (width_ - 1) / 2;
}

int BoardLogic::GetMaxLineLength()
{
    return max(width_, height_);
}

void BoardLogic::ClampPopulationAndLineLength()
{
    initialPopulation_ = Clamp(initialPopulation_, 0, GetMaxInitialPopulation());
    lineLength_ = Clamp(lineLength_, 0, GetMaxLineLength());
}

// Первоначально режим упаковывался в биты одного числа типа unsigned,
// но в виде строки он выглядит понятнее при сохранении в конфиг.
String BoardLogic::BoardModeToString()
{
    return String("w") + width_ + "h" + height_ +
        "c" + numColors_ + "p" + initialPopulation_ +
        "l" + lineLength_ + "d" + diagonal_;
}
