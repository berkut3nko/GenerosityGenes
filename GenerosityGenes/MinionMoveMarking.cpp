#include "primary_utilities.hpp"


void Minion::setMarkForMove(size_t answerId)
{
    std::vector<std::vector<object>> localMap(sizeWorldX, std::vector<object>(sizeWorldY));
    std::copy(worldMap.begin(), worldMap.end(), localMap.begin());
    std::vector<double> pointsForMove(14);
    infoMove currentInfoMove;
    for (size_t currentMoveAnalize = 0; currentMoveAnalize < 14; ++currentMoveAnalize)
    {
        switch (currentMoveAnalize)
        {
        case 0:
            currentInfoMove = moveUp();
            break;
        case 1:
            currentInfoMove = moveDown();
            break;
        case 2:
            currentInfoMove = moveRight();
            break;
        case 3:
            currentInfoMove = moveLeft();
            break;
        case 4:
            currentInfoMove = moveUpLeft();
            break;
        case 5:
            currentInfoMove = moveUpRight();
            break;
        case 6:
            currentInfoMove = moveDownLeft();
            break;
        case 7:
            currentInfoMove = moveDownRight();
            break;
        case 8:
            currentInfoMove = StartSynthesis();
            break;
        case 9:
            currentInfoMove = RaiseProtection();
            break;
        case 10:
            currentInfoMove = bornUp();
            break;
        case 11:
            currentInfoMove = bornDown();
            break;
        case 12:
            currentInfoMove = bornLeft();
            break;
        case 13:
            currentInfoMove = bornRight();
            break;
        }
        pointsForMove[currentMoveAnalize] = analyzeMove(currentInfoMove);
        std::copy(localMap.begin(), localMap.end(), worldMap.begin());
    }
    if (*std::max(pointsForMove.begin(), pointsForMove.end()) == pointsForMove[answerId])
        points += pointsForMove[answerId] * 2.01634623;//buff if best move
    else
        points += pointsForMove[answerId];
}
double Minion::analyzeMove(infoMove move)
{
    double markMove = 0;
    switch (move)
    {
    case infoMove::synthesis:
        markMove += myColony->coef_Synthesis + (analyzePos(move) / 8.0);
        break;

    case infoMove::protection:
        markMove += myColony->coef_Protection + (analyzePos(move) / 8.0);
        break;

    case infoMove::born:
        markMove += myColony->coef_Born + (analyzePos(move) / 8.0);
        break;

    case infoMove::attackEnemy:
        markMove += myColony->coef_AttackEnemy + (analyzePos(move) / 8.0);
        break;

    case infoMove::attackTeam:
        markMove += myColony->coef_AttackTeam + (analyzePos(move) / 8.0);
        break;

    case infoMove::eat:
        markMove += myColony->coef_Eat + (analyzePos(move) / 8.0);
        break;

    case infoMove::eatTeammate:
        markMove += myColony->coef_Eat + myColony->coef_AttackTeam + (analyzePos(move) / 8.0);
        break;

    case infoMove::eatEnemy:
        markMove += myColony->coef_Eat + myColony->coef_AttackEnemy + (analyzePos(move) / 8.0);
        break;

    case infoMove::move:
        markMove += (analyzePos(move) / 8.0);
        break;
    default:
        break;
    }
    return markMove;

}
object tempObj;
double Minion::analyzePos(infoMove move)
{
    object tempObj;
    double posMark = 0;
    for (int y = 0; y < 5; ++y)
    {
        for (int x = 0; x < 5; ++x)
        {
            if (x != 1 || y != 1)
            {
                tempObj = worldMap[position.x - (x - 2)][position.y - (y - 2)];
                if (tempObj.type == Types::fruit)
                {
                    posMark += myColony->coef_EatClose;
                    continue;
                }
                if (tempObj.type == Types::minion)
                {
                    if (tempObj.minionAddress->myColony == myColony)
                        posMark += myColony->coef_TeamClose;
                    else {
                        posMark += myColony->coef_EnemyClose;
                        if (move == infoMove::protection)
                            posMark += (myColony->coef_EnemyClose + myColony->coef_Protection) * 2;

                    }
                    continue;
                }
                if (tempObj.type == Types::border)
                {
                    posMark += myColony->coef_Border;
                    continue;
                }
                if (tempObj.type == Types::spawner)
                {
                    for (std::pair<shared_ptr<Colony>, shared_ptr<Spawner>> spawner : allActiveSpawners)
                    {
                        if (spawner.second->spawnerPosition.x == position.x - (x - 1) && spawner.second->spawnerPosition.y == position.y - (y - 1))
                        {
                            if (spawner.first = myColony)
                                posMark += myColony->coef_TeamSpawnerClose;
                            else
                                posMark += myColony->coef_EnemySpawnerClose;
                        }
                    }
                    continue;
                }
            }
        }
    }
    return posMark;
}


std::vector<double> Minion::inputs()
{
    vector<double> input(MinionSettings::minionInputs + myColony->sizeMemmory, { 0 });

    for (int y = 0; y < 5; ++y)
    {
        for (int x = 0; x < 5; ++x)
        {
            if (x != 2 || y != 2)
            {
                tempObj = worldMap[position.x - (x - 2)][position.y - (y - 2)];
                //Firstblock
                switch (tempObj.type)
                {
                case Types::fruit:
                    input[(x * 3) + (y * 15)] = 1;
                    break;
                case Types::border:
                    input[(x * 3) + (y * 15)] = -1;
                    break;
                case Types::spawner:
                    input[(x * 3) + (y * 15)] = -1;
                    break;
                case Types::air:
                    input[(x * 3) + (y * 15)] = 0;
                    break;
                default:
                    input[(x * 3) + (y * 15)] = 0;
                    break;
                }
                //Second block
                if ((tempObj.type == Types::minion) && tempObj.minionAddress != nullptr)
                {
                    if (tempObj.minionAddress->myColony == myColony)
                    {
                        input[(x * 3) + (y * 15) + 1] = 1;
                    }
                    else
                    {
                        input[(x * 3) + (y * 15) + 1] = -1;
                    }
                }
                else if (tempObj.type == Types::spawner)
                {
                    for (std::pair<shared_ptr<Colony>, shared_ptr<Spawner>> spawner : allActiveSpawners)
                    {
                        if (spawner.second->spawnerPosition == Point({ position.x - (x - 2), position.y - (y - 2) }))
                        {
                            if (spawner.first == myColony)
                            {
                                input[(x * 3) + (y * 15) + 1] = 1;
                            }
                            else
                            {
                                input[(x * 3) + (y * 15) + 1] = -1;
                            }
                        }
                    }
                }
                else
                {
                    input[(x * 3) + (y * 15) + 1] = 0;
                }

                //Trird block
                if (tempObj.type == Types::minion && tempObj.minionAddress != nullptr)
                {
                    if (tempObj.minionAddress->IsProtection)
                    {
                        input[(x * 3) + (y * 15) + 2] = 1;
                    }
                    else
                        if (tempObj.minionAddress->IsDead)
                        {
                            input[(x * 3) + (y * 15) + 2] = -1;
                        }
                        else
                        {
                            input[(x * 3) + (y * 15) + 2] = 0;
                        }
                }
            }
        }
    }
    input[MinionSettings::minionInputs - 2] = hunger;
    input[MinionSettings::minionInputs - 1] = fat;
    if (!memmory.empty())
        for (size_t i = 0; i < myColony->sizeMemmory; ++i)
        {
            input[MinionSettings::minionInputs + i] = memmory[i];
        }
    return input;
}