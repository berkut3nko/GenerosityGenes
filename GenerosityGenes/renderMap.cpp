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

sf::RenderWindow window_MinionBrain(sf::VideoMode(500, 980), "Minion brain");
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
sf::CircleShape tempShapeMinion(static_cast<float>(multiplicator / 2.5));

sf::CircleShape neuronShape(5.f);
sf::Vertex weightShape[2];

Minion* minionToShowBrain = nullptr;
void InitializationRender()
{
    tempShapeMinion.setOutlineThickness(static_cast<size_t>(multiplicator / 15));
    airShape.setFillColor(dC::air);
    airShape.setPosition(sf::Vector2f(0, 0));

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
        //��� �����
        window.draw(airShape);


        for (int x = 0; x < sizeWorldX; ++x) {
            for (int y = 0; y < sizeWorldY; ++y) {
                if (worldMap[x][y].type != Types::air) {
                    if (worldMap[x][y].type != minion)
                    {

                        tempShape.setFillColor(defaultColor(worldMap[x][y].type));
                        tempShape.setPosition(sf::Vector2f(multiplicator * x, multiplicator * y));
                        window.draw(tempShape);
                    }
                    else
                    {
                        tempShapeMinion.setPosition(sf::Vector2f(multiplicator * x, multiplicator * y));

                        if (worldMap[x][y].minionAsdress->IsDead)
                            tempShapeMinion.setFillColor(dC::dead);
                        else {

                                tempShapeMinion.setFillColor(worldMap[x][y].minionAsdress->myColony->colonyColor);

                            if (worldMap[x][y].minionAsdress->IsSynthesis)
                            {
                                tempShapeMinion.setOutlineColor(sf::Color::Green);
                            }
                            else if (worldMap[x][y].minionAsdress->IsProtection)
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
            }
        }



        animationZoomSin = (sin(((double)animationZoom / (double)(animation_smoothness * 2.0)) * PI) * zoomScale) + 1;
        zoomSize = sf::Vector2f(multiplicator * sizeWorldX / (animationZoomSin),
                                multiplicator * sizeWorldY / (animationZoomSin));
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window_MinionBrain.isOpen())
        {
            mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if (worldMap[(size_t)mousePos.x / multiplicator][(size_t)mousePos.y / multiplicator].type == minion)
            {
                //������������ ������� �����
                window_MinionBrain.setVisible(true);
                window_MinionBrain.clear();
                minionToShowBrain = worldMap[(size_t)mousePos.x / multiplicator][(size_t)mousePos.y / multiplicator].minionAsdress;
                isWindowMinionBrain = true;
                for (size_t inputIt = 0; inputIt < MinionSettings::minionInputs; ++inputIt)
                {
                    neuronShape.setPosition(sf::Vector2f(0.f, 17.f * inputIt));
                    window_MinionBrain.draw(neuronShape);
                }
                for (size_t layersIt = 0; layersIt < minionToShowBrain->MyBrain.layers_.size(); ++layersIt)
                {
                    for (size_t outputsIt = 0; outputsIt < minionToShowBrain->MyBrain.layers_[layersIt].output_size_; ++outputsIt)
                    {
                        neuronShape.setPosition(sf::Vector2f((250.f * (layersIt + 1)),((MinionSettings::minionInputs - minionToShowBrain->MyBrain.layers_[layersIt].output_size_)/2) * 17.f + 17.f * outputsIt));
                        window_MinionBrain.draw(neuronShape);
                        for (size_t inputsIt = 0; inputsIt < minionToShowBrain->MyBrain.layers_[layersIt].input_size_; ++inputsIt)
                        {
                            weightShape[0] = sf::Vertex(sf::Vector2f(250.f * layersIt, (((MinionSettings::minionInputs - minionToShowBrain->MyBrain.layers_[layersIt].input_size_) / 2) * 17.f + 17.f * inputsIt) + 5.f));
                            weightShape[1] = sf::Vertex(neuronShape.getPosition());
                            weightShape->color = sf::Color(0,255* minionToShowBrain->MyBrain.layers_[layersIt].weights()[inputsIt][outputsIt],0,255);
                            window_MinionBrain.draw(weightShape,2,sf::Lines);
                        }
                    }
                }
                window_MinionBrain.display();

            }
            else
            {
                isWindowMinionBrain = false;
                window_MinionBrain.setVisible(false);
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
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
            if (zoom)
            {
                if (animationZoom <= animation_smoothness)
                {
                    ++animationZoom;
                }

                mainCamera.setSize(zoomSize);
                mousePos = (window.mapPixelToCoords(sf::Mouse::getPosition(window)));
                //���� ������ �� ��� ��������
                if (abs(mousePosOld.x - mousePos.x) + abs(mousePosOld.y - mousePos.y) > 100.0f && !isWindowMinionBrain)
                {
                    //������� ������ �� ����
                    mainCamera.setCenter(
                        sf::Vector2f((cameraMoveSpeed * mousePosOld.x) + ((1 - cameraMoveSpeed) * mousePos.x),
                                     (cameraMoveSpeed * mousePosOld.y) + ((1 - cameraMoveSpeed) * mousePos.y))
                                        );
                    mousePosOld = mainCamera.getCenter();
                }
            }
            else
            {
                if (animationZoom > 0) 
                {   
                    --animationZoom; 
                    //������� ���������� �� ������
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

