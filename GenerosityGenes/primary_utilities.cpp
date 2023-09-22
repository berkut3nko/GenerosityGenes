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
unsigned int findMultiplierSize(unsigned int monitorSizeX, unsigned int monitorSizeY)
{
    return (monitorSizeX / sizeWorldX >= monitorSizeY / sizeWorldY) ? monitorSizeY / sizeWorldY : monitorSizeX / sizeWorldX;
}
unsigned int multiplicator = findMultiplierSize(1680, 980);
sf::RenderWindow window(sf::VideoMode(multiplicator* sizeWorldX, multiplicator* sizeWorldY), "SFML works!");
void render()
{
    sf::RectangleShape tempShape;
    sf::CircleShape tempShapeMinion;
    if (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        for (int x = 0; x < sizeWorldX; ++x) {
            for (int y = 0; y < sizeWorldY; ++y) {
                if (worldMap[x][y].type != minion)
                {
                    tempShape.setSize(sf::Vector2f(multiplicator, multiplicator));
                    tempShape.setFillColor(worldMap[x][y].color);
                    tempShape.setPosition(sf::Vector2f(multiplicator * x, multiplicator * y));
                    window.draw(tempShape);
                }
                else 
                {
                    tempShape.setSize(sf::Vector2f(multiplicator, multiplicator));
                    tempShape.setFillColor(dC::air);
                    tempShape.setPosition(sf::Vector2f(multiplicator * x, multiplicator * y));
                    window.draw(tempShape);
                    tempShapeMinion.setRadius(static_cast<float>(multiplicator / 2.5));
                    tempShapeMinion.setOutlineColor(dC::air);
                    if(!worldMap[x][y].minionAsdress->IsDead)
                        tempShapeMinion.setFillColor(worldMap[x][y].color);
                    else
                        tempShapeMinion.setFillColor(dC::dead);
                    tempShapeMinion.setPosition(sf::Vector2f(multiplicator * x, multiplicator * y));

                    if (worldMap[x][y].minionAsdress->IsSynthesis)
                    {
                        tempShapeMinion.setOutlineThickness(static_cast<size_t>(multiplicator/15));
                        tempShapeMinion.setOutlineColor(sf::Color::Green);
                    }
                    else if (worldMap[x][y].minionAsdress->IsProtection)
                    {
                        tempShapeMinion.setOutlineThickness(static_cast<size_t>(multiplicator / 15));
                        tempShapeMinion.setOutlineColor(sf::Color::Blue);
                    }
                    else
                    {
                        tempShapeMinion.setOutlineThickness(static_cast<size_t>(multiplicator / 30));
                        tempShapeMinion.setOutlineColor(sf::Color::Black);
                    }
                    window.draw(tempShapeMinion);
                }
            }
        }

        window.display();
    }
}


//Це конснтруктор класса Колонія
Colony::Colony(size_t neuronsCount, std::string name) : nameColony(name), _neuronsCount(neuronsCount)
{
    colonyColor = sf::Color(rand() % 256, rand() % 256, rand() % 256, rand() % 256);

    allColonys.insert(std::make_pair(nameColony, this));
}
//Це завантажувальний конструктор
Colony::Colony(string name) : nameColony(name)
{
    colonyBrain->NeuralNetworkWay = name;
    LoadColony();
    allColonys.insert(std::make_pair(nameColony, this));
}

//Очищення динамічної пам'яті 
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

//Ініціалізація верктору вказивників на своїх мінійонів
std::vector<Minion*> Colony::minionAddresses;


void Colony::createMinion(Point coordinate)
{
    Minion* newMinion = new Minion(coordinate, this);
    minionAddresses.push_back(newMinion);
    colonyAddresses.push_back(newMinion);
    ++MinionSettings::countMiniones;
}

//Початок симуляції життя
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

        render();
        std::this_thread::sleep_for(std::chrono::seconds(1));
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


//Save - Load модуль
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
