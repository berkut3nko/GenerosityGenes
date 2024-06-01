
#include "worldMap.hpp"
#include "primary_utilities.hpp"
#include "settings.hpp"
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
    if(isTestVersion)loadTextures();

    Colony::LoadColonies(version);
    colonyArea.clear();



    InitializationRender();
    Colony::startLife();


    //���������� ���
    Colony::SaveColonies(version);
    Colony::SaveMiniones(version);


    return 0;
}