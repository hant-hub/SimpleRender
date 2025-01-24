#include "config.h"
#include "frame.h"
#include <string.h>
#include "init.h"
#include "log.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "sprite.h"


int main() {

    CRASH_CALL(CreateVulkan());

    Attachment attachments[SR_SPRITE_ATTACHMENT_NUM];
    SubPass passes[1];

    SpriteGetSubpass(passes, attachments, 0);

    PresentInfo p = {0};
    CRASH_CALL(InitPresent(&p, passes, 1, attachments, 1));

    SpriteRenderer* r = malloc(sizeof(SpriteRenderer));
    CRASH_CALL(SpriteInit(r, &p.p, 0, (Camera){.pos = {0, 0}, .size = {100, 100}, .rotation = 0}, 1));

    //build multipass


    Texture textures[2] = {0};
    DynamicTexture t[1] = {0};

    CRASH_CALL(CreateDynTexture(&t[0], (TextureConfig){
                    .width = 1000,
                    .height = 1000,
                    .filter = VK_FILTER_LINEAR,
                    .format = VK_FORMAT_R8G8B8_SRGB,
                    .channels = 3,
                    .accessmode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                    .anisotropy = VK_FALSE,
                }));


    textures[0] = t[0].t;    
    CRASH_CALL(SetTextureSlots(r, textures, 1));


    SpriteHandle s1 = CreateSprite(r, (sm_vec2f){0.0f, 0.0f}, (sm_vec2f){WIDTH, HEIGHT}, 0, 0);
    //SpriteHandle s3 = CreateSprite(&r, (sm_vec2f){50.0f, 0.0f}, (sm_vec2f){100, 100}, 1);

    BeginUpdateDynTexture(&t[0]);

    for (int i = 0; i < t[0].size/3; i += 1) {
        double x = (double)((i) % 1000) / 1000.0;
        double y = (((double)(i)) / 1000) / 1000.0;

        x -= 0.5;
        y -= 0.5;
        x *= 16;
        y *= 16;

        double px, py;
        int color = 255;
        for (int j = 0; j < 1000; j++) {
            px = x;
            py = y;

            x = px * px - py * py;
            y = 2 * px * py;

            x -= 1;

            if (x*x + y*y >= 4) {
                color = 0; 
                break;
            }
        }

        //SR_LOG_DEB("c: %f %f", __real__ c, __imag__ c);
        t[0].data[3*i] = color; 
        t[0].data[3*i+2] = color;
        t[0].data[3*i+1] = color;
    }
    EndUpdateDynTexture(&t[0]);

    unsigned int frameCounter = 0;
    double last = 0.0;
    bool flip = FALSE;
    sm_vec2d center = (sm_vec2d){0, 0};
    double zoom = 1.0f;
    while (!glfwWindowShouldClose(sr_context.w)) {
        bool rerender = FALSE;
        glfwPollEvents();


        SpriteEntry* s = GetSprite(r, s1);
        if (glfwGetKey(sr_context.w, GLFW_KEY_W) == GLFW_PRESS) {
            center.y += 0.1 * zoom;
            rerender = TRUE;
        }
        if (glfwGetKey(sr_context.w, GLFW_KEY_A) == GLFW_PRESS) {
            center.x += 0.1 * zoom;
            rerender = TRUE;
        }
        if (glfwGetKey(sr_context.w, GLFW_KEY_S) == GLFW_PRESS) {
            center.y -= 0.1 * zoom;
            rerender = TRUE;
        }
        if (glfwGetKey(sr_context.w, GLFW_KEY_D) == GLFW_PRESS) {
            center.x -= 0.1 * zoom;
            rerender = TRUE;
        }
        
        if (glfwGetKey(sr_context.w, GLFW_KEY_H) == GLFW_PRESS) {
            s->size = sm_vec2_f32_mul(s->size, (sm_vec2f){0.999, 0.999});
            rerender = TRUE;
        }
        if (glfwGetKey(sr_context.w, GLFW_KEY_J) == GLFW_PRESS) {
            s->size = sm_vec2_f32_mul(s->size, (sm_vec2f){1.001, 1.001});
            rerender = TRUE;
        }

        if (rerender) {
            BeginUpdateDynTexture(&t[0]);

            for (int i = 0; i < t[0].size/3; i += 1) {
                double x = (double)((i) % 2000) / 2000.0;
                double y = (((double)(i)) / 2000) / 2000.0;

                x -= 0.5;
                y -= 0.5;
                x -= center.x;
                y -= center.y;
                x *= zoom;
                y *= zoom;

                double px, py;
                int color = 255;
                for (int j = 0; j < 1000; j++) {
                    px = x;
                    py = y;

                    x = px * px - py * py;
                    y = 2 * px * py;

                    x -= 1;

                    if (x*x + y*y >= 4) {
                        color = 0; 
                        break;
                    }
                }

                //SR_LOG_DEB("c: %f %f", __real__ c, __imag__ c);
                t[0].data[3*i] = color; 
                t[0].data[3*i+2] = color;
                t[0].data[3*i+1] = color;
            }
            EndUpdateDynTexture(&t[0]);
        }




        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;


        StartFrame(&p, frameCounter);
        SpriteDrawFrame(r, &p, frameCounter);
        SubmitFrame(&p, frameCounter); 



    }

    DestroyDynTexture(&t[0]);
    DestroyPresent(&p);
    SpriteDestroy(r);
    free(r);
    DestroyVulkan();
    return 0;
}
