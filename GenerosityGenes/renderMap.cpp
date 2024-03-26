#include "primary_utilities.hpp"
#include "settings.hpp"

struct screen
{
    size_t x, y;
};
float findMultiplierSize(screen windowSize)
{
    return static_cast<float>((windowSize.x / sizeWorldX >= windowSize.y / sizeWorldY) ? windowSize.y / sizeWorldY : windowSize.x / sizeWorldX);
}
screen myScreen = { 1980, 1080 };
float multiplicator = findMultiplierSize(myScreen);
sf::RenderWindow window(sf::VideoMode(static_cast<size_t>(multiplicator * sizeWorldX), static_cast<size_t>(multiplicator * sizeWorldY)), "GenerosityGenes");
bool isMainWindowOpen = true;
sf::Event event;


sf::Color defaultColor(Types type)
{
    switch (type)
    {
    case minion:
        return dC::dead;
        break;
    case fruit:
        return dC::fruit;
        break;
    case border:
        return dC::border;
        break;
    case air:
        return dC::air;
        break;
    default:
        return dC::air;
        break;
    }
}

sf::RenderWindow window_MinionBrain(sf::VideoMode(980, 980), "Minion brain");
bool isWindowMinionBrain = false;

sf::View mainCamera;
size_t animationZoom = 0;
sf::Vector2f cameraCenterPos = sf::Vector2f(multiplicator * sizeWorldX / 2, multiplicator * sizeWorldY / 2);

sf::Vector2f mousePos;

sf::Vector2f tempPos;
Point mouseLocale;
float animationZoomSin;
double cosHealth; 
bool zoom = false;


sf::Vector2f zoomSize;
sf::RectangleShape spawnerShape(sf::Vector2f(multiplicator, multiplicator));
sf::RectangleShape borderShape(sf::Vector2f(multiplicator, multiplicator));
sf::CircleShape fruitShape(static_cast<float>(multiplicator / 2.5));
sf::RectangleShape tempAreaShape(sf::Vector2f(multiplicator, multiplicator));
sf::RectangleShape airShape(sf::Vector2f(multiplicator* sizeWorldX, multiplicator* sizeWorldY));
sf::RectangleShape globalBorderShape[4]=
{
    sf::RectangleShape(sf::Vector2f(multiplicator* sizeWorldX, multiplicator*2)),
    sf::RectangleShape(sf::Vector2f(multiplicator* sizeWorldX, multiplicator*2)),
    sf::RectangleShape(sf::Vector2f(multiplicator*2, multiplicator* sizeWorldY)),
    sf::RectangleShape(sf::Vector2f(multiplicator*2, multiplicator* sizeWorldY))
};
sf::CircleShape tempShapeMinion(static_cast<float>(multiplicator / 2.5));
sf::Color tempColor;
sf::CircleShape neuronShape(3.f);
sf::Vertex weightShape[2];

void ColonyListUpdate();

Minion* minionToShowBrain = nullptr;
void InitializationRender()
{
    ColonyListUpdate();
    ImGui::SFML::Init(window);

    tempShapeMinion.setOutlineThickness((float)multiplicator/10);

    spawnerShape.setOutlineThickness((float)multiplicator / 10);
    spawnerShape.setOutlineColor(dC::dead);

    fruitShape.setFillColor(dC::fruit);
    fruitShape.setOutlineThickness(0);

    airShape.setFillColor(dC::air);
    airShape.setPosition(sf::Vector2f(0, 0));
    borderShape.setFillColor(dC::border);
    globalBorderShape[0].setFillColor(dC::border);
    globalBorderShape[0].setPosition(sf::Vector2f(0,0));
    globalBorderShape[1].setFillColor(dC::border);
    globalBorderShape[1].setPosition(sf::Vector2f(0, multiplicator * (sizeWorldY-2)));
    globalBorderShape[2].setFillColor(dC::border);
    globalBorderShape[2].setPosition(sf::Vector2f(0, 0));
    globalBorderShape[3].setFillColor(dC::border);
    globalBorderShape[3].setPosition(sf::Vector2f(multiplicator * (sizeWorldX-2), 0));


    neuronShape.setFillColor(sf::Color::Blue);

    window_MinionBrain.setVisible(false);
}
sf::Clock deltaClock;
enum editorMode
{
    addFruit,
    addBorder,
    fillAir,
    addMinion,
    empty
};

editorMode cursorState = empty;

//Connection
char tempServerIpAdress[64];
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
int spawnerSize = 3;
static int selectedColony = -1;
std::stringstream  ss;
void eraseArea(Point place)
{
    if (worldMap[place.x][place.y].type == minion) {
        if (worldMap[place.x][place.y].minionAddress != nullptr)
            worldMap[place.x][place.y].minionAddress->kill();}
        else if (worldMap[place.x][place.y].type == fruit)
            poolOfFruits.erase(place);
        else if (worldMap[place.x][place.y].type == border)
            poolOfBorders.erase(place);

    worldMap[place.x][place.y].type = air;
    return;
}
void ColonyListUpdate()
{
    colonyNames = "";
    for (auto colony : allColonies)
    {
        colonyNames += colony.first + '\0';
    }
}
void imGui()
{

        ImGui::Begin("Game control");

        ImGui::BeginTabBar("Controler"); {
            if(ImGui::BeginTabItem("Setting"))
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
            if(ImGui::BeginTabItem("Colony control"))
            {
                ImGui::BeginGroup();
                if (ImGui::Button("Create new colony")) {
                    new Colony(firstLayer,secondLayer,addColonyName);
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
                ImGui::Combo("All colonies",&selectedColony,colonyNames.c_str(), static_cast<int>(allColonies.size()));
                ss.str("");
                if (selectedColony != -1) {
                    ss << next(allColonies.begin(), selectedColony)->second->sizeColony;
                    ImGui::Text(string("Colony size:" + ss.str()).c_str());
                    if (ImGui::Button("Create minion", ImVec2(140, 30)))
                    {
                        next(allColonies.begin(), selectedColony)->second->createMinion();
                    }

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
                        for (const auto area : colonyArea)
                        {
                            if (worldMap[area.x][area.y].minionAddress != nullptr)
                            if(worldMap[area.x][area.y].minionAddress->myColony == next(allColonies.begin(), selectedColony)->second)
                            {
                                colonyArea.erase(area);
                            }
                        }
                        for (auto minion : next(allColonies.begin(), selectedColony)->second->colonyAddresses)
                        {
                            delete minion;
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
                if(ImGui::Button("Add fruit", ImVec2(70, 30)))
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
                        serverPort = std::strtoul(tempServerPort,NULL,0);
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
void render()
{
    if (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::Resized:
                cameraCenterPos = sf::Vector2f(multiplicator * sizeWorldX / 2, multiplicator * sizeWorldY / 2);
                break;
            default:
                break;
            }
        }
        ImGui::SFML::Update(window, deltaClock.restart());
        imGui();
        window.clear();

        //Фон Карти
        window.draw(airShape);

        for (const auto area : colonyArea)
        {
            if (worldMap[area.x][area.y].minionAddress != nullptr) {
                tempColor = (worldMap[area.x][area.y].minionAddress->myColony->colonyColor);
                tempAreaShape.setPosition(sf::Vector2f(area.x * multiplicator, area.y * multiplicator));
                tempColor.a = 100;
                tempAreaShape.setFillColor(tempColor);
                window.draw(tempAreaShape);
            }
        }
        for (const auto colony : allColonies)
        {

            for (const auto minion : colony.second->colonyAddresses)
            {
                tempShapeMinion.setPosition(sf::Vector2f(multiplicator * minion->position.x, multiplicator * minion->position.y));

                if (minion->IsDead) {
                    tempShapeMinion.setFillColor(dC::dead);
                    tempShapeMinion.setOutlineColor(sf::Color::Black);
                }
                else {

                    tempShapeMinion.setFillColor(minion->myColony->colonyColor);
                    if (minion->IsSynthesis)
                    {
                        tempShapeMinion.setOutlineColor(sf::Color::Green);
                    }
                    else if (minion->IsProtection)
                    {
                        tempShapeMinion.setOutlineColor(sf::Color::Blue);
                    }
                    else if(!minion->IsDead)
                    {
                        cosHealth = cos(minion->health * PI/2);
                        tempShapeMinion.setOutlineColor(sf::Color(static_cast<sf::Uint8>(255 * cosHealth), 0 , 0, 255));
                    }
                }
                window.draw(tempShapeMinion);
            }
        }

        for (const auto fruit : poolOfFruits)
        {
            fruitShape.setPosition(sf::Vector2f(multiplicator * fruit.x, multiplicator * fruit.y));
            window.draw(fruitShape);
        }
        for (const auto border : poolOfBorders)
        {
            borderShape.setPosition(sf::Vector2f(multiplicator * border.x, multiplicator * border.y));
            window.draw(borderShape);
        }

        for (std::pair<Colony*, Spawner*> spawner : allActiveSpawners)
        {
            spawnerShape.setFillColor(spawner.first->colonyColor);
            spawnerShape.setPosition(sf::Vector2f(multiplicator * spawner.second->spawnerPosition.x, multiplicator * spawner.second->spawnerPosition.y));
            window.draw(spawnerShape);

        }

        window.draw(globalBorderShape[0]);
        window.draw(globalBorderShape[1]);
        window.draw(globalBorderShape[2]);
        window.draw(globalBorderShape[3]);


        animationZoomSin = (sinf((animationZoom / (animation_smoothness * 2.0f)) * PI) * zoomScale) + 1;
        zoomSize = sf::Vector2f(multiplicator * sizeWorldX / (animationZoomSin),
                                multiplicator * sizeWorldY / (animationZoomSin));
        //if(sf::Keyboard::isKeyPressed(sf::Keyboard::C))
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if ((mousePos.x >= 0 && mousePos.y >= 0) && (mousePos.x <= multiplicator * sizeWorldX && mousePos.y <= multiplicator * sizeWorldY))
            {
                mouseLocale = Point{ (size_t)(mousePos.x / multiplicator) , (size_t)(mousePos.y / multiplicator) };
                if (cursorState != empty) //(dev tip)
                {
                    switch (cursorState) {
                    case addFruit:
                        eraseArea(mouseLocale);
                        worldMap[mouseLocale.x][mouseLocale.y].type = Types::fruit;
                        poolOfFruits.insert(mouseLocale);
                        break;
                    case fillAir:
                        eraseArea(mouseLocale);
                        break;
                    case addBorder:
                        eraseArea(mouseLocale);
                        worldMap[mouseLocale.x][mouseLocale.y].type = Types::border;
                        poolOfBorders.insert(mouseLocale);
                        break;
                    case addMinion:
                        eraseArea(mouseLocale);
                        next(allColonies.begin(), selectedColony)->second->createMinion(mouseLocale);
                        break;
                    default:
                        break;
                    }
                }
                else
                if (window_MinionBrain.isOpen()) {
                    if (worldMap[mouseLocale.x][mouseLocale.y].type == minion)
                    {
                        //демонстрація нейроної мережі
                        window_MinionBrain.setVisible(true);
                        window_MinionBrain.clear();
                        minionToShowBrain = worldMap[mouseLocale.x][mouseLocale.y].minionAddress;
                        isWindowMinionBrain = true;
                        for (size_t inputIt = 0; inputIt < MinionSettings::minionInputs + minionToShowBrain->myColony->sizeMemmory; ++inputIt)
                        {
                            neuronShape.setPosition(sf::Vector2f(0.f, 12.f * inputIt));
                            window_MinionBrain.draw(neuronShape);
                        }
                        for (size_t layersIt = 0; layersIt < minionToShowBrain->MyBrain.layers_.size(); ++layersIt)
                        {
                            for (size_t outputsIt = 0; outputsIt < minionToShowBrain->MyBrain.layers_[layersIt].output_size_; ++outputsIt)
                            {
                                neuronShape.setPosition(sf::Vector2f((250.f * (layersIt + 1)), ((MinionSettings::minionInputs - minionToShowBrain->MyBrain.layers_[layersIt].output_size_) / 2) * 12.f + 12.f * outputsIt));
                                for (size_t inputsIt = 0; inputsIt < minionToShowBrain->MyBrain.layers_[layersIt].input_size_; ++inputsIt)
                                {
                                    weightShape[0] = sf::Vertex(sf::Vector2f(250.f * layersIt + 1.5f, (((MinionSettings::minionInputs + minionToShowBrain->myColony->sizeMemmory) - minionToShowBrain->MyBrain.layers_[layersIt].input_size_) / 2) * 12.f + 12.f * inputsIt + 1.5f),
                                        sf::Color(0, static_cast<sf::Uint8>(255.0 * (1.0 / (1.0 + exp(minionToShowBrain->MyBrain.layers_[layersIt].weights()[inputsIt][outputsIt])))), 0, static_cast<sf::Uint8>(255 * (1 - (1 / (1 + exp(minionToShowBrain->MyBrain.layers_[layersIt].weights()[inputsIt][outputsIt])))))));
                                    weightShape[0].position.y += (layersIt != 0) ? -12.0f : 0.0f;
                                    weightShape[1] = sf::Vertex(sf::Vector2f(neuronShape.getPosition().x + 1.5f, neuronShape.getPosition().y + 1.5f),
                                        weightShape[0].color);

                                    window_MinionBrain.draw(weightShape, 3, sf::Lines);
                                }
                                window_MinionBrain.draw(neuronShape);
                            }
                        }
                        window_MinionBrain.display();

                    }
                    else
                    {
                        isWindowMinionBrain = false;
                        window_MinionBrain.setVisible(false);
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        if (!window_MinionBrain.isOpen())
        {
            isWindowMinionBrain = false;
        }
        if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            zoom=!zoom;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
            if (zoom)
            {
                if (animationZoom <= animation_smoothness)
                {
                    ++animationZoom;
                }

                mainCamera.setSize(zoomSize);
                mousePos = (window.mapPixelToCoords(sf::Mouse::getPosition(window)));
                //Якщо вийшло за межі квадрата
                tempPos = sf::Vector2f((abs(cameraCenterPos.x - mousePos.x) / myScreen.x), (abs(cameraCenterPos.y - mousePos.y) / myScreen.y));
                if (std::max({ tempPos.x,tempPos.y,(tempPos.x+ tempPos.y) * 0.6f }) > (cameraMoveRange / zoomScale) && !isWindowMinionBrain)
                    {
                        //Повільно рухати до миші
                        mousePos = sf::Vector2f(
                            ((1 - (cameraMoveSpeed / 5)) * cameraCenterPos.x) + ((cameraMoveSpeed / 5) * mousePos.x),
                            ((1 - (cameraMoveSpeed / 5)) * cameraCenterPos.y) + ((cameraMoveSpeed / 5) * mousePos.y));
                            
                        if (((mousePos.x >= 0 && mousePos.y >= 0) && (mousePos.x <= multiplicator * sizeWorldX && mousePos.y <= multiplicator * sizeWorldY)))
                        {
                            cameraCenterPos = mousePos;
                            mainCamera.setCenter(mousePos);
                        }
                    }

            }
            else
            {
                if (animationZoom > 0) 
                {   
                    --animationZoom; 
                    //Повільне наближення до центру
                    mainCamera.setCenter(sf::Vector2f(
                                    (0.9f * cameraCenterPos.x) + (0.1f * multiplicator * sizeWorldX / 2) ,
                                    (0.9f * cameraCenterPos.y) + (0.1f * multiplicator * sizeWorldY / 2) ));
                    cameraCenterPos = mainCamera.getCenter();
                }
                else
                {   
                    mainCamera.setCenter(sf::Vector2f(multiplicator * sizeWorldX / 2, multiplicator * sizeWorldY / 2));
                }
                mainCamera.setSize(zoomSize);
            }

        window.setView(mainCamera);
        ImGui::SFML::Render(window);
        window.display();
    }
    else
    {
    ImGui::SFML::Shutdown();
    isMainWindowOpen = false;
    }
}

