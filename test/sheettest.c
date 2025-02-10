#include "config.h"
#include "frame.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include <GLFW/glfw3.h>
#include "sheet.h"


int main() {

    CRASH_CALL(CreateVulkan());

    Attachment attachments[SR_SHEET_ATTACHMENT_NUM];
    SubPass passes[1];

    SpriteGetSubpass(passes, attachments, 0);

    PresentInfo p = {0};
    CRASH_CALL(InitPresent(&p, passes, 1, attachments, 1));

    SpriteRenderer* r = malloc(sizeof(SpriteRenderer));
    CRASH_CALL(SpriteInit(r, &p.p, 0, (Camera){.pos = {0, 0}, .size = {100, 100}, .rotation = 0}, 2));

    //build multipass


    Texture textures[2] = {0};

    CRASH_CALL(LoadTexture(&textures[1], "resources/textures/duck.jpg"));
    CRASH_CALL(LoadTexture(&textures[0], "resources/textures/texture.jpg"));
    
    CRASH_CALL(SetTextureSlots(r, textures, ARRAY_SIZE(textures)));

    SheetHandle s1 = CreateSprite(r, (sm_vec2f){50.0f, 0.0f}, (sm_vec2f){100, 100}, 0, 1);
    SheetHandle s2 = CreateSprite(r, (sm_vec2f){0.0f, 0.0f}, (sm_vec2f){100, 100}, 1, 0);
    //SpriteHandle s3 = CreateSprite(&r, (sm_vec2f){50.0f, 0.0f}, (sm_vec2f){100, 100}, 1);


    unsigned int frameCounter = 0;
    double last = 0.0;
    bool flip = FALSE;
    while (!glfwWindowShouldClose(sr_context.w)) {
        glfwPollEvents();

        SheetEntry* e = GetSprite(r, s1);
        e->rotation += 0.001f;
        if (glfwGetTime() > last + 3.0) { 
            SheetEntry *s = GetSprite(r, s2);
            s->layer += flip ? -2 : 2;
            s->texture = flip ? 0 : 1;
            last = glfwGetTime();
            flip = !flip;
        }

        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(&p, frameCounter);
        SpriteDrawFrame(r, &p, frameCounter);
        SubmitFrame(&p, frameCounter); 

    }

    DestroyTexture(&textures[0]);
    DestroyTexture(&textures[1]);
    DestroyPresent(&p);
    SpriteDestroy(r);
    free(r);
    DestroyVulkan();
    return 0;
}
