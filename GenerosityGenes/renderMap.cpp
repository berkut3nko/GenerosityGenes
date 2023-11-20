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

static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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


bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}


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
void gui()
{
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX9 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}
void render()
{
    HWND hwnd = window.getSystemHandle();


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
            tempAreaShape.setPosition(sf::Vector2f(area.x * multiplicator, area.y * multiplicator));
            if(worldMap[area.x][area.y].minionAddress!=nullptr)
                tempColor = (worldMap[area.x][area.y].minionAddress->myColony->colonyColor);
            tempColor.a = 100;
            tempAreaShape.setFillColor(tempColor);
            window.draw(tempAreaShape);
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

