/*
Доска всегда в центре координат. Локальные координаты дочерних нод
доски совпадают с их мировыми координатами.
*/

#pragma once
#include "Global.h"

#define MIN_BOARD_WIDTH 2
#define MAX_BOARD_WIDTH 10
#define DEFAULT_BOARD_WIDTH 6

#define MIN_BOARD_HEIGHT 3
#define MAX_BOARD_HEIGHT 10
#define DEFAULT_BOARD_HEIGHT 6

#define MIN_NUM_COLORS 3
#define MAX_NUM_COLORS 7
#define DEFAULT_NUM_COLORS 6

#define MIN_LINE_LENGTH 3
#define DEFAULT_LINE_LENGTH 3

#define DEFAULT_POPULATION 0

#define DEFAULT_DIAGONAL true

// Гарантируется, что игровое поле всегда доступно после инициализации игры.
#define BOARD_LOGIC GLOBAL->boardNode_->GetComponent<BoardLogic>()

// Событие вызывает функцию Animate для всех юнитов. Данное событие
// было добавлено, так как требуется выполнить в определенном порядке
// код из разных компонентов.
URHO3D_EVENT(E_ANIMATEUNIT, AnimateUnit)
{
    URHO3D_PARAM(P_TIMESTEP, TimeStep); // float
}

// Этот компонент реализует игровую логику.
// Компонент нужно прикрепить к пустой ноде и вызвать метод CreateBoard.
class BoardLogic : public Component
{
    URHO3D_OBJECT(BoardLogic, Component);

public:
    // Параметры игрового поля.
    int width_ = DEFAULT_BOARD_WIDTH;
    int height_ = DEFAULT_BOARD_HEIGHT;
    int numColors_ = DEFAULT_NUM_COLORS;
    int initialPopulation_ = DEFAULT_POPULATION;
    int lineLength_ = DEFAULT_LINE_LENGTH;
    bool diagonal_ = DEFAULT_DIAGONAL;
    
    // Реальный счет (отображаемый счет плавно стремится к этому значению).
    int score_ = 0;
 
    // Игрок не может походить, если в данный момент какие-то юниты движутся.
    bool needBreakUpdate_ = false;

    BoardLogic(Context* context);
    static void RegisterObject(Context* context);

    // Метод создает игровое поле.
    void CreateBoard();

    // Преобразует координаты ячейки в пространственные координаты ноды.
    Vector3 GetCellPos(int gridX, int gridY);

    int GetMaxInitialPopulation();
    int GetMaxLineLength();

    // После изменения размеров доски нужно корректировать значения
    // initialPopulation_ и lineLength_.
    void ClampPopulationAndLineLength();

    // Идентификатор для настроек игрового поля.
    String BoardModeToString();

    // Проверяет, что больше нет доступных ходов.
    bool DetectGameOver();

    Node* selectedUnit_ = nullptr;

    void UpdateSelectedUnit();

private:
    Vector<WeakPtr<Node> > grid_;

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    // Создает юнит случайного цвета.
    void CreateUnit(int gridX, int gridY);
    // Обрабатывает клик по юниту.
    void OnClickUnit(Node* node);
    // Перемещает юнит из одной ячейки в другую.
    void MoveUnit(Node* node, int gridX, int gridY);
    // Двигает очередь юнитов вдоль периметра доски, если впереди есть пустые места,
    // а затем добавляет новые юниты в конец очереди.
    void MoveBorderUnits();
    // Отвязывает юнит от сетки.
    void RemoveUnit(int gridX, int gridY);
    // Находит и удаляет линии из одноцветных юнитов.
    void FindAndRemoveLines();
    // Определяет количество одноцветных юнитов в каком-то направлении.
    // Если стартовая клетка пустая, то возвращает 0.
    int GetLineLength(int startX, int startY, const IntVector2& dir);
    // Устанавливает в true элементы двумерного массива.
    void MarkToRemove(PODVector<bool>& output, int startX, int startY, int count, const IntVector2& dir);
};
