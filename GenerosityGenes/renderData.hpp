
struct screen
{
    size_t x, y;
};

extern const screen myScreen;
extern float zoomScale;
extern float cameraMoveSpeed;
extern float animation_smoothness;
extern float cameraMoveRange;

enum editorMode
{
    addFruit,
    addBorder,
    fillAir,
    addMinion,
    empty
};

extern editorMode cursorState;

extern float multiplicator;

extern int selectedColony;
void imGui();
