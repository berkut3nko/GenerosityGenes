#include "primary_utilities.hpp"
#include "settings.hpp"
unsigned int findMultiplierSize(unsigned int monitorSizeX, unsigned int monitorSizeY)
{
    return (monitorSizeX / sizeWorldX >= monitorSizeY / sizeWorldY) ? monitorSizeY / sizeWorldY : monitorSizeX / sizeWorldX;
}
unsigned int multiplicator = findMultiplierSize(1680, 980);
sf::RenderWindow window(sf::VideoMode(multiplicator* sizeWorldX, multiplicator* sizeWorldY), "SFML works!");
sf::View mainCamera;
size_t animationZoom = 0;
sf::Vector2f mousePosOld = sf::Vector2f(multiplicator * sizeWorldX / 2, multiplicator * sizeWorldY / 2);
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
        for (int x = 0; x < sizeWorldX; ++x) {
            for (int y = 0; y < sizeWorldY; ++y) {
                if (worldMap[x][y].type != minion)
                {
                    tempShape.setSize(sf::Vector2f(multiplicator, multiplicator));
                    tempShape.setFillColor(worldMap[x][y].color);
                    tempShape.setPosition(sf::Vector2f(multiplicator * x, multiplicator * y));
                    window.draw(tempShape);
                }
                else
                {
                    tempShape.setSize(sf::Vector2f(multiplicator, multiplicator));
                    tempShape.setFillColor(dC::air);
                    tempShape.setPosition(sf::Vector2f(multiplicator * x, multiplicator * y));
                    window.draw(tempShape);
                    tempShapeMinion.setRadius(static_cast<float>(multiplicator / 2.5));
                    tempShapeMinion.setOutlineColor(dC::air);
                    if (!worldMap[x][y].minionAsdress->IsDead)
                        tempShapeMinion.setFillColor(worldMap[x][y].color);
                    else
                        tempShapeMinion.setFillColor(dC::dead);
                    tempShapeMinion.setPosition(sf::Vector2f(multiplicator * x, multiplicator * y));

                    if (worldMap[x][y].minionAsdress->IsSynthesis)
                    {
                        tempShapeMinion.setOutlineThickness(static_cast<size_t>(multiplicator / 15));
                        tempShapeMinion.setOutlineColor(sf::Color::Green);
                    }
                    else if (worldMap[x][y].minionAsdress->IsProtection)
                    {
                        tempShapeMinion.setOutlineThickness(static_cast<size_t>(multiplicator / 15));
                        tempShapeMinion.setOutlineColor(sf::Color::Blue);
                    }
                    else
                    {
                        tempShapeMinion.setOutlineThickness(static_cast<size_t>(multiplicator / 30));
                        tempShapeMinion.setOutlineColor(sf::Color::Black);
                    }
                    window.draw(tempShapeMinion);
                }
            }
        }

        sf::Vector2f mousePos;

        double animationZoomSin = sin(((double)animationZoom / 10) * PI);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            {
                if (animationZoom <= 5) 
                {
                    ++animationZoom;
                }

                mainCamera.setSize(
                    sf::Vector2f
                    (
                        multiplicator * sizeWorldX / (animationZoomSin * zoomScale),
                        multiplicator * sizeWorldY / (animationZoomSin * zoomScale))
                    );
                mousePos = (window.mapPixelToCoords(sf::Mouse::getPosition(window)));
                if (abs(mousePosOld.x - mousePos.x) + abs(mousePosOld.y - mousePos.y) > 100.0f)
                {
                    mainCamera.setCenter(sf::Vector2f((cameraMoveSpeed * mousePosOld.x) + ((1 - cameraMoveSpeed) * mousePos.x), (cameraMoveSpeed * mousePosOld.y) + ((1 - cameraMoveSpeed) * mousePos.y)));
                    mousePosOld = (mousePos);
                }
            }
            else
            {
                if (animationZoom > 0) 
                { --animationZoom; mainCamera.setCenter(mousePosOld);}
                else
                {   mainCamera.setCenter(sf::Vector2f(multiplicator * sizeWorldX / 2, multiplicator * sizeWorldY / 2)); }
                mainCamera.setSize(
                    sf::Vector2f    
                    (
                        multiplicator * sizeWorldX / ((animationZoom != 0)?(animationZoomSin * zoomScale):1),
                        multiplicator * sizeWorldY / ((animationZoom != 0)?(animationZoomSin * zoomScale):1)
                    ));
                
            }

        
        window.setView(mainCamera);
        window.display();
    }
}
