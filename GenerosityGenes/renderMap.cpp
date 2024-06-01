#include "primary_utilities.hpp"
#include "renderData.hpp"
#include "settings.hpp"


const screen myScreen = { 1980, 1080 };
float zoomScale = 3.0f;
float cameraMoveSpeed = 0.1f;
float animation_smoothness = 20.0f;
float cameraMoveRange = 0.33f;

float findMultiplierSize(screen windowSize)
{
    return static_cast<float>((windowSize.x / sizeWorldX >= windowSize.y / sizeWorldY) ? windowSize.y / sizeWorldY : windowSize.x / sizeWorldX);
}

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
        if (isTestVersion) {

            size_t y = 100;
            //Test Texture
            for (size_t i = 0; i < minionTexturesPool.size(); ++i)
            {
                Sprite temp;
                temp.setTexture(minionTexturesPool[i].first.idle);
                size_t x = 100;
                temp.setPosition(sf::Vector2f(100., 100.));
                window.draw(temp);
                x += 100;
                window.draw(Sprite(minionTexturesPool[i].first.dead));
                x += 100;
                window.draw(Sprite(minionTexturesPool[i].first.protection));
                x += 100;
                window.draw(Sprite(minionTexturesPool[i].first.synthesis));
                y += 100;
            }
        }

        for (const auto area : colonyArea)
        {
            if (area.second != nullptr) {
                tempColor = (area.second->colonyColor);
                tempAreaShape.setPosition(sf::Vector2f(area.first.x * multiplicator, area.first.y * multiplicator));
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

        for (std::pair<shared_ptr<Colony>, shared_ptr<Spawner>> spawner : allActiveSpawners)
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

