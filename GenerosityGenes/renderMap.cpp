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


sf::Vector2f zoomSize;
sf::RectangleShape tempShape(sf::Vector2f(multiplicator, multiplicator));
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

Minion* minionToShowBrain = nullptr;
void InitializationRender()
{
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
void render()
{
    if (window.isOpen())
    {

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        //Фон Карти
        window.draw(airShape);


        tempShape.setOutlineThickness(0);
        for (const auto area : colonyArea)
        {
            tempColor = area.second;
            tempColor.a = 100;
            tempShape.setFillColor(tempColor);
            tempShape.setPosition(sf::Vector2f(multiplicator * area.first.x, multiplicator * area.first.y));
            window.draw(tempShape);
        }
        for (const auto colony : allColonys)
        {

            for (const auto minion : colony.second->colonyAddresses)
            {
                tempShapeMinion.setPosition(sf::Vector2f(multiplicator * minion->position.x, multiplicator * minion->position.y));

                if (minion->IsDead)
                    tempShapeMinion.setFillColor(dC::dead);
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
                    else
                    {
                        tempShapeMinion.setOutlineColor(sf::Color::Black);
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

                    if (abs(mousePosOld.x - mousePos.x) + abs(mousePosOld.y - mousePos.y) > 100.0f && !isWindowMinionBrain)
                    {
                        //Повільно рухати до миші
                        mousePos = sf::Vector2f(
                            (cameraMoveSpeed * mousePosOld.x) + ((1 - cameraMoveSpeed) * mousePos.x),
                            (cameraMoveSpeed * mousePosOld.y) + ((1 - cameraMoveSpeed) * mousePos.y));
                            
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
        window.display();
    }
    else
    {
    isMainWindowOpen = false;
    }
}

