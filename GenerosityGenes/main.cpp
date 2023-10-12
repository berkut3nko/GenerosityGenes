
#include "worldMap.hpp"
#include "primary_utilities.hpp"

//chatNeuro.hpp - ��������� ��� ������� �����
//primary_utilities.hpp - C����� � ���� �������� 
//primary_utilities.cpp - ��������� ������� � ��� (dev tip) - ����� �������� �� ����� ������
//MinionLogic.cpp - ��������� ���������� ��� ������ � �������� �������, ������ �����
//worldMap.hpp - ��'��� ����� 
//include.hpp  - ������� ������ �� ���������� �������


string version = "v0.0.1";

int main()
{
    std::srand(static_cast<unsigned int>(time(NULL)));
    worldInitialization();
    InitializationRender();

    // ����������� �����
    Colony myFirstColony(50, 36, "testColony1");
    myFirstColony.createMinion(/*��������� ��������� ������*/{ 2,1 });
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
    //���������� ���
    //Colony::SaveMiniones(version);
    return 0;
}