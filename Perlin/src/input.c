#include "input.h"
#include "macros.h"
#include "mesh.h"
#include "scene.h"
#include "shader.h"

#define BOUNDS 1
#define NEAR_PLANE 0.01f
#define FAR_PLANE 2.0f

static InputData g_data = {
    .planeColor = {1, 1, 1, 1},
    .planeDetail = INITIAL_PLANE_RES,
    .chunkSizeXZ = CHUNK_XZ,
    .amplitude = 1.0f,
    .frequency = 1.0f,
    .lacunarity = 2.0f,
    .octaves = 4,
    .persistence = 0.5f,
    .scale = 10.0f,
    .offsetXY = 0.5f,
    .heightScale = 1.0f,
    .wireframe = false,
    .showHelp = false,
    .windowVisible = true,
    .doDisplacement = false,
    .showNoiseGui = true,
    .reloadTerrain = true,
    .renderTerrain = false,
    .liveUpdate = true,
};

static void callbackKeyboard(ProgContext ctx, int key, int action, int mods) {
    NK_UNUSED(mods);

    camera_keyboardCallback(scene_getCamera(), key, action);

    const int pressedKey = key;

    if (action == GLFW_PRESS) {
        switch (pressedKey) {
            case GLFW_KEY_ESCAPE:
                window_shouldCloseWindow(ctx);
                break;
            case GLFW_KEY_F1:
                TOGGLE(g_data.showHelp);
                break;
            case GLFW_KEY_F2:
                TOGGLE(g_data.wireframe);
                break;
            case GLFW_KEY_F11:
                window_setFullscreen(ctx, !window_isFullscreen(ctx));
                break;
            case GLFW_KEY_R:
                shader_reload();
                g_data.reloadTerrain = true;
                break;
            case GLFW_KEY_T:
                TOGGLE(g_data.renderTerrain);
                break;
            case GLFW_KEY_U:
                TOGGLE(g_data.liveUpdate);
                break;
            default:
                break;
        }
    }
}

void callbackFramebuffer(ProgContext ctx, int width, int height) {
    NK_UNUSED(ctx);
    const float aspect = (float)width / (float)height;

    scene_perspective(45.0f, aspect, 0.1f, 300.0f);

    g_data.windowVisible = (width > 0 && height > 0);
}

static void callbackMouseMove(ProgContext ctx, double x, double y) {
    camera_mouseMoveCallback(scene_getCamera(), ctx, (float) x, (float) y);
}

static void callbackMouseButton(ProgContext ctx, int button, int action, int mods) {
    NK_UNUSED(ctx);
    NK_UNUSED(mods);
    camera_mouseButtonCallback(scene_getCamera(), button, action);
}

void input_init(ProgContext ctx) {
    window_setKeyboardCallback(ctx, callbackKeyboard);
    window_setFramebufferSizeCallback(ctx, callbackFramebuffer);
    window_setMouseMovementCallback(ctx, callbackMouseMove);
    window_setMouseButtonCallback(ctx, callbackMouseButton);
}

void input_cleanup(void) {

}

InputData* input_getInputData(void) {
    return &g_data;
}
