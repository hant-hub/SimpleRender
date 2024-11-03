#include "common.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include <GLFW/glfw3.h>
#include "sprite.h"


int main() {

    SpriteRenderer r = {0};
    CRASH_CALL(CreateVulkan());
    CRASH_CALL(SpriteInit(&r,(Camera){.pos = {0, 0}, .size = {100, 100}, .rotation = 0}, 2));

    Texture textures[2] = {0};

    CRASH_CALL(LoadTexture(&textures[0], "resources/textures/texture.jpg"));
    CRASH_CALL(LoadTexture(&textures[1], "resources/textures/duck.jpg"));
    
    CRASH_CALL(SetTextureSlots(&r, textures, ARRAY_SIZE(textures)));

    SpriteHandle s1 = CreateSprite(&r, (sm_vec2f){50.0f, 0.0f}, (sm_vec2f){100, 100}, 0, 1);
    SpriteHandle s2 = CreateSprite(&r, (sm_vec2f){0.0f, 0.0f}, (sm_vec2f){100, 100}, 1, 0);
    //SpriteHandle s3 = CreateSprite(&r, (sm_vec2f){50.0f, 0.0f}, (sm_vec2f){100, 100}, 1);


    unsigned int frameCounter = 0;
    double last = 0.0;
    bool flip = FALSE;
    while (!glfwWindowShouldClose(sr_context.w)) {
        glfwPollEvents();

        if (glfwGetTime() > last + 3.0) { 
            SpriteEntry *s = GetSprite(&r, s2);
            s->layer += flip ? -2 : 2;
            s->texture = flip ? 0 : 1;
            last = glfwGetTime();
            flip = !flip;
        }

        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        SpriteDrawFrame(&r, frameCounter);

    }

    DestroyTexture(&textures[0]);
    DestroyTexture(&textures[1]);
    SpriteDestroy(&r);
    DestroyVulkan();
    return 0;
}
