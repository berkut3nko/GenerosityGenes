
#include "worldMap.hpp"
#include "primary_utilities.hpp"
#include "settings.hpp"
std::vector<std::vector<object>> worldMap(sizeWorldX, std::vector<object>(sizeWorldY));

//chatNeuro.hpp - Інтерфейс для нейроної мережі
//primary_utilities.hpp - Cмітник зі всіма методами 
//primary_utilities.cpp - реалізація більшості з них (dev tip) - треба рознести по різним файлам
//MinionLogic.cpp - реалізація інтерфейсу для роботи з нейроною мережою, ігрова логіка
//worldMap.hpp - об'ява карти 
//include.hpp  - швидкий доступ до підключаємих бібліотек




int main()
{
    srand(static_cast<unsigned int>(time(NULL)));
    worldInitialization();
    if(isTestVersion)loadTextures();

    Colony::LoadColonies(version);
    colonyArea.clear();



    InitializationRender();
    Colony::startLife();


    //збереження гри
    Colony::SaveColonies(version);
    Colony::SaveMiniones(version);


    return 0;
}