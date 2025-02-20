#include "config.h"
#include "frame.h"
#include "init.h"
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
    TextRenderer* t = calloc(sizeof(TextRenderer), 1);
    PresentInfo p = {0};

    CRASH_CALL(CreateVulkan());

    Attachment attachments[SR_TEXT_NUM_ATTACHMENTS];
    SubPass passes[1];

    TextGetSubpass(passes, attachments, 0);
    CRASH_CALL(InitPresent(&p, passes, 1, attachments, SR_TEXT_NUM_ATTACHMENTS));
    //CRASH_CALL(SpriteInit(&r,(Camera){.pos = {0, 0}, .size = {100, 100}, .rotation = 0}, 2));
    Font f;
    CRASH_CALL(LoadFont("./resources/fonts/JetBrainsMonoNLNerdFontPropo-Regular.ttf", 60, &f));
    CRASH_CALL(TextInit(t, &f, 60, &p.p, 0));
    
    //CRASH_CALL(SetTextureSlot(&r, &tex, 0));
    //CRASH_CALL(SetTextureSlot(&r, &font.atlas, 1));

    //SpriteHandle s1 = CreateSprite(&r, (sm_vec2f){0.0f, 0.0f}, (sm_vec2f){100, 100}, 0, 1);
    //SpriteHandle s2 = CreateSprite(&r, (sm_vec2f){0.0f, 0.0f}, (sm_vec2f){100, 100}, 1, 0);


    SetArea(t, (sm_vec2f){100, 100});
    SetColor(t, (sm_vec3f){1.0, 1.0, 1.0});

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
                AppendText(t, "Blam", 4, (sm_vec2f){50 * i, 50 * j + shift}, i + 10, 0.3 * sin(scale + (float)j/5) + 0.5);
            }
        }

        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(&p, frameCounter);
        TextDrawFrame(t, &p, frameCounter);
        SubmitFrame(&p, frameCounter);

        

    }

    DestroyFont(&f);
    TextDestroy(t);
    free(t);
    DestroyPresent(&p);
    DestroyVulkan();
    return 0;
}
