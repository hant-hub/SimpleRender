#include "config.h"
#include "fractal.h"
#include "frame.h"
#include "init.h"
#include "util.h"
#include <GLFW/glfw3.h>
#include "sprite.h"
#include "vec3.h"

static sm_vec2f pos = {0};
static sm_vec2f zinit = {0};
static float zoom = 1.5f;
static float pw = 2;

static FractalRenderer* r;


int main() {
    r = malloc(sizeof(FractalRenderer));
    PresentInfo p = {0};
    CRASH_CALL(CreateVulkan());

    Attachment attachments[SR_SPRITE_ATTACHMENT_NUM];
    SubPass passes[1];

    FractalGetSubpass(passes, attachments, 0);
    CRASH_CALL(InitPresent(&p, passes, 1, attachments, 1));
    CRASH_CALL(FractalInit(r,"mandel", &p.p, 0));

    FractalSetPos(r, (sm_vec2f){0.0f, 0.0f});
    FractalSetZoom(r, 1.5f);
    FractalSetColor(r, (sm_vec3f){1.0f, 0.0f, 1.0f});

    //build multipass

    unsigned int frameCounter = 0;
    double last = 0.0;
    bool flip = FALSE;
    
    float dt = 0.0f;

    while (!glfwWindowShouldClose(sr_context.w)) {
        double start = glfwGetTime();
        glfwPollEvents();

        if (glfwGetKey(sr_context.w, GLFW_KEY_Z) == GLFW_PRESS) {
            zoom *= 1.005f;
        }
        if (glfwGetKey(sr_context.w, GLFW_KEY_X) == GLFW_PRESS) {
            zoom *= 1.0/1.005;
        }

        if (glfwGetKey(sr_context.w, GLFW_KEY_C) == GLFW_PRESS) {
            pw += 1;
        }
        if (glfwGetKey(sr_context.w, GLFW_KEY_V) == GLFW_PRESS) {
            pw -= 1;
        }

        if (glfwGetKey(sr_context.w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            if (glfwGetKey(sr_context.w, GLFW_KEY_LEFT) == GLFW_PRESS) {
                zinit.x += 1.0f * dt;
            }
            if (glfwGetKey(sr_context.w, GLFW_KEY_UP) == GLFW_PRESS) {
                zinit.y += 1.0f * dt;
            }
            if (glfwGetKey(sr_context.w, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                zinit.x -= 1.0f * dt;
            }
            if (glfwGetKey(sr_context.w, GLFW_KEY_DOWN) == GLFW_PRESS) {
                zinit.y -= 1.0f * dt;
            }
        } else {
            if (glfwGetKey(sr_context.w, GLFW_KEY_LEFT) == GLFW_PRESS) {
                pos.x += zoom * 1.0f * dt;
            }
            if (glfwGetKey(sr_context.w, GLFW_KEY_UP) == GLFW_PRESS) {
                pos.y += zoom * 1.0f * dt;
            }
            if (glfwGetKey(sr_context.w, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                pos.x -= zoom * 1.0f * dt;
            }
            if (glfwGetKey(sr_context.w, GLFW_KEY_DOWN) == GLFW_PRESS) {
                pos.y -= zoom * 1.0f * dt;
            }
        }
        if (glfwGetKey(sr_context.w, GLFW_KEY_R) == GLFW_PRESS) {
            zinit = (sm_vec2f){0};
            pos = (sm_vec2f){0};
            zoom = 2.0f;
        }




//        sm_vec3f color = (sm_vec3f) {
//            sin(glfwGetTime()),
//            cos(glfwGetTime()) * sin(glfwGetTime() + SM_PI/2),
//            cos(glfwGetTime()) * cos(glfwGetTime() + SM_PI/2)
//        };
//
//        color = (sm_vec3f) {
//            color.x * color.x + 0.1,
//            color.y * color.y + 0.1,
//            color.z * color.z + 0.1f
//        };
        sm_vec3f color = (sm_vec3f) {
            1.0f,
            0.3f,
            0.7f
        };

        FractalSetPos(r, pos);
        FractalSetZoom(r, zoom);
        FractalSetColor(r, color);
        FractalSetInit(r, zinit);

        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(&p, frameCounter);
        FractalDrawFrame(r, &p, frameCounter);
        SubmitFrame(&p, frameCounter); 

        dt = glfwGetTime() - start;

    }

    FractalDestroy(r);
    DestroyPresent(&p);
    free(r);
    DestroyVulkan();
    return 0;
}
