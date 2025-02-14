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

    SpriteRenderer* r = malloc(sizeof(SpriteRenderer));
    TextRenderer* t = malloc(sizeof(TextRenderer));
    PresentInfo p = {0};
    CRASH_CALL(CreateVulkan());

    Attachment attachments[SR_SPRITE_ATTACHMENT_NUM * 2] = {0};
    SubPass passes[2] = {0};

    SpriteGetSubpass(passes, attachments, 0);
    TextGetSubpass(passes, attachments, SR_SPRITE_ATTACHMENT_NUM);
    CRASH_CALL(InitPresent(&p, passes, 2, attachments, 2));

    CRASH_CALL(SpriteInit(r, &p.p, 0, (Camera){.pos = {0, 0}, .size = {WIDTH, HEIGHT}, .rotation = 0}, 2));
    CRASH_CALL(TextInit(t, "resources/fonts/JetBrainsMonoNLNerdFontPropo-Regular.ttf", 60, &p.p, 1))
    SetArea(t, (sm_vec2f){WIDTH, HEIGHT});

    //build multipass


    Texture textures[2] = {0};

    CRASH_CALL(LoadTexture(&textures[1], "resources/textures/duck.jpg"));
    CRASH_CALL(LoadTexture(&textures[0], "resources/textures/texture.jpg"));
    
    CRASH_CALL(SetTextureSlots(r, textures, ARRAY_SIZE(textures)));

    SpriteHandle s1 = CreateSprite(r, (sm_vec2f){50.0f, 0.0f}, (sm_vec2f){100, 100}, 0, 1);
    SpriteHandle s2 = CreateSprite(r, (sm_vec2f){0.0f, 0.0f}, (sm_vec2f){WIDTH, HEIGHT}, 0, 0);
    //SpriteHandle s3 = CreateSprite(&r, (sm_vec2f){50.0f, 0.0f}, (sm_vec2f){100, 100}, 1);
    AppendText(t, "test", 4, (sm_vec2f){10, 10}, 10);


    unsigned int frameCounter = 0;
    double last = 0.0;
    bool flip = FALSE;
    double times[10] = {0};
    int top = 0;
    while (!glfwWindowShouldClose(sr_context.w)) {
        glfwPollEvents();

        double start = glfwGetTime();
        SpriteEntry* e = GetSprite(r, s1);
        e->rotation += 1.0f * times[(top + 9) % 10];
        if (glfwGetTime() > last + 3.0) { 
            SpriteEntry *s = GetSprite(r, s2);
            s->layer += flip ? -2 : 2;
            s->texture = flip ? 0 : 1;
            last = glfwGetTime();
            flip = !flip;
        }
        float time = glfwGetTime() * 2;
        //SetColor(t, (sm_vec3f){sin(time) * sin(time), sin(time) * cos(time), cos(time)});

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

        char buf[16];
        int len = snprintf(buf, 16, "fps: %f", avg);
        ClearText(t);
        AppendText(t, buf, len-1, (sm_vec2f){-400, -240}, 1);

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
