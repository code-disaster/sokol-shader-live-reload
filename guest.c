/*
	guest.c - guest module to manage live reload & recreation of shader code

	Based on a sample of cr.h, see https://github.com/fungos/cr/blob/master/samples/basic_guest.c.
 */
#include <stdio.h>
#include <assert.h>
#include <cr.h>

#include "cube.h"
#include "cube.glsl.h"

static unsigned int CR_STATE version = 1;

static void recreate_cube_shader_pipeline(struct cr_plugin* ctx)
{
	app_state_t* state = (app_state_t*) ctx->userdata;

    /* create shader */
    sg_shader _shd = sg_make_shader(cube_shader_desc());

    /* create pipeline object */
    sg_pipeline _pip = sg_make_pipeline(&(sg_pipeline_desc) {
        .layout = {
            /* test to provide buffer stride, but no attr offsets */
            .buffers[0].stride = 28,
            .attrs = {
                [ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_vs_color0].format = SG_VERTEXFORMAT_FLOAT4
            }
        },
        .shader = _shd,
        .index_type = SG_INDEXTYPE_UINT16,
        .depth_stencil = {
            .depth_compare_func = SG_COMPAREFUNC_LESS_EQUAL,
            .depth_write_enabled = true,
        },
        .rasterizer.cull_mode = SG_CULLMODE_BACK,
        .rasterizer.sample_count = sapp_query_desc().sample_count,
    });

	/*
		TODO error checks
		- for example, this could output an error, then just reuse the previous shader/pipeline
	*/

	/* cleanup & replace */
	sg_destroy_pipeline(state->pip);
	state->pip = _pip;

	sg_destroy_shader(state->shd);
	state->shd = _shd;
}

CR_EXPORT int cr_main(struct cr_plugin *ctx, enum cr_op operation)
{
    assert(ctx);

	if (operation == CR_LOAD)
	{
		/* initial creation */
		recreate_cube_shader_pipeline(ctx);
	}

    if (operation != CR_STEP)
	{
        fprintf(stdout, "OP: %s(%d)\n", operation == CR_LOAD ? "LOAD" : "UNLOAD", ctx->version);
        return 0;
    }

	if (ctx->version > version)
	{
		recreate_cube_shader_pipeline(ctx);
	}
	else if (ctx->version < version)
	{
        fprintf(stdout, "A rollback happened due to failure: %x!\n", ctx->failure);
    }

    version = ctx->version;

    static bool print_version = true;
    if (print_version)
	{
        fprintf(stdout, "loaded version: %d\n", ctx->version);
        print_version = false;
    }

    return 0;
}
