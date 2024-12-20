#include "config.h"
#include "fractal.h"
#include "frame.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include <GLFW/glfw3.h>
#include "sprite.h"


int main() {

    FractalRenderer* r = malloc(sizeof(FractalRenderer));
    PresentInfo p = {0};
    CRASH_CALL(CreateVulkan());

    Attachment attachments[SR_SPRITE_ATTACHMENT_NUM];
    SubPass passes[1];

    FractalGetSubpass(passes, attachments, 0);
    CRASH_CALL(InitPresent(&p, passes, 1, attachments, 1));
    CRASH_CALL(FractalInit(r, &p.p, 0));

    //build multipass

    unsigned int frameCounter = 0;
    double last = 0.0;
    bool flip = FALSE;
    while (!glfwWindowShouldClose(sr_context.w)) {
        glfwPollEvents();

        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(&p, frameCounter);
        FractalDrawFrame(r, &p, frameCounter);
        SubmitFrame(&p, frameCounter); 

    }

    FractalDestroy(r);
    DestroyPresent(&p);
    free(r);
    DestroyVulkan();
    return 0;
}
