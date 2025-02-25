#include "config.h"
#include "frame.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "sheet.h"


int main() {

    CRASH_CALL(CreateVulkan());

    Attachment attachments[SR_SHEET_ATTACHMENT_NUM];
    SubPass passes[1];

    SheetGetSubpass(passes, attachments, 0);

    PresentInfo p = {0};
    CRASH_CALL(InitPresent(&p, passes, 1, attachments, 1));

    SheetRenderer* r = calloc(sizeof(SheetRenderer), 1);
    CRASH_CALL(SheetInit(r, &p.p, 0, (Sh_Camera){.pos = {0, 0}, .size = {100, 100}, .rotation = 0}, 2));

    //build multipass


    Texture textures[2] = {0};

    CRASH_CALL(LoadTexture(&textures[1], "resources/textures/duck.jpg"));
    CRASH_CALL(LoadTextureConfig(&textures[0], "resources/textures/Whale Boi.png", (TextureConfig) {
                .anisotropy = FALSE,
                .accessmode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                .filter = VK_FILTER_NEAREST
                }));
    
    CRASH_CALL(SHSetTextureSlots(r, textures, ARRAY_SIZE(textures)));

    SheetHandle s1 = CreateSpriteSh(r, (sm_vec2f){0.0f, 0.0f}, (sm_vec2f){100, 50}, 0, 4);
    //SpriteHandle s3 = CreateSprite(&r, (sm_vec2f){50.0f, 0.0f}, (sm_vec2f){100, 100}, 1);
    {
        SheetEntry* e = GetSpriteSh(r, s1);
        e->scale = (sm_vec2f) {
            1, 1
        };
        e->selection = (sm_vec2f) {
            0, 0
        };
    }

    unsigned int frameCounter = 0;
    double last = 0.0;
    bool flip = FALSE;
    while (!glfwWindowShouldClose(sr_context.w)) {
        glfwPollEvents();


        //e->rotation += 0.01f;
        if (glfwGetTime() > last + 0.08) { 
            SheetEntry* e = GetSpriteSh(r, s1);
            e->selection.x -= 1;
            if (!flip && e->selection.x < -6) {
                flip = TRUE;
                e->selection.x = 0;
                e->selection.y = 1;
            }
            if (flip && e->selection.x < -5) {
                flip = FALSE;
                e->selection.x = 0;
                e->selection.y = 0;
            }
            //s->layer += flip ? -2 : 2;
            //s->texture = flip ? 0 : 1;
            last = glfwGetTime();

            e->color = (sm_vec3f){sin(last) * cos(last), sin(last) * sin(last), cos(last)};
        }

        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(&p, frameCounter);
        SheetDrawFrame(r, &p, frameCounter);
        SubmitFrame(&p, frameCounter); 

    }

    DestroyTexture(&textures[0]);
    DestroyTexture(&textures[1]);
    DestroyPresent(&p);
    SheetDestroy(r);
    free(r);
    DestroyVulkan();
    return 0;
}
