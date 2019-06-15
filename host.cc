#include "cube.h"

#include <Windows.h>
#include <stdlib.h>

/*
    configure cr.h
 */

#define CR_HOST CR_UNSAFE
#include <cr.h>

static cr_plugin plugin_ctx;
const char* plugin = CR_DEPLOY_PATH "/" CR_PLUGIN("guest");

/*
    sokol_app callbacks, implemented in cube.c
 */

#define SAMPLE_COUNT (4)

extern "C" void init(void* user_data);
extern "C" void frame(void* user_data);
extern "C" void cleanup();

/*
    wrap sokol_app callback functions to handle guest load/update/unload
 */

static void plugin_init(void* user_data)
{
    plugin_ctx.userdata = user_data;
    cr_plugin_load(plugin_ctx, plugin);

    init(user_data);
}

static void plugin_frame(void* user_data)
{
    cr_plugin_update(plugin_ctx);

    frame(user_data);
}

static void plugin_cleanup()
{
    cr_plugin_close(plugin_ctx);
    
    cleanup();
}

/*
    Windows application entry point
 */

#if FIPS_APP_CMDLINE
int main(int /*argc*/, char** /*argv*/)
#else
int WINAPI WinMain(HINSTANCE /*instance*/, HINSTANCE /*prev_instance*/, LPSTR /*cmdline*/, int /*cmdshow*/)
#endif
{
    app_state_t* state = (app_state_t*) calloc(1, sizeof(app_state_t));

    sapp_desc desc = {};
    desc.user_data = state;
    desc.init_userdata_cb = plugin_init;
    desc.frame_userdata_cb = plugin_frame;
    desc.cleanup_cb = plugin_cleanup;
    desc.width = 800;
    desc.height = 600;
    desc.sample_count = SAMPLE_COUNT;
    desc.window_title = "live shader reload sample";

    const int error_code = sapp_run(&desc);

    free(state);

    return error_code;
}
