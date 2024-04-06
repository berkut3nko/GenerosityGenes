#include "primary_utilities.hpp"
using namespace MinionSettings;

void Minion::stopPhases()
{
    IsProtection = false;
    IsSynthesis = false;
}
void Minion::kill()
{
    if (this != nullptr) {
        worldMap[position.x][position.y].type = air;
        for (vector<shared_ptr<Minion>>::iterator it = myColony->colonyAddresses.begin(); it != myColony->colonyAddresses.end(); ) {
            if (it->get()->ID == ID)
            {
                myColony->colonyAddresses.erase(it);
                --countMiniones;
                --myColony->sizeColony;
                break;
            }
            else ++it;
        }
        Colony::minionAddresses.erase(this->ID);
    }
}

void Minion::nextMove()
{
    if(this)
    if (IsDead)
    {
        if (rotting < 5) {
            ++rotting;
            return;
        }
        else 
        {
            rotting = 6;
            this->kill();
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
        stopPhases();
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
    if (count > 0.0)
        health = 1.0;
    else
        count *= eat_cost;
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
    colonyArea.insert({Point{position.x ,position.y},myColony});
    colonyArea.insert({(Point{ position.x + 1 ,position.y + 1 }),myColony});
    colonyArea.insert({(Point{ position.x + 1 ,position.y - 1 }), myColony});
    colonyArea.insert({(Point{ position.x - 1 ,position.y + 1 }), myColony});
    colonyArea.insert({(Point{ position.x - 1 ,position.y - 1 }), myColony});
    colonyArea.insert({(Point{ position.x ,position.y + 1 }), myColony});
    colonyArea.insert({(Point{ position.x ,position.y - 1 }), myColony});
    colonyArea.insert({(Point{ position.x - 1 ,position.y }), myColony});
    colonyArea.insert({(Point{ position.x + 1 ,position.y }), myColony});
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
            worldMap[newPosX][newPosY].minionAddress->kill();
            move(newPosX, newPosY);
            if(worldMap[newPosX][newPosY].minionAddress->myColony == myColony)
                return infoMove::eatTeammate;
            else 
                return infoMove::eatEnemy;
        }
        break;
    case Types::fruit:
        getHungry(0.8);
        poolOfFruits.erase(Point{ newPosX,newPosY });
        move(newPosX, newPosY);
        return infoMove::eat;
        break;

    case Types::spawner:
        getHungry(-.02);
        return infoMove::moveToBorder;
        break;

    case Types::air:
        move(newPosX, newPosY);
        getHungry(-.02);
        return infoMove::move;
        break;
    default:
        return infoMove::move;
        break;
    }
    return infoMove::move;
}
void Minion::Attack(Minion* minion)
{
    if (minion->IsProtection)
    {
        minion->health -= 0.25;
    }
    else {
        if (minion->IsSynthesis)
        {
            minion->health -= 1.00;
        }
        else
        {
            minion->health -= 0.5;
        }
    }

    minion->getHungry(0.0);
    return;
}
infoMove Minion::born(size_t posX, size_t posY)
{
    if (worldMap[posX][posY].type == Types::air && hunger > 0.5f)
    {
        myColony->createMinion({ posX , posY }, this, 0.5);
        getHungry(-0.4);
        return infoMove::born;
    }
    else
    {
        return interact(posX, posY);
    }
}

infoMove Minion::moveUp()
{
    return interact(position.x, position.y - 1);
}

infoMove Minion::moveUpLeft()
{
    return interact(position.x + 1, position.y - 1);
}

infoMove Minion::moveUpRight()
{
    return interact(position.x - 1, position.y - 1);
}

infoMove Minion::moveDown()
{
    return interact(position.x, position.y + 1);
}

infoMove Minion::moveDownLeft()
{
    return interact(position.x + 1, position.y + 1);
}

infoMove Minion::moveDownRight()
{
    return interact(position.x - 1, position.y + 1);
}

infoMove Minion::moveRight()
{
    return interact(position.x + 1, position.y);
}

infoMove Minion::moveLeft()
{
    return interact(position.x - 1, position.y);
}
infoMove Minion::bornUp()
{
    return born(position.x, position.y - 1);
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
    IsSynthesis = true;
    getHungry(-0.01);
    return infoMove::synthesis;
}
infoMove Minion::RaiseProtection()
{
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
