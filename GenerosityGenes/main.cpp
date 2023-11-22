
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
        // ініціалізація колоній
    Colony::LoadColonies(version);

    if(allColonies.find("highBrain") == allColonies.end())Colony first(32, 24, "highBrain");
    if(allColonies.find("newBorn") == allColonies.end())Colony first(50, 28, "newBorn");

    //Spawner spawner2(&mySecondColony, 5);

    Spawner spawner1(allColonies["highBrain"], 9);
    Spawner spawner2(allColonies["newBorn"], 4);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    Colony::startLife();
    Colony::SaveColonies(version);

    //збереження гри
    Colony::SaveMiniones(version);


    return 0;
}
/*
std::ofstream file(filename);
    for (const auto& row : weights_) {
        for (const auto& value : row) {
            file << value << ",";
        }
        file << "\n";
    }
    file.close();
    
    
    std::ifstream file(filename);
    string line;
    weights_.clear();
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        string value_str;
        std::vector<double> row;
        while (std::getline(iss, value_str, ',')) {
            row.push_back(std::stod(value_str));
        }
        weights_.push_back(row);
    }
    file.close();
*/