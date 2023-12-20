
#include "worldMap.hpp"
#include "primary_utilities.hpp"
std::vector<std::vector<object>> worldMap(sizeWorldX, std::vector<object>(sizeWorldY));

//chatNeuro.hpp - ��������� ��� ������� �����
//primary_utilities.hpp - C����� � ���� �������� 
//primary_utilities.cpp - ��������� ������� � ��� (dev tip) - ����� �������� �� ����� ������
//MinionLogic.cpp - ��������� ���������� ��� ������ � �������� �������, ������ �����
//worldMap.hpp - ��'��� ����� 
//include.hpp  - ������� ������ �� ���������� �������


string version = "v0.0.3";

int main()
{
    srand(static_cast<unsigned int>(time(NULL)));
    worldInitialization();
    //imGuiInit();
        // ����������� ������
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


    //���������� ���
    Colony::SaveColonies(version);
    Colony::SaveMiniones(version);


    return 0;
}