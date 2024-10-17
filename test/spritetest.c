#include "common.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include <GLFW/glfw3.h>
#include "sprite.h"


int main() {

    RenderState r = {0};
    CRASH_CALL(CreateVulkan());
    CRASH_CALL(SpriteInit(&r,(Camera){.pos = {0, 0}, .size = {100, 100}, .rotation = 0}, 2));

    Texture textures[2] = {0};

    CRASH_CALL(LoadTexture(&r.cmd, &textures[0], "resources/textures/texture.jpg"));
    CRASH_CALL(LoadTexture(&r.cmd, &textures[1], "resources/textures/duck.jpg"));
    
    CRASH_CALL(SetTextureSlots(&r, textures, ARRAY_SIZE(textures)));

    //SpriteHandle s1 = CreateSprite(&r, (sm_vec2f){-100.0f, 0.0f}, (sm_vec2f){100, 100}, 0);
    //SpriteHandle s2 = CreateSprite(&r, (sm_vec2f){0.0f, 0.0f}, (sm_vec2f){100, 100}, 1);
    //SpriteHandle s3 = CreateSprite(&r, (sm_vec2f){50.0f, 0.0f}, (sm_vec2f){100, 100}, 1);
    SpriteHandle sprites[15000];
    for (int i = 0; i < 15000; i++) {
        u32 x = i % 200;
        u32 y = i / 200;
        sprites[i] = CreateSprite(&r, (sm_vec2f){300 - (3.0f * x), 100 + (-3.0f * y)}, (sm_vec2f){5, 5}, 0, 1);
        SpriteEntry* e = GetSprite(&r, sprites[i]);
        e->rotation = ((float)rand()/RAND_MAX * SM_2_PI) - SM_PI;
        e->pos.x += ((float)rand()/RAND_MAX * 10.0f) - 5.0f;
        e->pos.y += ((float)rand()/RAND_MAX * 10.0f) - 5.0f;
        e->size.x = ((float)rand()/RAND_MAX * 10.0f) + 3.0f;
        e->size.y = ((float)rand()/RAND_MAX * 10.0f) + 3.0f;
    }


    unsigned int frameCounter = 0;
    while (!glfwWindowShouldClose(sr_context.w)) {
        glfwPollEvents();

        for (int i = 0; i < 15000; i++) {
            SpriteEntry* e = GetSprite(&r, sprites[i]);
            e->rotation += ((float)(i % 400))/10000.0f;
            e->pos.x += sin((float)i + glfwGetTime())/5.0f;
            e->pos.y += cos((float)i - glfwGetTime())/5.0f;
            e->size.x += sin((float)i + glfwGetTime())/5.0f;
            e->size.y += sin((float)i + glfwGetTime())/5.0f;
        }                

        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        DrawFrame(&r, frameCounter);

    }

    DestroyTexture(&textures[0]);
    DestroyTexture(&textures[1]);
    SpriteDestroy(&r);
    DestroyVulkan();
    return 0;
}
