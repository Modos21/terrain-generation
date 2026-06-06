#include <fhwcg/fhwcg.h>
#include <time.h>

#include "gui.h"
#include "shader.h"
#include "input.h"
#include "scene.h"

#define DEFAULT_WINDOW_WIDTH 1200
#define DEFAULT_WINDOW_HEIGHT 800

static void init(ProgContext ctx) {
    srand((uint)time(NULL));
    rand();
    shader_init();
    gui_myInit(ctx);
    scene_init(ctx);
    input_init(ctx);
}

/**
 * Gibt den Speicher aller zur Laufzeit reservierten Resourcen frei
 */
static void cleanup(ProgContext ctx) {
    scene_cleanup();
    shader_cleanup();
    input_cleanup();
    gui_myCleanup(ctx);
    // muss/sollte als letztes passieren
    window_cleanup(ctx);
}

int main(void) {
    ProgContext ctx = window_init(PROGRAM_NAME, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 1, WINDOW_FLAGS_VSYNC);

    init(ctx);
    glClearColor(0, 0, 0, 1);

    InputData *input = input_getInputData();

    while (window_startNewFrame(ctx)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float delta = (float)window_getDeltaTime(ctx);
        camera_updateCamera(scene_getCamera(), delta);

        if (input->windowVisible) {
            scene_draw(input);
            // für GUI immer wireframe aus
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            gui_render(ctx, gui_draw);
        }

        window_swapBuffers(ctx);
    }

    cleanup(ctx);

    return EXIT_SUCCESS;
}
