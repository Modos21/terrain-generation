#include "gui.h"
#include "input.h"
#include "scene.h"
#include "shader.h"

static const GuiHelpLine g_helpLines[] = {
    {"Toggle this menu", "F1"},
    {"Toggle wireframe", "F2"},
    {"Toggle fullscreen", "F11"},
    {"Reload Shaders", "R"},
    {"Close App", "ESC"}
};

static GuiRenderWindow g_noiseRenderWindow = NULL;

static bool gui_drawHelp(ProgContext ctx) {
    int w, h;
    window_getRealSize(ctx, &w, &h);
    float width = (float)w * 0.25f;
    float height = (float)h * 0.5f;
    float x = width * 1.5f;
    float y = height * 0.5f;

    return gui_widgetHelp(ctx, g_helpLines, NK_LEN(g_helpLines), nk_rect(x, y, width, height));
}

static void gui_drawControls(ProgContext ctx) {
    InputData *input = input_getInputData();

    if (gui_begin(ctx, "Perlin Noise demo", nk_rect(10, 10, 300, 550),
                   NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_SCALABLE)) {
        gui_layoutRowDynamic(ctx, 20, 1);

        gui_label(ctx, "General settings", NK_TEXT_ALIGN_LEFT);

        gui_layoutRowDynamic(ctx, 40, 2);

        if (gui_button(ctx, "Quit")) {
            window_shouldCloseWindow(ctx);
        }

        if (gui_button(ctx, window_isFullscreen(ctx) ? "Window" : "Fullscreen")) {
            window_setFullscreen(ctx, !window_isFullscreen(ctx));
        }

        if (gui_button(ctx, "Shader reload")) {
            shader_reload();
        }

        if (gui_button(ctx, "Take Screenshot")) {
            texture_saveScreenshot(ctx);
        }

        gui_layoutRowDynamic(ctx, 30, 1);

        int currDetail = input->planeDetail;
        gui_propertyInt(ctx, "Plane detail", 1, &input->planeDetail, 512, 1, 0.5f);

        if (currDetail != input->planeDetail) {
            scene_updatePlaneRes(input->planeDetail);
        }

        if (gui_treePush(ctx, NK_TREE_TAB, "Noise settings", NK_MAXIMIZED)) {
            gui_layoutRowDynamic(ctx, 30, 1);

            gui_propertyInt(ctx, "Octaves", 1, &input->octaves, 10, 1, 0.5f);
            gui_propertyFloat(ctx, "Amplitude", 0.0f, &input->amplitude, 50.0f, 0.1f, 0.01f);
            gui_propertyFloat(ctx, "Frequency", 0.01f, &input->frequency, 50.0f, 0.01f, 0.01f);
            gui_propertyFloat(ctx, "Scale", 0.1f, &input->scale, 100.0f, 0.1f, 0.1f);
            gui_propertyFloat(ctx, "Persistence", 0.0f, &input->persistence, 1.0f, 0.01f, 0.001f);
            gui_propertyFloat(ctx, "Lacunarity", 1.0f, &input->lacunarity, 10.0f, 0.1f, 0.01f);
            gui_propertyFloat(ctx, "Offset (XY)", 0.0f, &input->offsetXY, 100.0f, 0.1f, 0.1f);

            gui_widgetColor(ctx, "Noise color", input->planeColor);

            gui_checkbox(ctx, input->showNoiseGui ? "Hide noise GUI" : "Show noise GUI", &input->showNoiseGui);

            gui_treePop(ctx);
        }

        if (gui_treePush(ctx, NK_TREE_TAB, "Vertex displacement", NK_MAXIMIZED)) {
            gui_layoutRowDynamic(ctx, 30, 1);

            gui_checkbox(ctx, "Vertex displacement", &input->doDisplacement);
            gui_propertyFloat(ctx, "Height scale", 0.0f, &input->heightScale, 2.0f, 0.01f, 0.01f);
            gui_treePop(ctx);
        }
    }
    gui_end(ctx);
}

static void gui_drawNoiseGui(ProgContext ctx, int w, int h, void *data) {
    NK_UNUSED(ctx);
    NK_UNUSED(w);
    NK_UNUSED(h);
    NK_UNUSED(data);
    scene_drawNoiseQuad();
}

void gui_myInit(ProgContext ctx) {
    gui_init(ctx, FHWCG_FONT_PATH);
    g_noiseRenderWindow = gui_createRenderWindow();
}

static void gui_drawNoiseWidget(ProgContext ctx, InputData* input) {
    // noise window
    int w, h;
    window_getFramebufferSize(ctx, &w, &h);

    float noiseGuiSize = 200;

    struct nk_rect bounds = {
        .x = (float) w - 20 - noiseGuiSize,
        .y = 10,
        .w = noiseGuiSize,
        .h = noiseGuiSize
    };

    gui_widgetRenderWindowFixed(ctx, "Noise",
        g_noiseRenderWindow, bounds, 1, NOISE_GUI_UNIT, gui_drawNoiseGui, input);
}

void gui_draw(ProgContext ctx) {
    InputData* input = input_getInputData();

    gui_drawControls(ctx);

    if (input->showHelp) {
        input->showHelp = gui_drawHelp(ctx);
    }

    if (input->showNoiseGui) {
        gui_drawNoiseWidget(ctx, input);
    }

    vec3 camPos;
    camera_getPosition(scene_getCamera(), camPos);

    int w, h;
    window_getRealSize(ctx, &w, &h);
    float guiW = 250, guiH = 30;
    float wF = (float) w;
    float hF = (float) h;

    if (gui_begin(ctx, "", nk_rect(wF - guiW - 10, hF - guiH - 10, guiW, guiH),
        NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
        gui_layoutRowDynamic(ctx, 30, 1);

        char buf[100];
        snprintf(buf, ARRAYSIZE(buf), "Cam (X %.3f  Y %.3f  Z %.3f)", camPos[0], camPos[1], camPos[2]);
        gui_label(ctx, buf, NK_TEXT_ALIGN_LEFT);
    }
    gui_end(ctx);
}

void gui_myCleanup(ProgContext ctx) {
    if (g_noiseRenderWindow) {
        gui_disposeRenderWindow(&g_noiseRenderWindow);
        g_noiseRenderWindow = NULL;
    }
    gui_cleanup(ctx);
}
