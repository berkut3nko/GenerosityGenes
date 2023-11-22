#include "primary_utilities.hpp"

Minion::Minion(Point spawn_position, Colony* currentColony) :position(spawn_position), myColony(currentColony), 
MyBrain({ {MinionSettings::minionInputs + myColony->sizeMemmory, myColony->_neuronsCount.first},
          {myColony->_neuronsCount.first, myColony->_neuronsCount.second},
          {myColony->_neuronsCount.second,MinionSettings::minionOutputs + myColony->sizeMemmory} }, myColony->nameColony)
{
    id = MinionSettings::countMiniones;
    worldMap[position.x][position.y].type = minion;
    worldMap[position.x][position.y].minionAddress = this;
    ++MinionSettings::countMiniones;
    Colony::minionAddresses.push_back(this);
    memmory.resize(myColony->sizeMemmory,0);
    ++myColony->sizeColony;
    allocateArea();
}
Minion::Minion(Point spawn_position, Colony* currentColony, NeuralNetwork* parentBrain, double hungerFromParent) :position(spawn_position), myColony(currentColony), MyBrain(*parentBrain), hunger(hungerFromParent)
{
    id = MinionSettings::countMiniones;
    worldMap[position.x][position.y].type = minion;
    worldMap[position.x][position.y].minionAddress = this;
    ++MinionSettings::countMiniones;
    Colony::minionAddresses.push_back(this);
    memmory.resize(myColony->sizeMemmory, 0);
    ++myColony->sizeColony;
    allocateArea();
}
Minion::Minion(Colony* _myColony, Point _position,double _fat,double _hunger):myColony(_myColony),MyBrain(_myColony->bestMinionBrain), position(_position),fat(_fat),hunger(_hunger)
{

    id = MinionSettings::countMiniones;
    ++MinionSettings::countMiniones;

    worldMap[position.x][position.y].type = minion;
    worldMap[position.x][position.y].minionAddress = this;

    Colony::minionAddresses.push_back(this);
    myColony->colonyAddresses.push_back(this);
    MyBrain = myColony->bestMinionBrain;
    memmory.resize(myColony->sizeMemmory, 0);
    ++myColony->sizeColony;
    allocateArea();
}


void Minion::stopPhases()
{
    IsProtection = false;
    IsSynthesis = false;
}

void Minion::setMarkForMove(size_t answerId)
{
    std::vector<std::vector<object>> localMap(sizeWorldX, std::vector<object>(sizeWorldY));
    std::copy(worldMap.begin(), worldMap.end(),localMap.begin());
    std::vector<double> pointsForMove(14);
    infoMove currentInfoMove;
    for (size_t currentMoveAnalize = 0; currentMoveAnalize < 14; ++currentMoveAnalize)
    {
        switch (currentMoveAnalize)
        {
        case 0:
            currentInfoMove=moveUp();
            break;
        case 1:
            currentInfoMove=moveDown();
            break;
        case 2:
            currentInfoMove=moveRight();
            break;
        case 3:
            currentInfoMove=moveLeft();
            break;
        case 4:
            currentInfoMove=moveUpLeft();
            break;
        case 5:
            currentInfoMove=moveUpRight();
            break;
        case 6:
            currentInfoMove=moveDownLeft();
            break;
        case 7:
            currentInfoMove=moveDownRight();
            break;
        case 8:
            currentInfoMove=StartSynthesis();
            break;
        case 9:
            currentInfoMove=RaiseProtection();
            break;
        case 10:
            currentInfoMove=bornUp();
            break;
        case 11:
            currentInfoMove=bornDown();
            break;
        case 12:
            currentInfoMove=bornLeft();
            break;
        case 13:
            currentInfoMove=bornRight();
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
        markMove += myColony->coef_Synthesis + (analyzePos(move)/8.0);
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
                    if(tempObj.minionAddress->myColony == myColony)
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
                    for (std::pair<Colony*, Spawner*> spawner : allActiveSpawners)
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
    vector<double> input(MinionSettings::minionInputs+myColony->sizeMemmory,{0});

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
                    for (auto spawner : allActiveSpawners)
                    {
                        if (spawner.second->spawnerPosition == Point(position.x - (x - 2), position.y - (y - 2)))
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
    for (size_t i = 0; i < myColony->sizeMemmory; ++i)
    {
        input[MinionSettings::minionInputs + i] = memmory[i];
    }
    return input;
}

void Minion::nextMove()
{
    if(this)
    if (IsDead == true)
    {
        if (rotting < 5) {
            ++rotting;
            return;
        }
        else 
        {
            rotting = 6;
            worldMap[position.x][position.y].type = air;
            for (auto it = myColony->colonyAddresses.begin(); it != myColony->colonyAddresses.end(); ++it)
                if (*(it) == this)
                {
                    myColony->colonyAddresses.erase(it);
                    --MinionSettings::countMiniones;
                    --myColony->sizeColony;
                    return;
                }
        }

    }
    else
    {
        vector<double> answers = MyBrain.forward_minion(inputs());

        //Пошук максимального
        size_t answerId = 0; double maxValue = answers[0];
        for (size_t id = 0; id < MinionSettings::minionOutputs; ++id)
        {
            if (answers.at(id) > maxValue)
            {
                maxValue = answers.at(id);
                answerId = id;
            }
        }
        for (size_t i = 0; i < myColony->sizeMemmory; ++i)
        {
            memmory[i] = answers[i + MinionSettings::minionOutputs];
        }
        infoMove currentMove;
        //setMarkForMove(answerId);
        switch (answerId)
        {
        case 0:
            currentMove = moveUp();
            break;
        case 1:
            currentMove = moveDown();
            break;
        case 2:
            currentMove = moveRight();
            break;
        case 3:
            currentMove = moveLeft();
            break;
        case 4:
            currentMove = moveUpLeft();
            break;
        case 5:
            currentMove = moveUpRight();
            break;
        case 6:
            currentMove = moveDownLeft();
            break;
        case 7:
            currentMove = moveDownRight();
            break;
        case 8:
            currentMove = StartSynthesis();
            break;
        case 9:
            currentMove = RaiseProtection();
            break;
        case 10:
            currentMove = bornUp();
            break;
        case 11:
            currentMove = bornDown();
            break;
        case 12:
            currentMove = bornLeft();
            break;
        case 13:
            currentMove = bornRight();
            break;
        }
        points += analyzeMove(currentMove);
        return;
    }
}
void Minion::getHungry(double count)
{

    hunger = hunger + fat + count;
    if (hunger > 1)
    {
        hunger = 1;
        fat = hunger + fat + count - 1;
    }
    if (hunger <= 0 || health <= 0) 
    {
        IsDead = true; 
        stopPhases();
        hunger = 0; 
    }

}
void Minion::allocateArea()
{
    colonyArea.insert(Point{position.x ,position.y});
    colonyArea.insert((Point{ position.x + 1 ,position.y + 1 }));
    colonyArea.insert((Point{ position.x + 1 ,position.y - 1 }));
    colonyArea.insert((Point{ position.x - 1 ,position.y + 1 }));
    colonyArea.insert((Point{ position.x - 1 ,position.y - 1 }));
    colonyArea.insert((Point{ position.x ,position.y + 1 }));
    colonyArea.insert((Point{ position.x ,position.y - 1 }));
    colonyArea.insert((Point{ position.x - 1 ,position.y }));
    colonyArea.insert((Point{ position.x + 1 ,position.y }));

    worldMap[position.x+1][position.y-1].minionAddress = this;
    worldMap[position.x+1][position.y-1].minionAddress = this;
    worldMap[position.x-1][position.y+1].minionAddress = this;
    worldMap[position.x-1][position.y-1].minionAddress = this;

    worldMap[position.x+1][position.y].minionAddress = this;
    worldMap[position.x-1][position.y].minionAddress = this;
    worldMap[position.x][position.y+1].minionAddress = this;
    worldMap[position.x][position.y-1].minionAddress = this;

}
void Minion::move(size_t MovePosX, size_t MovePosY)
{
    worldMap[position.x][position.y].type = Types::air;
    position.x = MovePosX;
    position.y = MovePosY;
    worldMap[position.x][position.y].type = minion;
    //3x3 mark area 
    worldMap[position.x][position.y].minionAddress = this;
    allocateArea();

}
infoMove Minion::interact(size_t newPosX, size_t newPosY)
{
    switch (worldMap[newPosX][newPosY].type)
    {
    case Types::border:
        getHungry(-.02);
        return infoMove::moveToBorder;
        break;
    case Types::minion:
        if (!worldMap[newPosX][newPosY].minionAddress->IsDead)
        {
            Attack(worldMap[newPosX][newPosY].minionAddress);
            getHungry(-.05);
            if (worldMap[newPosX][newPosY].minionAddress->myColony == myColony)
            {
                return infoMove::attackTeam;
            }
            else
            {
                return infoMove::attackEnemy;
            }
        }
        else
        {
            getHungry(.5);
            for (auto it = myColony->colonyAddresses.begin(); it != myColony->colonyAddresses.end(); ++it)
            {
                if ((*it) == worldMap[newPosX][newPosY].minionAddress)
                {
                    myColony->colonyAddresses.erase(it);
                    break;
                }
            }
            move(newPosX, newPosY);
            if(worldMap[newPosX][newPosY].minionAddress->myColony == myColony)
                return infoMove::eatTeammate;
            else 
                return infoMove::eatEnemy;
        }
        break;
    case Types::fruit:
        getHungry(1.0);
        poolOfFruits.erase(Point{ newPosX,newPosY });
        move(newPosX, newPosY);
        return infoMove::eat;
        break;

    case Types::spawner:
        getHungry(-.02);
        break;

    case Types::air:
        move(newPosX, newPosY);
        getHungry(-.02);
        return infoMove::move;
        break;
    }
}
void Minion::Attack(Minion* minion)
{
    if (minion->IsProtection)
    {
        minion->health -= 0.25;
    }
    else
        if (minion->IsSynthesis)
        {
            minion->health -= 1.00;
        }
        else
            {
                minion->health -= 0.5;
            }

    minion->getHungry(0.0);
    return;
}
infoMove Minion::born(size_t posX, size_t posY)
{
    if (worldMap[posX][posY].type == Types::air && hunger > 0.5f)
    {
        myColony->createMinion({ posX , posY }, this, 0.5);
        getHungry(-0.5);
        return infoMove::born;
    }
    else
    {
        return interact(posX, posY);
    }
}

infoMove Minion::moveUp()
{
    stopPhases();
    return interact(position.x, position.y - 1);
}

infoMove Minion::moveUpLeft()
{
    stopPhases();
    return interact(position.x + 1, position.y - 1);
}

infoMove Minion::moveUpRight()
{
    stopPhases();
    return interact(position.x - 1, position.y - 1);
}

infoMove Minion::moveDown()
{
    stopPhases();
    return interact(position.x, position.y + 1);
}

infoMove Minion::moveDownLeft()
{
    stopPhases();
    return interact(position.x + 1, position.y + 1);
}

infoMove Minion::moveDownRight()
{
    stopPhases();
    return interact(position.x - 1, position.y + 1);
}

infoMove Minion::moveRight()
{
    stopPhases();
    return interact(position.x + 1, position.y);
}

infoMove Minion::moveLeft()
{
    stopPhases();
    return interact(position.x - 1, position.y);
}
infoMove Minion::bornUp()
{
    born(position.x, position.y - 1);
    return infoMove::born;
}
infoMove Minion::bornDown()
{
    return born(position.x, position.y + 1);
}
infoMove Minion::bornLeft()
{
    return born(position.x + 1, position.y);
}
infoMove Minion::bornRight()
{   
    return born(position.x - 1, position.y);
}

infoMove Minion::StartSynthesis()
{
    stopPhases();
    IsSynthesis = true;
    getHungry(-0.01);
    return infoMove::synthesis;
}
infoMove Minion::RaiseProtection()
{
    stopPhases();
    IsProtection = true;
    getHungry(-0.05);
    return infoMove::protection;
}
string Minion::SaveMe()
{
    string data = myColony->nameColony + ' '
        + std::to_string(position.x) + ' '
        + std::to_string(position.y) + ' '
        + std::to_string(hunger) + ' '
        + std::to_string(fat);
    return data;
}
