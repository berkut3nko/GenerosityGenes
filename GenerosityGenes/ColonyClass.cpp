#include "primary_utilities.hpp"

//Це конснтруктор класса Колонія
Colony::Colony(size_t neuronsCountFirst, size_t neuronsCountSecond, std::string name) :
    nameColony(name),
    _neuronsCount({ neuronsCountFirst,neuronsCountSecond }),
    colonyColor(sf::Color(rand() % 256, rand() % 256, rand() % 128 + 128, 255)),
    bestMinionBrain({ {MinionSettings::minionInputs + sizeMemmory, _neuronsCount.first},
          {_neuronsCount.first, _neuronsCount.second},
          {_neuronsCount.second,MinionSettings::minionOutputs + sizeMemmory} }, nameColony)
{
    for (auto colony : allColonies)
    {
        colonyRelations.insert(std::make_pair(colony.second, 0.5f));
        colony.second->colonyRelations.insert(std::make_pair(this, 0.5f));
    }
    coefInitialization();
}

Colony::~Colony()
{
    for (auto minion : colonyAddresses)
        minion->kill();
}

//Ініціалізація верктору вказивників на своїх мінійонів
std::map<size_t, shared_ptr<Minion>> Colony::minionAddresses;
//Унікальні індефікатори
size_t Colony::nextMinionID = 0;

void Colony::coefInitialization()
{
    /*coef_Synthesis = (double(rand()%200) / 100.0) - 1.0;
    coef_Protection = (double(rand() % 200) / 100.0) - 1.0;
    coef_Born = (double(rand() % 200) / 100.0) - 1.0;
    coef_AttackEnemy = (double(rand() % 200) / 100.0) - 1.0;
    coef_Eat = (double(rand() % 200) / 100.0) - 1.0;
    coef_AttackTeam = (double(rand() % 200) / 100.0) - 1.0;
    coef_Border = (double(rand() % 200) / 100.0) - 1.0;
    coef_SpawnerEnemy = (double(rand() % 200) / 100.0) - 1.0;
    coef_SpawnerTeam = (double(rand() % 200) / 100.0) - 1.0;
    srand(static_cast<unsigned int>(time(NULL)));*/
    coef_Synthesis = 0.1f;
    coef_Protection = 0.1f;
    coef_Born = 0.6f;
    coef_AttackEnemy = 0.7f;
    coef_Eat = 1.0f;
    coef_AttackTeam = -0.3f;
    coef_Border = -1.0f;
    coef_SpawnerEnemy = 0.5f;
    coef_SpawnerTeam = -1.0f;
    coef_TeamClose = 0.3f;
    coef_EnemyClose = 0.5f;
    coef_TeamSpawnerClose = -0.1f;
    coef_EnemySpawnerClose = 0.4f;
    coef_EatClose = 0.6f;
}

//
// Saves colony data to file SaveColonies_ game version .save
//
void Colony::SaveColonies(string version)
{
    std::ofstream allColoniesFile("SaveColonies_" + version + ".save");
    if (allColoniesFile.is_open()) {
        for (auto colony : allColonies)
        {
            colony.second->bestMinionBrain.NeuralNetworkWay = colony.first;
            colony.second->SaveColony();
            allColoniesFile << colony.first << "\t";
            allColoniesFile 
                << colony.second->hasSpawner << '\t'            << colony.second->colonyMinSize << '\t'
                << colony.second->_neuronsCount.first << '\t'   << colony.second->_neuronsCount.second << '\t'
                << colony.second->coef_AttackEnemy << '\t'      << colony.second->coef_AttackTeam << '\t'
                << colony.second->coef_Border << '\t'           << colony.second->coef_Born << '\t'
                << colony.second->coef_Eat << '\t'              << colony.second->coef_EatClose << '\t'
                << colony.second->coef_EnemyClose << '\t'       << colony.second->coef_EnemySpawnerClose << '\t'
                << colony.second->coef_Protection << '\t'       << colony.second->coef_SpawnerEnemy << '\t'
                << colony.second->coef_SpawnerTeam << '\t'      << colony.second->coef_Synthesis << '\t'
                << colony.second->coef_TeamClose << '\t'        << colony.second->coef_TeamSpawnerClose << '\n';
        }
        allColoniesFile.close();
    }
}
void Colony::LoadColonies(string version)
{
    std::ifstream LoadAllColoniesFile("SaveColonies_" + version + ".save");
    if (LoadAllColoniesFile.is_open()) {
        string name, colonyName;
        bool _colonyHasSpawner = false;
        size_t _colonySize;
        std::pair<int, int> neuronsCount{ 32,28 };


        shared_ptr<Colony> temp;
        while (std::getline(LoadAllColoniesFile, name)) {
            std::istringstream iss(name);
            string value_str;
            std::getline(iss, value_str, '\t');
            colonyName = value_str;
            std::getline(iss, value_str, '\t');
            _colonyHasSpawner = value_str == "1" ? true : false;
            std::getline(iss, value_str, '\t');
            _colonySize = std::stoi(value_str);

            srand(rand() % 241);
            std::getline(iss, value_str, '\t');
            neuronsCount.first = stoi(value_str);
            std::getline(iss, value_str, '\t');
            neuronsCount.second = stoi(value_str);
            temp = make_shared<Colony>(neuronsCount.first, neuronsCount.second, colonyName);
            temp->LoadColony();
            allColonies.insert({ colonyName,temp });
            temp->hasSpawner = false; //
            temp->colonyMinSize = _colonySize;
            //if (_colonyHasSpawner)
                //allActiveSpawners.insert(std::make_pair(allColonies[colonyName], make_shared<Spawner>(temp, _colonySize)));
            std::getline(iss, value_str, '\t');
            temp->coef_AttackEnemy = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_AttackTeam = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_Border = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_Born = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_Eat = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_EatClose = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_EnemyClose = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_EnemySpawnerClose = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_Protection = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_SpawnerEnemy = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_SpawnerTeam = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_Synthesis = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_TeamClose = std::stof(value_str);
            std::getline(iss, value_str, '\t');
            temp->coef_TeamSpawnerClose = std::stof(value_str);

        }
        LoadAllColoniesFile.close();
    }
}
//Save - Load модуль
void Colony::SaveColony()
{
    bestMinionBrain.SaveAI();
}

void Colony::LoadColony()
{
    bestMinionBrain.LoadAI();
}

void Colony::SaveMiniones(string version)
{
    std::ofstream file(version + "_miniones");
    if (!file.is_open()) {

        return;
    }
    for (const auto& colony : allColonies)
        for (const auto& minion : colony.second->colonyAddresses)
        {
            file << minion->SaveMe() << '\n';
        }

    file.close();
}

void Colony::LoadMiniones(string version)
{
    std::ifstream file(version + "_miniones");
    if (!file.is_open()) {

        return;
    }

    string dataMinion;
    shared_ptr<Colony> colonyAddress;
    Point pos;
    double fat, hunger;
    while (std::getline(file, dataMinion))
    {
        std::istringstream iss(dataMinion);
        string value_str;

        std::getline(iss, value_str, ' ');
        if (allColonies.count(value_str) > 0) {
            colonyAddress = allColonies[value_str];
        }
        else {
            if (allColonies.size() > 0)
                colonyAddress = allColonies.begin()->second;
            else
                return; // call default constructor (dev tip)
        }

        std::getline(iss, value_str, ' ');
        pos.x = stoull(value_str);

        std::getline(iss, value_str, ' ');
        pos.y = stoull(value_str);

        std::getline(iss, value_str, ' ');
        hunger = stod(value_str);

        std::getline(iss, value_str, ' ');
        fat = stod(value_str);

        shared_ptr<Minion> newMinion = make_shared<Minion>(colonyAddress, pos, fat, hunger);
        minionAddresses.insert({ newMinion->ID,newMinion });
    }

    file.close();
}