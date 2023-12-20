
#include "worldMap.hpp"
#include "primary_utilities.hpp"
std::vector<std::vector<object>> worldMap(sizeWorldX, std::vector<object>(sizeWorldY));

//chatNeuro.hpp - Інтерфейс для нейроної мережі
//primary_utilities.hpp - Cмітник зі всіма методами 
//primary_utilities.cpp - реалізація більшості з них (dev tip) - треба рознести по різним файлам
//MinionLogic.cpp - реалізація інтерфейсу для роботи з нейроною мережою, ігрова логіка
//worldMap.hpp - об'ява карти 
//include.hpp  - швидкий доступ до підключаємих бібліотек


string version = "v0.0.3";

int main()
{
    srand(static_cast<unsigned int>(time(NULL)));
    worldInitialization();
    //imGuiInit();
        // ініціалізація колоній
    Colony::LoadColonies(version);
    Colony::LoadMiniones(version);
    if (allColonies.find("highBrain") == allColonies.end())Colony first(32, 24, "highBrain");
    if (allColonies.find("newBorn") == allColonies.end())Colony second(50, 28, "newBorn");

    //Spawner spawner2(&mySecondColony, 5);

    Spawner spawner1(allColonies["highBrain"], 9);
    Spawner spawner2(allColonies["newBorn"], 4);


    std::this_thread::sleep_for(std::chrono::milliseconds(50));


    InitializationRender();
    Colony::startLife();


    //збереження гри
    Colony::SaveColonies(version);
    Colony::SaveMiniones(version);


    return 0;
}