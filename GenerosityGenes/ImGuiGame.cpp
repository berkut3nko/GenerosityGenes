#include "primary_utilities.hpp"
#include "renderData.hpp"
#include "settings.hpp"


//Connection
char tempServerIpAdress[64] = "localhost";
char tempServerPort[5];
sf::IpAddress serverIpAdress;
unsigned short serverPort;
char userName[64];
NetworkClient netConnection;
vector<string> namesVec;
bool isConnected = false;
bool isServerOpened = false;

//ImGUI 
char addColonyName[64];
int firstLayer;
int secondLayer;
string colonyNames;
string isGameStoped;
int spawnerSize = 0;

std::stringstream  ss;

int selectedColony = -1;
editorMode cursorState = empty;

void ColonyListUpdate()
{
    colonyNames = "";
    for (std::pair<string, shared_ptr<Colony>> colony : allColonies)
    {
        colonyNames += colony.first + '\0';
    }
}

void imGui()
{

    ImGui::Begin("Game control");

    ImGui::BeginTabBar("Controler"); {
        if (ImGui::BeginTabItem("Setting"))
        {
            selectedColony = -1;
            ImGui::SetNextItemWidth(150.f);
            ImGui::DragFloat("camera zoom scale", &zoomScale, 0.1f, 1.0f, 50.0f, "%.2f");
            ImGui::SetNextItemWidth(150.f);
            ImGui::DragFloat("camera move speed", &cameraMoveSpeed, 0.01f, 0.05f, 0.5f, "%.2f");
            ImGui::SetNextItemWidth(150.f);
            ImGui::DragFloat("zoom animation quality", &animation_smoothness, 1.0f, 10.0f, 100.0f, "%.0f");
            if (ImGui::Button("Stop game", ImVec2(140, 30)))
            {
                isStoped = !isStoped;
            }
            isGameStoped = "Game is stoped: ";
            if (isStoped)
                isGameStoped += "true";
            else isGameStoped += "false";
            ImGui::Text(isGameStoped.c_str());
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Colony control"))
        {
            ImGui::BeginGroup();
            if (ImGui::Button("Create new colony")) {

                allColonies.insert(std::make_pair(addColonyName, make_shared<Colony>(firstLayer, secondLayer, addColonyName)));
                allColonies[addColonyName]->createMinion();
                ColonyListUpdate();
            }
            ImGui::SetNextItemWidth(200.f);
            ImGui::InputText("colony name", addColonyName, 64);
            ImGui::SetNextItemWidth(150.f);
            ImGui::DragInt("first layer size", &firstLayer, 1, 10, 50, "%d");
            ImGui::SetNextItemWidth(150.f);
            ImGui::DragInt("second layer size", &secondLayer, 1, 10, 50, "%d");
            ss.str("");
            ss << Colony::minionAddresses.size();
            ImGui::Text(string("Population size:" + ss.str()).c_str());
            ImGui::EndGroup();
            ImGui::BeginGroup();
            ImGui::Combo("All colonies", &selectedColony, colonyNames.c_str(), static_cast<int>(allColonies.size()));
            ss.str("");
            if (selectedColony != -1) {
                spawnerSize = static_cast<int>(next(allColonies.begin(), selectedColony)->second->colonyMinSize);
                ss << next(allColonies.begin(), selectedColony)->second->sizeColony;
                ImGui::Text(string("Colony size:" + ss.str() + " and neural network size:" + std::to_string(next(allColonies.begin(), selectedColony)->second->getNeuronsCount().first) + ' ' + std::to_string(next(allColonies.begin(), selectedColony)->second->getNeuronsCount().second)).c_str());
                if (ImGui::Button("Create minion", ImVec2(140, 30)))
                {
                    next(allColonies.begin(), selectedColony)->second->createMinion();
                }
                ImGui::SameLine();
                if(next(allColonies.begin(), selectedColony)->second->hasSpawner == false)
                    if (ImGui::Button("Create Spawner", ImVec2(140, 30)))
                {
                        allActiveSpawners.insert(std::make_pair(next(allColonies.begin(), selectedColony)->second, make_shared<Spawner>(next(allColonies.begin(), selectedColony)->second, next(allColonies.begin(), selectedColony)->second->colonyMinSize)));
                        next(allColonies.begin(), selectedColony)->second->colonyMinSize = spawnerSize;
                }
                ImGui::SameLine();
                if(ImGui::DragInt("Spawner Min Population", &spawnerSize, 1, 0, 100, "%d", ImGuiSliderFlags_AlwaysClamp))
                    next(allColonies.begin(), selectedColony)->second->colonyMinSize = spawnerSize;

                ImGui::Text("Neural network coefficients:");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_AttackEnemy", &(*next(allColonies.begin(), selectedColony)->second).coef_AttackEnemy, 0.05f, -1.f, 1.f, "%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_AttackTeam", &(*next(allColonies.begin(), selectedColony)->second).coef_AttackTeam, 0.05f, -1.f, 1.f, "%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_Border", &(*next(allColonies.begin(), selectedColony)->second).coef_Border, 0.05f, -1.f, 1.f, "%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_Born", &(*next(allColonies.begin(), selectedColony)->second).coef_Born, 0.05f, -1.f, 1.f, "%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_Eat", &(*next(allColonies.begin(), selectedColony)->second).coef_Eat, 0.05f, -1.f, 1.f, "%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_Protection", &(*next(allColonies.begin(), selectedColony)->second).coef_Protection, 0.05f, -1.f, 1.f, "%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_SpawnerEnemy", &(*next(allColonies.begin(), selectedColony)->second).coef_SpawnerEnemy, 0.05f, -1.f, 1.f, "%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_SpawnerTeam", &(*next(allColonies.begin(), selectedColony)->second).coef_SpawnerTeam, 0.05f, -1.f, 1.f, "%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_Synthesis", &(*next(allColonies.begin(), selectedColony)->second).coef_Synthesis, 0.05f, -1.f, 1.f, "%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_TeamClose", &(*next(allColonies.begin(), selectedColony)->second).coef_TeamClose, 0.05f, -1.f, 1.f, "%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_EnemyClose", &(*next(allColonies.begin(), selectedColony)->second).coef_EnemyClose, 0.05f, -1.f, 1.f, "%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_TeamSpawnerClose", &(*next(allColonies.begin(), selectedColony)->second).coef_TeamSpawnerClose, 0.05f, -1.f, 1.f, "%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_EnemySpawnerClose", &(*next(allColonies.begin(), selectedColony)->second).coef_EnemySpawnerClose, 0.05f, -1.f, 1.f, "%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_EatClose", &(*next(allColonies.begin(), selectedColony)->second).coef_EatClose, 0.05f, -1.f, 1.f, "%.2f");
                if (ImGui::Button("Delete colony"))
                {
                    for (const std::pair<Point,shared_ptr<Colony>> area : colonyArea)
                    {
                        if (area.second != nullptr)
                            if (area.second == next(allColonies.begin(), selectedColony)->second)
                            {
                                colonyArea.erase(area.first);
                            }
                    }
                    allColonies.erase(next(allColonies.begin(), selectedColony)->first);
                    ColonyListUpdate();
                    selectedColony = -1;
                }
            }
            ImGui::EndGroup();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Game Editor"))
        {
            ImGui::SetNextItemWidth(150.f);
            ImGui::DragFloat("Eat cost coefficient", &MinionSettings::eat_cost, 0.1f, 0.1f, 5.0f, "%.1f");
            if (ImGui::Button("None state", ImVec2(140, 30)))
            {
                cursorState = empty;
            }
            if (ImGui::Button("Add fruit", ImVec2(70, 30)))
            {
                cursorState = addFruit;
            }
            ImGui::SameLine();
            if (ImGui::Button("Add border", ImVec2(70, 30)))
            {
                cursorState = addBorder;
            }
            ImGui::SameLine();
            if (ImGui::Button("Fill air", ImVec2(70, 30)))
            {
                cursorState = fillAir;
            }
            ImGui::Combo("All colonies", &selectedColony, colonyNames.c_str(), static_cast<int>(allColonies.size()));
            ss.str("");
            if (selectedColony != -1)
            {
                if (ImGui::Button("Create minion", ImVec2(120, 30)))
                {
                    cursorState = addMinion;
                }
            }
            ImGui::EndTabItem();
        }
        else
        {
            cursorState = empty;
        }
        if (ImGui::BeginTabItem("Server connection"))
        {
            ImGui::BeginGroup();
            ImGui::SetNextItemWidth(200.f);
            if (!isConnected && !isServerOpened) {
                ImGui::InputText("IP adress", tempServerIpAdress, 64);
                ImGui::SetNextItemWidth(200.f);
                ImGui::InputText("Port", tempServerPort, 6);
                ImGui::SetNextItemWidth(200.f);
                ImGui::InputText("Player name", userName, 64);
                if (ImGui::Button("Connect", ImVec2(100, 50)))
                {
                    serverIpAdress = sf::IpAddress(tempServerIpAdress);
                    serverPort = static_cast<unsigned short>(std::stoul(tempServerPort, NULL, 0));
                    if (netConnection.init() == sf::Socket::Done)
                        isConnected = true;
                    netConnection.registerOnServer(serverIpAdress, serverPort, userName);
                    netConnection.receiveConnectedClientsNames(namesVec);
                }
                ImGui::SameLine();
                if (ImGui::Button("OpenServer", ImVec2(140, 50)))
                {
                    if (netServer.init() == sf::Socket::Done)
                        isServerOpened = true;
                }
            }
            else
            {
                if (isConnected)
                {
                    ImGui::Text(("Connection: " + string(userName) + "\nTo server: " + tempServerIpAdress + ':' + tempServerPort + " - Succeed").c_str());
                }
                else
                    if (isServerOpened)
                    {
                        ImGui::Text(("Address: " + netServer.getAdress().toString() + ':' + std::to_string(netServer.getPort()) + " - Succeed").c_str());
                    }
            }

            ImGui::EndGroup();
            ImGui::EndTabItem();
        }
    }
    ImGui::EndTabBar();
    ImGui::End();
}