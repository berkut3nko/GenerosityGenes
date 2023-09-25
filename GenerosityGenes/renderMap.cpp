#include "primary_utilities.hpp"
#include "settings.hpp"
unsigned int findMultiplierSize(unsigned int monitorSizeX, unsigned int monitorSizeY)
{
    return (monitorSizeX / sizeWorldX >= monitorSizeY / sizeWorldY) ? monitorSizeY / sizeWorldY : monitorSizeX / sizeWorldX;
}
unsigned int multiplicator = findMultiplierSize(1980, 1080);
sf::RenderWindow window(sf::VideoMode(multiplicator* sizeWorldX, multiplicator* sizeWorldY), "SFML works!");


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



sf::View mainCamera;
size_t animationZoom = 0;
sf::Vector2f mousePosOld = sf::Vector2f(multiplicator * sizeWorldX / 2, multiplicator * sizeWorldY / 2);
bool zoom = false;
sf::Vector2f mousePos;
double animationZoomSin;
sf::Vector2f zoomSize;




void render()
{
    sf::RectangleShape tempShape;
    sf::CircleShape tempShapeMinion;
    if (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        //Фон Карти
        tempShape.setSize(sf::Vector2f(multiplicator * sizeWorldX, multiplicator * sizeWorldY));
        tempShape.setFillColor(dC::air);
        tempShape.setPosition(sf::Vector2f(0, 0));
        window.draw(tempShape);
        for (int x = 0; x < sizeWorldX; ++x) {
            for (int y = 0; y < sizeWorldY; ++y) {
                if (worldMap[x][y].type != Types::air) {
                    if (worldMap[x][y].type != minion)
                    {
                        tempShape.setSize(sf::Vector2f(multiplicator, multiplicator));
                        tempShape.setFillColor(defaultColor(worldMap[x][y].type));
                        tempShape.setPosition(sf::Vector2f(multiplicator * x, multiplicator * y));
                        window.draw(tempShape);
                    }
                    else
                    {

                        tempShapeMinion.setRadius(static_cast<float>(multiplicator / 2.5));
                        tempShapeMinion.setOutlineThickness(static_cast<size_t>(multiplicator / 15));

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
        if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            zoom=!zoom;
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
                if (abs(mousePosOld.x - mousePos.x) + abs(mousePosOld.y - mousePos.y) > 100.0f)
                {
                    //Повільно рухати до миші
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
                    //Повільне наближення до центру
                    mainCamera.setCenter(sf::Vector2f(
                                    (0.75f * mousePosOld.x) + (0.25f * multiplicator * sizeWorldX / 2),
                                    (0.75f * mousePosOld.y) + (0.25f * multiplicator * sizeWorldY / 2)));
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
}

