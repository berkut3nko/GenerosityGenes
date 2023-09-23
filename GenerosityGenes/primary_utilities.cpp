#include "primary_utilities.hpp"
std::vector<std::vector<object>> worldMap(sizeWorldX, std::vector<object>(sizeWorldY));
std::map<std::string, Colony*> allColonys;

void worldInitialization()
{
    for (size_t x = 0; x < sizeWorldX; ++x)
    {
        worldMap[x][0].type = border;
        worldMap[x][sizeWorldY - 1].type = border;
        worldMap[x][0].color = dC::border;
        worldMap[x][sizeWorldY - 1].color = dC::border;
    }
    for (size_t y = 1; y < sizeWorldY - 1; ++y)
    {
        worldMap[0][y].type = border;
        worldMap[sizeWorldX - 1][y].type = border;
        worldMap[0][y].color = dC::border;
        worldMap[sizeWorldX - 1][y].color = dC::border;
    }

}



//�� ������������ ������ ������
Colony::Colony(size_t neuronsCount, std::string name) : nameColony(name), _neuronsCount(neuronsCount)
{
    colonyColor = sf::Color(rand() % 256, rand() % 256, rand() % 256, rand() % 256);

    allColonys.insert(std::make_pair(nameColony, this));
}
//�� ���������������� �����������
Colony::Colony(string name) : nameColony(name)
{
    colonyBrain->NeuralNetworkWay = name;
    LoadColony();
    allColonys.insert(std::make_pair(nameColony, this));
}

//�������� �������� ���'�� 
Colony::~Colony()
{
    for (auto minion : minionAddresses)
    {
        if (minion->myColony == this)
        {
            delete minion;
        }
    }
}

//����������� �������� ���������� �� ���� ������
std::vector<Minion*> Colony::minionAddresses;


void Colony::createMinion(Point coordinate)
{
    Minion* newMinion = new Minion(coordinate, this);
    minionAddresses.push_back(newMinion);
    colonyAddresses.push_back(newMinion);
    ++MinionSettings::countMiniones;
}

//������� ��������� �����
void Colony::startLife()
{
    size_t count = 0;
    size_t maxPoints=0;
    bool leaveOne = false;
    while (true) {
        for (size_t fruitI = 0; fruitI < static_cast<size_t>(speedSummonFruit / 1.0f); ++fruitI)
        {
            summonFruit();
        }



        for (size_t i = 0; i < 10; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            render();
        }



        for (Minion* minion : minionAddresses)
        {
            minion->nextMove();
        }
        ++count;
        if (count % 5 == 0)   //(dev tip)
        {
            for (auto& item : allColonys)
            {
                for (const auto minion : item.second->colonyAddresses)
                {
                    if (!minion->IsDead) {
                        if (minion->points > maxPoints)
                        {
                            maxPoints = minion->points;
                            item.second->colonyBrain = &(minion->MyBrain);
                        }
                        minion->points = 0;
                    }
                }
                if (item.second->colonyBrain != nullptr)
                {
                    for (const auto minion : item.second->colonyAddresses)
                    {
                        minion->MyBrain = *(item.second->colonyBrain);
                        if (leaveOne)
                        {
                            minion->MyBrain.mutate();
                        }
                        else leaveOne = true;
                    }
                }
            }
        }
    }
}

void Colony::createMinion()
{
    size_t Xtemp, Ytemp;
    while (true)
    {
        Xtemp = rand() % sizeWorldX;
        Ytemp = rand() % sizeWorldY;
        if (worldMap[Xtemp][Ytemp].type == Types::air)
        {
            Minion* newMinion = new Minion({ Xtemp ,Ytemp }, this);
            minionAddresses.push_back(newMinion);
            colonyAddresses.push_back(newMinion);
            ++MinionSettings::countMiniones;
            return;
        }
    }

}
void Colony::summonFruit()
{
    size_t Xtemp, Ytemp;
    while (true)
    {
        Xtemp = rand() % sizeWorldX;
        Ytemp = rand() % sizeWorldY;
        if (worldMap[Xtemp][Ytemp].type == Types::air)
        {
            worldMap[Xtemp][Ytemp].type  = Types::fruit; 
            worldMap[Xtemp][Ytemp].color =    dC::fruit;
            return;
        }
    }
}


//Save - Load ������
void Colony::SaveColony()
{
    colonyBrain->SaveAI();
}

void Colony::LoadColony()
{
    colonyBrain->LoadAI();
}

void Colony::SaveMiniones(string version)
{
    std::ofstream file(version + "_miniones");
    if (!file.is_open()) {

        return;
    }

    for (const auto& minion : minionAddresses)
    {
        file << minion->SaveMe() << '\n';
    }

    file.close();
}

void Colony::LoadMiniones(string version)
{
    std::ifstream file(version + "_miniones");
    if (!file.is_open()) {

        return;
    }

    string dataMinion;
    Minion* temp;
    while (std::getline(file, dataMinion))
    {
        temp = new Minion(dataMinion);
        minionAddresses.push_back(temp);
    }

    file.close();
}
