
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
    Colony myFirstColony(/*������� ������� � ����������� ���*/32,/*����� �����(��� ����������)*/ "testColony1");

    myFirstColony.createMinion();

    myFirstColony.createMinion();

    myFirstColony.createMinion();
    //Spawner spawner(&myFirstColony, 5, { 4,6 });
    //myFirstColony.createMinion(/*��������� ��������� ������*/{2,1});



    Colony mySecondColony(32, "testColony2");

    mySecondColony.createMinion();

    mySecondColony.createMinion();


    //Colony::LoadMiniones(version);

    Colony::startLife();


    //���������� ���
    Colony::SaveMiniones(version);
    return 0;
}