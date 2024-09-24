#include "command.h"
#include "common.h"
#include "config.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "mat4.h"
#include "memory.h"
#include "pipeline.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include "vec3.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan_core.h>
#include "sprite.h"


int main() {

    RenderState r = {0};
    CRASH_CALL(CreateVulkan());
    CRASH_CALL(SpriteInit(&r));


    SpriteHandle s1 = CreateSprite(&r, (sm_vec3f){50.0f, 50.0f, 0.0f}, (sm_vec3f){50, 50, 0});
    SpriteHandle s2 = CreateSprite(&r, (sm_vec3f){50.0f, 50.0f, 0.0f}, (sm_vec3f){50, 50, 0});


    sm_mat4f* model = GetModel(&r, s2);
    *model = sm_mat4_f32_translate(model, (sm_vec3f){-50.0f, -50.0f, 0.0f});
    *model = sm_mat4_f32_ry(model, SM_PI);
    *model = sm_mat4_f32_translate(model, (sm_vec3f){50.0f, 50.0f, 0.0f});


    unsigned int frameCounter = 0;
    while (!glfwWindowShouldClose(sr_context.w)) {
        glfwPollEvents();
        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        DrawFrame(&r, frameCounter);
        
        sm_mat4f* model = GetModel(&r, s1);
        *model = sm_mat4_f32_translate(model, (sm_vec3f){-50.0f, -50.0f, 0.0f});
        *model = sm_mat4_f32_ry(model, 0.01);
        *model = sm_mat4_f32_translate(model, (sm_vec3f){50.0f, 50.0f, 0.0f});

        model = GetModel(&r, s2);
        *model = sm_mat4_f32_translate(model, (sm_vec3f){-50.0f, -50.0f, 0.0f});
        *model = sm_mat4_f32_ry(model, 0.01);
        *model = sm_mat4_f32_translate(model, (sm_vec3f){50.0f, 50.0f, 0.0f});
    }

    SpriteDestroy(&r);
    DestroyVulkan();
    return 0;
}
