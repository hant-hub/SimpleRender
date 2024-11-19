#include "frame.h"
#include "init.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>
#include "sprite.h"
#include "text.h"


int main() {

    SpriteRenderer r = {0};
    TextRenderer t = {0};
    PresentInfo p = {0};
    CRASH_CALL(CreateVulkan());
    //CRASH_CALL(SpriteInit(&r,(Camera){.pos = {0, 0}, .size = {100, 100}, .rotation = 0}, 2));
    CRASH_CALL(TextInit(&t));
    CRASH_CALL(InitPresent(&p, &t.pass));

    FontData font;
    Texture tex;

    CRASH_CALL(LoadTexture(&tex, "resources/textures/texture.jpg"));
    CRASH_CALL(LoadFont(&font));
    
    //CRASH_CALL(SetTextureSlot(&r, &tex, 0));
    //CRASH_CALL(SetTextureSlot(&r, &font.atlas, 1));

    //SpriteHandle s1 = CreateSprite(&r, (sm_vec2f){0.0f, 0.0f}, (sm_vec2f){100, 100}, 0, 1);
    //SpriteHandle s2 = CreateSprite(&r, (sm_vec2f){0.0f, 0.0f}, (sm_vec2f){100, 100}, 1, 0);

    char text[1000];

    unsigned int frameCounter = 0;
    double last = 0.0;
    bool flip = FALSE;
    double prev = 0;
    double next = 1;
    double period = 0.0f;
    while (!glfwWindowShouldClose(sr_context.w)) {
        glfwPollEvents();
        
        float period = (period + (float)(next - prev))/2;
        prev = next;
        next = glfwGetTime();

        int size = snprintf(text, 999, "Frame: %2.f", 1/period);
        UpdateText(&t, text, size);

        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        GetFrame(&p, frameCounter);
        TextDrawFrame(&t, &p, frameCounter);
        PresentFrame(&p, frameCounter);

    }

    DestroyTexture(&font.atlas);
    DestroyTexture(&tex);
    DestroyPresent(&p);
    //SpriteDestroy(&r);
    TextDestroy(&t);
    DestroyVulkan();
    return 0;
}
