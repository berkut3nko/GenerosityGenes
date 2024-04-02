
#include "worldMap.hpp"
#include "primary_utilities.hpp"
std::vector<std::vector<object>> worldMap(sizeWorldX, std::vector<object>(sizeWorldY));

//chatNeuro.hpp - ��������� ��� ������� �����
//primary_utilities.hpp - C����� � ���� �������� 
//primary_utilities.cpp - ��������� ������� � ��� (dev tip) - ����� �������� �� ����� ������
//MinionLogic.cpp - ��������� ���������� ��� ������ � �������� �������, ������ �����
//worldMap.hpp - ��'��� ����� 
//include.hpp  - ������� ������ �� ���������� �������




int main()
{
    srand(static_cast<unsigned int>(time(NULL)));
    worldInitialization();
    //imGuiInit();
        // ����������� ������
    Colony::LoadColonies(version);
    Colony::LoadMiniones(version);


    if (allColonies.find("highBrain") == allColonies.end())
        allColonies.insert({"highBrain", make_shared<Colony>(32, 24, "highBrain") });
    srand(static_cast<unsigned int>(time(NULL)));
    if (allColonies.find("newBorn") == allColonies.end())
        allColonies.insert({ "newBorn", make_shared<Colony>(50, 28, "newBorn") });

    //Spawner spawner2(&mySecondColony, 5);
    if(allColonies.find("highBrain") != allColonies.end())
        allActiveSpawners.insert(std::make_pair(allColonies["highBrain"], make_shared<Spawner>(allColonies["highBrain"], 9)));
    
    if (allColonies.find("newBorn") != allColonies.end())
        allActiveSpawners.insert(std::make_pair(allColonies["newBorn"], make_shared<Spawner>(allColonies["newBorn"], 4)));
    

    std::this_thread::sleep_for(std::chrono::milliseconds(50));


    InitializationRender();
    Colony::startLife();


    //���������� ���
    Colony::SaveColonies(version);
    Colony::SaveMiniones(version);


    return 0;
}