
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
    InitializationRender();
    //imGuiInit();

    // ініціалізація колонії
    Colony myFirstColony(32,24,"highBrain");
    myFirstColony.LoadColony();
    Colony::LoadMiniones(version);
    //myFirstColony.createMinion();
    //myFirstColony.createMinion();
    //myFirstColony.createMinion();

    //Colony mySecondColony(16, 12, "Brainless");
    //mySecondColony.createMinion();
    //Colony::LoadMiniones(version);
    //Spawner spawner2(&mySecondColony, 5);
    Spawner spawner1(&myFirstColony, 9);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    Colony::startLife();

    myFirstColony.SaveColony();

    //збереження гри
    Colony::SaveMiniones(version);
    return 0;
}