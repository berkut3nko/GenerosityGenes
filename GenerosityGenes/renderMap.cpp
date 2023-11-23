#include "primary_utilities.hpp"
#include "settings.hpp"
unsigned int findMultiplierSize(unsigned int monitorSizeX, unsigned int monitorSizeY)
{
    return (monitorSizeX / sizeWorldX >= monitorSizeY / sizeWorldY) ? monitorSizeY / sizeWorldY : monitorSizeX / sizeWorldX;
}
unsigned int multiplicator = findMultiplierSize(1980, 1080);
sf::RenderWindow window(sf::VideoMode(multiplicator* sizeWorldX, multiplicator* sizeWorldY), "GenerosityGenes");
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
sf::Vector2f mousePosOld = sf::Vector2f(multiplicator * sizeWorldX / 2, multiplicator * sizeWorldY / 2);
bool zoom = false;
sf::Vector2f mousePos;
double animationZoomSin;
double cosHealth;


sf::Vector2f zoomSize;
sf::RectangleShape tempShape(sf::Vector2f(multiplicator, multiplicator));
sf::RectangleShape tempAreaShape(sf::Vector2f(multiplicator, multiplicator));
sf::RectangleShape airShape(sf::Vector2f(multiplicator* sizeWorldX, multiplicator* sizeWorldY));
sf::RectangleShape borderShape[4]=
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
    airShape.setFillColor(dC::air);
    airShape.setPosition(sf::Vector2f(0, 0));

    borderShape[0].setFillColor(dC::border);
    borderShape[0].setPosition(sf::Vector2f(0,0));
    borderShape[1].setFillColor(dC::border);
    borderShape[1].setPosition(sf::Vector2f(0, multiplicator * (sizeWorldY-2)));
    borderShape[2].setFillColor(dC::border);
    borderShape[2].setPosition(sf::Vector2f(0, 0));
    borderShape[3].setFillColor(dC::border);
    borderShape[3].setPosition(sf::Vector2f(multiplicator * (sizeWorldX-2), 0));


    neuronShape.setFillColor(sf::Color::Blue);

    window_MinionBrain.setVisible(false);
}
sf::Clock deltaClock;


//ImGUI 
char addColonyName[64];
int firstLayer;
int secondLayer;
string colonyNames;
int spawnerSize = 3;
std::stringstream  ss;
void ColonyListUpdate()
{
    colonyNames = "";
    for (auto colony : allColonies)
    {
        colonyNames += colony.first + '\0';
    }
}
void render()
{
    if (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        ImGui::SFML::Update(window,deltaClock.restart());
        ImGui::Begin("Game control");

        ImGui::BeginTabBar("Controler"); {
            if(ImGui::BeginTabItem("Setting"))
            {
                ImGui::SetNextItemWidth(150.f);
                ImGui::DragFloat("camera zoom scale", &zoomScale, 0.1f, 1.0f, 50.0f, "%.2f");
                ImGui::SetNextItemWidth(150.f);
                ImGui::DragFloat("camera move speed", &cameraMoveSpeed, 0.01f, 0.1f, 1.0f, "%.2f");
                ImGui::SetNextItemWidth(150.f);
                ImGui::DragFloat("zoom animation quality", &animation_smoothness, 1.0f, 10.0f, 100.0f, "%.0f");

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

                static int selectedColony = 0;
                ImGui::Combo("All colonies",&selectedColony,colonyNames.c_str(), allColonies.size());
                ss.str("");
                ss << next(allColonies.begin(), selectedColony)->second->sizeColony;
                ImGui::Text( string("Colony size:" + ss.str()).c_str());
                if(ImGui::Button("Create minion", ImVec2(140, 30)))
                {
                    next(allColonies.begin(), selectedColony)->second->createMinion();
                }
                ImGui::Text("Neural network coefficients:");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_AttackEnemy", &(*next(allColonies.begin(), selectedColony)->second).coef_AttackEnemy,0.05,-1,1,"%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_AttackTeam", &(*next(allColonies.begin(), selectedColony)->second).coef_AttackTeam,0.05,-1,1,"%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_Border", &(*next(allColonies.begin(), selectedColony)->second).coef_Border,0.05,-1,1,"%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_Born", &(*next(allColonies.begin(), selectedColony)->second).coef_Born,0.05,-1,1,"%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_Eat", &(*next(allColonies.begin(), selectedColony)->second).coef_Eat,0.05,-1,1,"%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_Protection", &(*next(allColonies.begin(), selectedColony)->second).coef_Protection,0.05,-1,1,"%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_SpawnerEnemy", &(*next(allColonies.begin(), selectedColony)->second).coef_SpawnerEnemy,0.05,-1,1,"%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_SpawnerTeam", &(*next(allColonies.begin(), selectedColony)->second).coef_SpawnerTeam,0.05,-1,1,"%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_Synthesis", &(*next(allColonies.begin(), selectedColony)->second).coef_Synthesis,0.05,-1,1,"%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_TeamClose", &(*next(allColonies.begin(), selectedColony)->second).coef_TeamClose,0.05,-1,1,"%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_EnemyClose", &(*next(allColonies.begin(), selectedColony)->second).coef_EnemyClose,0.05,-1,1,"%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_TeamSpawnerClose", &(*next(allColonies.begin(), selectedColony)->second).coef_TeamSpawnerClose,0.05,-1,1,"%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_EnemySpawnerClose", &(*next(allColonies.begin(), selectedColony)->second).coef_EnemySpawnerClose,0.05,-1,1,"%.2f");
                ImGui::SetNextItemWidth(200.f);
                ImGui::DragFloat("coef_EatClose", &(*next(allColonies.begin(), selectedColony)->second).coef_EatClose,0.05,-1,1,"%.2f");
                ImGui::EndGroup();
                ImGui::EndTabItem();
            }
        };
        ImGui::EndTabBar();
        ImGui::End();
        window.clear();

        //Фон Карти
        window.draw(airShape);


        tempShape.setOutlineThickness(0);
        for (const auto area : colonyArea)
        {
            tempAreaShape.setPosition(sf::Vector2f(area.x * multiplicator, area.y * multiplicator));
            if(worldMap[area.x][area.y].minionAddress!=nullptr)
                tempColor = (worldMap[area.x][area.y].minionAddress->myColony->colonyColor);
            tempColor.a = 100;
            tempAreaShape.setFillColor(tempColor);
            window.draw(tempAreaShape);
        }
        for (const auto colony : allColonies)
        {

            for (const auto minion : colony.second->colonyAddresses)
            {
                tempShapeMinion.setPosition(sf::Vector2f(multiplicator * minion->position.x, multiplicator * minion->position.y));

                if (minion->IsDead) {
                    tempShapeMinion.setFillColor(dC::dead);
                }
                else {

                    tempShapeMinion.setFillColor(minion->myColony->colonyColor);

                    if (minion->IsDead)
                    {
                        tempShapeMinion.setOutlineColor(sf::Color::Black);
                    }
                    else if (minion->IsSynthesis)
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
                        tempShapeMinion.setOutlineColor(sf::Color(255 * cosHealth, 0 , 0, 255));
                    }
                }
                window.draw(tempShapeMinion);
            }
        }

        for (const auto fruit : poolOfFruits)
        {

            tempShape.setFillColor(dC::fruit);
            tempShape.setPosition(sf::Vector2f(multiplicator * fruit.x, multiplicator * fruit.y));
            window.draw(tempShape);
        }

        tempShape.setOutlineThickness((float)multiplicator / 10);
        tempShape.setOutlineColor(dC::dead);

        for (std::pair<Colony*, Spawner*> spawner : allActiveSpawners)
        {
            tempShape.setFillColor(spawner.first->colonyColor);
            tempShape.setPosition(sf::Vector2f(multiplicator * spawner.second->spawnerPosition.x, multiplicator * spawner.second->spawnerPosition.y));
            window.draw(tempShape);

        }

        window.draw(borderShape[0]);
        window.draw(borderShape[1]);
        window.draw(borderShape[2]);
        window.draw(borderShape[3]);


        animationZoomSin = (sin(((double)animationZoom / (double)(animation_smoothness * 2.0)) * PI) * zoomScale) + 1;
        zoomSize = sf::Vector2f(multiplicator * sizeWorldX / (animationZoomSin),
                                multiplicator * sizeWorldY / (animationZoomSin));
        //if(sf::Keyboard::isKeyPressed(sf::Keyboard::C))
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window_MinionBrain.isOpen())
        {
            mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if ((mousePos.x >= 0 && mousePos.y >= 0) && (mousePos.x <= multiplicator * sizeWorldX && mousePos.y <= multiplicator * sizeWorldY))
            {
                if (worldMap[(size_t)mousePos.x / multiplicator][(size_t)mousePos.y / multiplicator].type == minion)
                {
                    //демонстрація нейроної мережі
                    window_MinionBrain.setVisible(true);
                    window_MinionBrain.clear();
                    minionToShowBrain = worldMap[(size_t)mousePos.x / multiplicator][(size_t)mousePos.y / multiplicator].minionAddress;
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
                                weightShape[0] = sf::Vertex(sf::Vector2f(250.f * layersIt + 1.5f, (((MinionSettings::minionInputs + minionToShowBrain->myColony->sizeMemmory) - minionToShowBrain->MyBrain.layers_[layersIt].input_size_ ) / 2) * 12.f + 12.f * inputsIt + 1.5f ),
                                    sf::Color(0, 255.0 * (1.0 / (1.0 + exp(minionToShowBrain->MyBrain.layers_[layersIt].weights()[inputsIt][outputsIt]))), 0, 255 * (1 - (1 / (1 + exp(minionToShowBrain->MyBrain.layers_[layersIt].weights()[inputsIt][outputsIt]))))));
                                weightShape[0].position.y += (layersIt != 0) ? -12.0f : 0.0f;
                                weightShape[1] = sf::Vertex(sf::Vector2f(neuronShape.getPosition().x+1.5f,neuronShape.getPosition().y+1.5f),
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

                    if (abs(mousePosOld.x - mousePos.x) + abs(mousePosOld.y - mousePos.y) > (350.0f/zoomScale) && !isWindowMinionBrain)
                    {
                        //Повільно рухати до миші
                        mousePos = sf::Vector2f(
                            ((1 - (cameraMoveSpeed / 5)) * mousePosOld.x) + ((cameraMoveSpeed / 5) * mousePos.x),
                            ((1 - (cameraMoveSpeed / 5)) * mousePosOld.y) + ((cameraMoveSpeed / 5) * mousePos.y));
                            
                        if (((mousePos.x >= 0 && mousePos.y >= 0) && (mousePos.x <= multiplicator * sizeWorldX && mousePos.y <= multiplicator * sizeWorldY)))
                        {
                            mousePosOld = mousePos;
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
                                    (0.9f * mousePosOld.x) + (0.1f * multiplicator * sizeWorldX / 2) ,
                                    (0.9f * mousePosOld.y) + (0.1f * multiplicator * sizeWorldY / 2) ));
                    mousePosOld = mainCamera.getCenter();
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

