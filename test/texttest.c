#include "frame.h"
#include "init.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>
#include "sprite.h"
#include "text.h"


int main() {

    SpriteRenderer r = {0};
    TextRenderer* t = malloc(sizeof(TextRenderer));
    PresentInfo p = {0};

    CRASH_CALL(CreateVulkan());

    SubPass passes[1];

    TextGetSubpass(passes, NULL, 0);
    CRASH_CALL(InitPresent(&p, passes, 1, NULL, 0));
    //CRASH_CALL(SpriteInit(&r,(Camera){.pos = {0, 0}, .size = {100, 100}, .rotation = 0}, 2));
    CRASH_CALL(TextInit(t, &p.p, 0));

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
    while (!glfwWindowShouldClose(sr_context.w)) {
        glfwPollEvents();
        
        ClearText(t);
        for (int i = -10; i < 100; i++) {
            for (int j = -10; j < 50; j++) {
                float shift = glfwGetTime();
                float scale = shift * 2;
                shift = shift - floor(shift);
                shift *= 10;
                AppendText(t, "Blam", 4, (sm_vec2f){15 * i, 5 * j + shift}, 0.3 * sin(scale + (float)j/5) + 0.5);
            }
        }

        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(&p, frameCounter);
        TextDrawFrame(t, &p, frameCounter);
        SubmitFrame(&p, frameCounter);

        

    }

    DestroyTexture(&font.atlas);
    DestroyTexture(&tex);
    DestroyPresent(&p);
    //SpriteDestroy(&r);
    TextDestroy(t);
    free(t);
    DestroyVulkan();
    return 0;
}
