/*
    This is a slightly modified copy of the "noentry-dll-sapp" sample.
    see: https://github.com/floooh/sokol-samples/blob/master/sapp/noentry-dll-sapp.c
 */


/* need implementation unit for Handmade_Math in one place */
#define HANDMADE_MATH_IMPLEMENTATION

#include "cube.h"

#define SOKOL_SHDC_DECL // for vs_params_t, SLOT_vs_params
#include "cube.glsl.h"

void init(void* user_data)
{
    app_state_t* state = (app_state_t*) user_data;

    sg_setup(&(sg_desc){
        .gl_force_gles2 = sapp_gles2(),
        .mtl_device = sapp_metal_get_device(),
        .mtl_renderpass_descriptor_cb = sapp_metal_get_renderpass_descriptor,
        .mtl_drawable_cb = sapp_metal_get_drawable,
        .d3d11_device = sapp_d3d11_get_device(),
        .d3d11_device_context = sapp_d3d11_get_device_context(),
        .d3d11_render_target_view_cb = sapp_d3d11_get_render_target_view,
        .d3d11_depth_stencil_view_cb = sapp_d3d11_get_depth_stencil_view
    });

    /* cube vertex buffer */
    float vertices[] = {
        -1.0, -1.0, -1.0,   1.0, 0.5, 0.0, 1.0,
         1.0, -1.0, -1.0,   1.0, 0.5, 0.0, 1.0,
         1.0,  1.0, -1.0,   1.0, 0.5, 0.0, 1.0,
        -1.0,  1.0, -1.0,   1.0, 0.5, 0.0, 1.0,

        -1.0, -1.0,  1.0,   0.5, 1.0, 0.0, 1.0,
         1.0, -1.0,  1.0,   0.5, 1.0, 0.0, 1.0,
         1.0,  1.0,  1.0,   0.5, 1.0, 0.0, 1.0,
        -1.0,  1.0,  1.0,   0.5, 1.0, 0.0, 1.0,

        -1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,
        -1.0,  1.0, -1.0,   0.0, 0.5, 1.0, 1.0,
        -1.0,  1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
        -1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,

        1.0, -1.0, -1.0,    1.0, 0.5, 0.5, 1.0,
        1.0,  1.0, -1.0,    1.0, 0.5, 0.5, 1.0,
        1.0,  1.0,  1.0,    1.0, 0.5, 0.5, 1.0,
        1.0, -1.0,  1.0,    1.0, 0.5, 0.5, 1.0,

        -1.0, -1.0, -1.0,   0.5, 0.5, 1.0, 1.0,
        -1.0, -1.0,  1.0,   0.5, 0.5, 1.0, 1.0,
         1.0, -1.0,  1.0,   0.5, 0.5, 1.0, 1.0,
         1.0, -1.0, -1.0,   0.5, 0.5, 1.0, 1.0,

        -1.0,  1.0, -1.0,   0.5, 1.0, 0.5, 1.0,
        -1.0,  1.0,  1.0,   0.5, 1.0, 0.5, 1.0,
         1.0,  1.0,  1.0,   0.5, 1.0, 0.5, 1.0,
         1.0,  1.0, -1.0,   0.5, 1.0, 0.5, 1.0
    };
    sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(vertices),
        .content = vertices,
    });

    /* create an index buffer for the cube */
    uint16_t indices[] = {
        0, 1, 2,  0, 2, 3,
        6, 5, 4,  7, 6, 4,
        8, 9, 10,  8, 10, 11,
        14, 13, 12,  15, 14, 12,
        16, 17, 18,  16, 18, 19,
        22, 21, 20,  23, 22, 20
    };
    sg_buffer ibuf = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .size = sizeof(indices),
        .content = indices,
    });

    /*
        shader and pipeline objects are created by the guest library
    */

    /* setup resource bindings */
    state->bind = (sg_bindings) {
        .vertex_buffers[0] = vbuf,
        .index_buffer = ibuf
    };
}

void frame(void* user_data, struct cr_plugin* plugin_user_data)
{
    app_state_t* state = (app_state_t*) user_data;

    vs_params_t vs_params;
    const float w = (float) sapp_width();
    const float h = (float) sapp_height();
    hmm_mat4 proj = HMM_Perspective(60.0f, w/h, 0.01f, 10.0f);
    hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 1.5f, 6.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
    hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);
    state->rx += 1.0f; state->ry += 2.0f;
    hmm_mat4 rxm = HMM_Rotate(state->rx, HMM_Vec3(1.0f, 0.0f, 0.0f));
    hmm_mat4 rym = HMM_Rotate(state->ry, HMM_Vec3(0.0f, 1.0f, 0.0f));
    hmm_mat4 model = HMM_MultiplyMat4(rxm, rym);
    vs_params.mvp = HMM_MultiplyMat4(view_proj, model);

    sg_pass_action pass_action = {
        .colors[0] = { .action = SG_ACTION_CLEAR, .val = { 0.5f, 0.25f, 0.75f, 1.0f } }
    };
    sg_begin_default_pass(&pass_action, (int)w, (int)h);
    sg_apply_pipeline(state->pip);
    sg_apply_bindings(&state->bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &vs_params, sizeof(vs_params));
    sg_draw(0, 36, 1);
    sg_end_pass();
    sg_commit();
}

void cleanup()
{
    sg_shutdown();
}
