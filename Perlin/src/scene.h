#pragma once

#include <fhwcg/fhwcg.h>
#include "input.h"

#define INITIAL_PLANE_RES 128

void scene_init(ProgContext ctx);

Camera *scene_getCamera(void);

void scene_updatePlaneRes(int newRes);

void scene_draw(InputData *data);

void scene_drawNoiseQuad(void);

void scene_cleanup(void);
