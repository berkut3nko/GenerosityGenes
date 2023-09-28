#include "primary_utilities.hpp"

Minion::Minion(Point spawn_position, Colony* currentColony) :position(spawn_position), myColony(currentColony), 
MyBrain({ {MinionSettings::minionInputs + myColony->sizeMemmory, myColony->_neuronsCount}, {myColony->_neuronsCount, MinionSettings::minionOutputs+myColony->sizeMemmory} }, myColony->nameColony)
{
    id = MinionSettings::countMiniones;
    worldMap[position.x][position.y].type = minion;
    worldMap[position.x][position.y].minionAsdress = this;
    ++MinionSettings::countMiniones;
    memmory.resize(myColony->sizeMemmory,0);
    ++myColony->sizeColony;
}
Minion::Minion(Point spawn_position, Colony* currentColony, NeuralNetwork* parentBrain, double hungerForParent) :position(spawn_position), myColony(currentColony), MyBrain(*parentBrain), hunger(hungerForParent)
{
    id = MinionSettings::countMiniones;
    worldMap[position.x][position.y].type = minion;
    worldMap[position.x][position.y].minionAsdress = this;
    ++MinionSettings::countMiniones;
    memmory.resize(myColony->sizeMemmory, 0);
    ++myColony->sizeColony;
}
Minion::Minion(string data) :MyBrain(
    { {MinionSettings::minionInputs + myColony->sizeMemmory, myColony->_neuronsCount},
    {myColony->_neuronsCount, MinionSettings::minionOutputs + myColony->sizeMemmory} }, myColony->nameColony)
{
    LoadMe(data);
    myColony->minionAddresses.push_back(this);
    id = MinionSettings::countMiniones;
    worldMap[position.x][position.y].type = minion;
    worldMap[position.x][position.y].minionAsdress = this;
    ++MinionSettings::countMiniones;
    MyBrain = *myColony->colonyBrain;
    memmory.resize(myColony->sizeMemmory, 0);
    ++myColony->sizeColony;
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
}
double Minion::analyzeMove(infoMove move)
{
    double markMove = 0;
    if(move == infoMove::synthesis)
    {
        markMove += myColony->coef_Synthesis;
    }
    return markMove;

}


object tempObj;
std::vector<double> Minion::inputs()
{
    vector<double> input(MinionSettings::minionInputs+myColony->sizeMemmory,{0});

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            if (x != 1 || y != 1)
            {
                tempObj = worldMap[position.x - (x - 1)][position.y - (y - 1)];
                if (tempObj.type == Types::fruit)
                {
                    input[x + (y * 3)] = 1;
                }
                else if (tempObj.type == Types::air)
                {
                    input[x + (y * 3) + 1] = 1;
                }
                else if (tempObj.type == Types::border)
                {
                    input[x + (y * 3) + 2] = 1;
                }
                else if (tempObj.type == Types::minion)
                {
                    if(tempObj.minionAsdress->myColony == myColony)
                        input[x + (y * 3) + 3] = 1;
                    else 
                        input[x + (y * 3) + 4] = 1;
                    if(tempObj.minionAsdress->IsDead)
                        input[x + (y * 3) + 5] = 1;
                    if (tempObj.minionAsdress->IsProtection)
                        input[x + (y * 3) + 6] = 1;
                    input[x + (y * 3) + 7] = tempObj.minionAsdress->fat;
                }
            }
        }
    }
    for (size_t i = 0; i < myColony->sizeMemmory; ++i)
    {
        input[MinionSettings::minionInputs + i] = memmory[i];
    }
    return input;
}

void Minion::nextMove()
{
    if (IsDead)
    {
        if(rotting<5)
        ++rotting;
        else {
            rotting = 0;
            worldMap[position.x][position.y].type = air;
            worldMap[position.x][position.y].minionAsdress = nullptr;
            for (auto it = myColony->colonyAddresses.begin(); it != myColony->colonyAddresses.end(); ++it)
                if (*(it) == this)
                {
                    myColony->colonyAddresses.erase(it);
                    return;
                }
        }
        return;
    }
    else
    {
        vector<double> answers = MyBrain.forward(inputs());

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
        switch (answerId)
        {
        case 0:
            moveUp();
            break;
        case 1:
            moveDown();
            break;
        case 2:
            moveRight();
            break;
        case 3:
            moveLeft();
            break;
        case 4:
            moveUpLeft();
            break;
        case 5:
            moveUpRight();
            break;
        case 6:
            moveDownLeft();
            break;
        case 7:
            moveDownRight();
            break;
        case 8:
            StartSynthesis();
            break;
        case 9:
            RaiseProtection();
            break;
        case 10:
            bornUp();
            break;
        case 11:
            bornDown();
            break;
        case 12:
            bornLeft();
            break;
        case 13:
            bornRight();
            break;
        }

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
    if (hunger <= 0) 
    {
        IsDead = true; 
        hunger = 0; 
        --MinionSettings::countMiniones;
        --myColony->sizeColony;
    }

}
infoMove Minion::move(size_t newPosX, size_t newPosY)
{
    switch (worldMap[newPosX][newPosY].type)
    {
    case Types::border:
        getHungry(-.1f);
        break;
    case Types::minion:
        if (worldMap[newPosX][newPosY].minionAsdress->IsDead)
        {
            //Attack();
            getHungry(-.05f);
            return infoMove::attack;
        }
        else
        {
            getHungry(.1f);
            position.x = newPosX;
            position.y = newPosY;
            worldMap[position.x][position.y].type = minion;
            worldMap[position.x][position.y].minionAsdress = this;
            return infoMove::eat;
        }
        break;
    case Types::fruit:
        getHungry(.75f);
        position.x = newPosX;
        position.y = newPosY;
        worldMap[position.x][position.y].type = minion;
        worldMap[position.x][position.y].minionAsdress = this;
        return infoMove::eat;
        break;

    case Types::spawner:
        getHungry(-.1f);
        break;

    default:
        position.x = newPosX;
        position.y = newPosY;
        worldMap[position.x][position.y].type = minion;
        worldMap[position.x][position.y].minionAsdress = this;
        getHungry(-.02f);
        return infoMove::move;
        break;
    }
}

void Minion::born(size_t posX, size_t posY)
{
    if (worldMap[posX][posY].type == Types::air && hunger > 0.5f)
    {
        myColony->createMinion({ posX , posY }, this);
        getHungry(-0.5f);
    }
    else
    {
        getHungry(-0.02f);
    }
}

infoMove Minion::moveUp()
{
    stopPhases();
    return move(position.x, position.y - 1);
}

infoMove Minion::moveUpLeft()
{
    stopPhases();
    return move(position.x + 1, position.y - 1);
}

infoMove Minion::moveUpRight()
{
    stopPhases();
    return move(position.x - 1, position.y - 1);
}

infoMove Minion::moveDown()
{
    stopPhases();
    return move(position.x, position.y + 1);
}

infoMove Minion::moveDownLeft()
{
    stopPhases();
    return move(position.x + 1, position.y + 1);
}

infoMove Minion::moveDownRight()
{
    stopPhases();
    return move(position.x - 1, position.y + 1);
}

infoMove Minion::moveRight()
{
    stopPhases();
    return move(position.x + 1, position.y);
}

infoMove Minion::moveLeft()
{
    stopPhases();
    return move(position.x - 1, position.y);
}
infoMove Minion::bornUp()
{
    born(position.x, position.y - 1);
    return infoMove::born;
}
infoMove Minion::bornDown()
{
    born(position.x, position.y + 1);
    return infoMove::born;
}
infoMove Minion::bornLeft()
{
    born(position.x + 1, position.y);
    return infoMove::born;
}
infoMove Minion::bornRight()
{
    born(position.x - 1, position.y);
    return infoMove::born;
}

infoMove Minion::StartSynthesis()
{
    stopPhases();
    IsSynthesis = true;
    getHungry(-0.05f);
    return infoMove::synthesis;
}
infoMove Minion::RaiseProtection()
{
    stopPhases();
    IsProtection = true;
    getHungry(-0.1f);
    return infoMove::protection;
}
string Minion::SaveMe()
{
    string data = myColony->nameColony + ' '
        + std::to_string(position.x) + ' '
        + std::to_string(position.y) + ' '
        + std::to_string(hunger) + ' '
        + std::to_string(fat) + ' '
        + (IsDead ? "true" : "false");
    return data;
}

void Minion::LoadMe(string data)
{
    std::istringstream iss(data);
    string value_str;

    std::getline(iss, value_str, ' ');
    if (allColonys.count(value_str) > 0) {
        myColony = allColonys[value_str];
    }
    else {
        if (allColonys.size() > 0)
            myColony = allColonys.begin()->second;
        else
            return; // call default constructor (dev tip)
    }

    std::getline(iss, value_str, ' ');
    this->position.x = stoull(value_str);

    std::getline(iss, value_str, ' ');
    this->position.y = stoull(value_str);

    std::getline(iss, value_str, ' ');
    hunger = stod(value_str);

    std::getline(iss, value_str, ' ');
    fat = stod(value_str);

    std::getline(iss, value_str, ' ');
    IsDead = (value_str == "true");
}
