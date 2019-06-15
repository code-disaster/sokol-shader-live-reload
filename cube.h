/*
    cube.h - includes & type definitions shared by host and guest

    The app_state is allocated by the host, then passed as user data pointer
    to both sokol_app and cr.
 */
#pragma once

#define SOKOL_DLL
#include "sokol_gfx.h"
#include "sokol_app.h"

#define HANDMADE_MATH_NO_SSE
#include "ext/HandmadeMath.h"

typedef struct
{
    float rx, ry;
    sg_shader shd;
    sg_pipeline pip;
    sg_bindings bind;
    unsigned int version;
} app_state_t;
