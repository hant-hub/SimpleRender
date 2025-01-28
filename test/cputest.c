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
#include "pthread.h"

#define NUM_THREADS 16

typedef struct {
    size_t size;
    char* data;
    int offset;
    sm_vec2d center;
    double zoom;
} thread_data;

void* render_thread(void* argp) {
    thread_data* d = argp;

    size_t size = d->size;
    char* data = d->data;
    sm_vec2d center = d->center;
    double zoom = d->zoom;
    int offset = d->offset;
    for (int i = offset; i < size/3; i += NUM_THREADS) {
        double x = (double)((i) % 1000) / 1000.0;
        double y = (((double)(i)) / 1000) / 1000.0;

        x -= 0.5;
        y -= 0.5;
        x *= 2.0f;
        y *= 2.0f;
        x *= zoom;
        y *= zoom;
        x -= center.x;
        y -= center.y;

        double px, py;
        float color = 0;
        for (int j = 0; j < 100; j++) {
            px = x;
            py = y;

            x = px * px - py * py;
            y = 2 * px * py;
            x -= 1;
            if (x*x + y*y >= 4) {
                color = 255 * (float)j; 
                color /= 50;
                break;
            }
        }

        //SR_LOG_DEB("c: %f %f", __real__ c, __imag__ c);
        data[3*i] = color * 0.9; 
        data[3*i+1] = color * 0.2;
        data[3*i+2] = color * 0.9;



    }
    //pthread_exit(0); 
    return NULL;
}

int main() {

    CRASH_CALL(CreateVulkan());

    Attachment attachments[SR_SPRITE_ATTACHMENT_NUM];
    SubPass passes[1];

    SpriteGetSubpass(passes, attachments, 0);

    PresentInfo p = {0};
    CRASH_CALL(InitPresent(&p, passes, 1, attachments, 1));

    SpriteRenderer* r = malloc(sizeof(SpriteRenderer));
    CRASH_CALL(SpriteInit(r, &p.p, 0, (Camera){.pos = {0, 0}, .size = {WIDTH, HEIGHT}, .rotation = 0}, 1));

    //build multipass


    Texture textures[2] = {0};
    DynamicTexture t[2] = {0};

    CRASH_CALL(CreateDynTexture(&t[0], (TextureConfig){
                    .width = 1000,
                    .height = 1000,
                    .filter = VK_FILTER_LINEAR,
                    .format = VK_FORMAT_R8G8B8_SRGB,
                    .channels = 3,
                    .accessmode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                    .anisotropy = VK_FALSE,
                }));
//    CRASH_CALL(CreateDynTexture(&t[1], (TextureConfig){
//                    .width = 1000,
//                    .height = 1000,
//                    .filter = VK_FILTER_LINEAR,
//                    .format = VK_FORMAT_R8G8B8_SRGB,
//                    .channels = 3,
//                    .accessmode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
//                    .anisotropy = VK_FALSE,
//                }));


    textures[0] = t[0].t;    
//    textures[1] = t[1].t;
    CRASH_CALL(SetTextureSlots(r, textures, 1));


    SpriteHandle s1 = CreateSprite(r, (sm_vec2f){0.0f, 0.0f}, (sm_vec2f){WIDTH*2, HEIGHT*2}, 0, 0);
    //SpriteHandle s3 = CreateSprite(&r, (sm_vec2f){50.0f, 0.0f}, (sm_vec2f){100, 100}, 1);


    unsigned int frameCounter = 0;
    double last = 0.0;
    bool flip = FALSE;
    sm_vec2d center = (sm_vec2d){0, 0};
    double zoom = 4.0f;
    int active = 0;


    double startperf = glfwGetTime();
    double endperf = 0;
    BeginUpdateDynTexture(&t[0]);
    thread_data args[NUM_THREADS];
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        args[i] = (thread_data){
            .offset = i,
                .data = t[0].data,
                .size = t[0].size,
                .center = center,
                .zoom = zoom
        };
        pthread_create(&threads[i], NULL, render_thread, &args[i]); 
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    EndUpdateDynTexture(&t[0]);
    endperf = glfwGetTime();
    SR_LOG_DEB("Render time: %f", endperf - startperf);

    while (!glfwWindowShouldClose(sr_context.w)) {
        bool rerender = FALSE;
        glfwPollEvents();


        SpriteEntry* s = GetSprite(r, s1);
        if (glfwGetKey(sr_context.w, GLFW_KEY_W) == GLFW_PRESS) {
            center.y += 0.05 * zoom;
            rerender = TRUE;
        }
        if (glfwGetKey(sr_context.w, GLFW_KEY_A) == GLFW_PRESS) {
            center.x += 0.05 * zoom;
            rerender = TRUE;
        }
        if (glfwGetKey(sr_context.w, GLFW_KEY_S) == GLFW_PRESS) {
            center.y -= 0.05 * zoom;
            rerender = TRUE;
        }
        if (glfwGetKey(sr_context.w, GLFW_KEY_D) == GLFW_PRESS) {
            center.x -= 0.05 * zoom;
            rerender = TRUE;
        }
        
        if (glfwGetKey(sr_context.w, GLFW_KEY_H) == GLFW_PRESS) {
            //s->size = sm_vec2_f32_mul(s->size, (sm_vec2f){0.999, 0.999});
            zoom *= 1.1f;
            rerender = TRUE;
        }
        if (glfwGetKey(sr_context.w, GLFW_KEY_J) == GLFW_PRESS) {
            //s->size = sm_vec2_f32_mul(s->size, (sm_vec2f){1.001, 1.001});
            zoom *= 0.9f;
            rerender = TRUE;
        }


        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;


        StartFrame(&p, frameCounter);
        SpriteDrawFrame(r, &p, frameCounter);
        SubmitFrame(&p, frameCounter); 

        if (rerender) {
            BeginUpdateDynTexture(&t[0]);

            for (int i = 0; i < NUM_THREADS; i++) {
                args[i] = (thread_data){
                        .offset = i,
                        .data = t[0].data,
                        .size = t[0].size,
                        .center = center,
                        .zoom = zoom
                };
                pthread_create(&threads[i], NULL, render_thread, &args[i]); 
            }
            for (int i = 0; i < NUM_THREADS; i++) {
                pthread_join(threads[i], NULL);
            }
            EndUpdateDynTexture(&t[0]);
        }


    }

    DestroyDynTexture(&t[0]);
    DestroyPresent(&p);
    SpriteDestroy(r);
    free(r);
    DestroyVulkan();
    return 0;
}
