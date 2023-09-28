
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
    Colony myFirstColony(/*кількість нейронів у прихованому шарі*/32,/*назва колонії(для збереження)*/ "testColony1");

    myFirstColony.createMinion();

    myFirstColony.createMinion();

    myFirstColony.createMinion();
    //Spawner spawner(&myFirstColony, 5, { 4,6 });
    //myFirstColony.createMinion(/*Кордината створення мінійона*/{2,1});



    Colony mySecondColony(32, "testColony2");

    mySecondColony.createMinion();

    mySecondColony.createMinion();


    //Colony::LoadMiniones(version);

    Colony::startLife();


    //збереження гри
    Colony::SaveMiniones(version);
    return 0;
}