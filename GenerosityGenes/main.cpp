
#include "worldMap.hpp"
#include "primary_utilities.hpp"

//chatNeuro.hpp - Інтерфейс для нейроної мережі
//primary_utilities.hpp - Cмітник зі всіма методами 
//primary_utilities.cpp - реалізація більшості з них (dev tip) - треба рознести по різним файлам
//MinionLogic.cpp - реалізація інтерфейсу для роботи з нейроною мережою, ігрова логіка
//worldMap.hpp - об'ява карти 
//include.hpp  - швидкий доступ до підключаємих бібліотек


string version = "v0.0.1";

int main()
{
    std::srand(static_cast<unsigned int>(time(NULL)));
    worldInitialization();
    InitializationRender();

    // ініціалізація колонії
    Colony myFirstColony(50, 36, "testColony1");
    myFirstColony.createMinion(/*Кордината створення мінійона*/{ 2,1 });
    myFirstColony.createMinion();

    myFirstColony.createMinion();

    myFirstColony.createMinion();


    //Colony mySecondColony(32, 24, "testColony2");

    //mySecondColony.createMinion();

    ////Colony::LoadMiniones(version);
    //Spawner spawner1(&mySecondColony, 5);
    Spawner spawner2(&myFirstColony, 9);

    Colony::startLife();

    myFirstColony.SaveColony();
    //збереження гри
    //Colony::SaveMiniones(version);
    return 0;
}