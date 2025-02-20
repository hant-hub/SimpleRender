#include "config.h"
#include "frame.h"
#include "init.h"
#include "text.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include <GLFW/glfw3.h>
#include "sprite.h"


int main() {

    SpriteRenderer* r = calloc(sizeof(SpriteRenderer), 1);
    TextRenderer* t = calloc(sizeof(TextRenderer), 1);
    PresentInfo p = {0};
    CRASH_CALL(CreateVulkan());

    Attachment attachments[SR_SPRITE_ATTACHMENT_NUM + SR_TEXT_NUM_ATTACHMENTS] = {0};
    SubPass passes[2] = {0};

    SpriteGetSubpass(passes, attachments, 0);
    TextGetSubpass(passes, attachments, SR_SPRITE_ATTACHMENT_NUM);
    CRASH_CALL(InitPresent(&p, passes, 2, attachments, 2));

    CRASH_CALL(SpriteInit(r, &p.p, 0, (Camera){.pos = {0, 0}, .size = {fWIDTH, fWIDTH}, .rotation = 0}, 2));
    Font f;
    CRASH_CALL(LoadFont("resources/fonts/JetBrainsMonoNLNerdFontPropo-Regular.ttf", 60, &f));
    CRASH_CALL(TextInit(t, &f, 60, &p.p, 1))
    SetArea(t, (sm_vec2f){fWIDTH, fWIDTH});

    //build multipass


    Texture textures[2] = {0};

    CRASH_CALL(LoadTexture(&textures[1], "resources/textures/Chisel.png"));
    CRASH_CALL(LoadTexture(&textures[0], "resources/textures/texture.jpg"));
    
    CRASH_CALL(SetTextureSlots(r, textures, ARRAY_SIZE(textures)));

    SpriteHandle s2 = CreateSprite(r, (sm_vec2f){0.0f, 0.0f}, (sm_vec2f){fWIDTH, fHEIGHT}, 0, 2);
    SpriteHandle s1 = CreateSprite(r, (sm_vec2f){0.0f, 0.0f}, (sm_vec2f){20, 60}, 1, 1);
    //SpriteHandle s3 = CreateSprite(&r, (sm_vec2f){50.0f, 0.0f}, (sm_vec2f){100, 100}, 1);
    AppendText(t, "test", 4, (sm_vec2f){10, 10}, 1.0, 10);


    unsigned int frameCounter = 0;
    double last = 0.0;
    bool flip = FALSE;
    double times[10] = {0};
    int top = 0;
    while (!glfwWindowShouldClose(sr_context.w)) {
        glfwPollEvents();

        double start = glfwGetTime();

        SpriteEntry* e = GetSprite(r, s1);
        //e->rotation += 0.001f;
        e->pos = GetTextPos(t);
        e->pos.y -= e->size.y/2;
        e->pos.x += e->size.x;
        if (glfwGetTime() > last + 0.5) { 
            e->layer = flip ? 1 : 3;
            last = glfwGetTime();
            flip = !flip;
        }

        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(&p, frameCounter);
        SpriteDrawFrame(r, &p, frameCounter);
        NextPass(&p, frameCounter);
        TextDrawFrame(t, &p, frameCounter);
        SubmitFrame(&p, frameCounter); 

        times[top] = (glfwGetTime() - start);
        top = (top + 1) % 10;

        double avg = 0.0;
        for (int i = 0; i < 10; i++) {
            avg += times[i];
        }
        avg /= 10;
        avg = 1.0/avg;

        char buf[32];
        int len = snprintf(buf, 32, "fps: %.3f", avg);
        ClearText(t);
        SetColor(t, (sm_vec3f){sin(start), sin(start) * cos(start), cos(start) * cos(start)});
        AppendText(t, buf, len-1, (sm_vec2f){-400, -240}, 1.0, 1);

        start = glfwGetTime()*1.2;

        SetColor(t, (sm_vec3f){sin(start), sin(start) * cos(start), cos(start) * cos(start)});
        AppendText(t, "\nTest\n\nw", 8, (sm_vec2f){-400, -240}, 3.0 * sin(start), 1);

    }

    DestroyTexture(&textures[0]);
    DestroyTexture(&textures[1]);
    DestroyPresent(&p);
    SpriteDestroy(r);
    TextDestroy(t);
    free(r);
    free(t);
    DestroyVulkan();
    return 0;
}
