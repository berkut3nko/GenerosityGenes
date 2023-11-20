
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
    InitializationRender();
    //imGuiInit();

    // ����������� �����
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

    //���������� ���
    Colony::SaveMiniones(version);
    return 0;
}