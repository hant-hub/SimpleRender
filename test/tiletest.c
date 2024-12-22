#include "tile.h"
#include "config.h"
#include "frame.h"
#include "util.h"
#include <GLFW/glfw3.h>


int main() {

    PresentInfo p = {0};
    TileRenderer t = {0};
    CRASH_CALL(CreateVulkan());

    Attachment attachments[SR_TILE_ATTACHMENT_NUM];
    SubPass passes[1];

    TileGetSubpass(passes, attachments, 0);
    CRASH_CALL(InitPresent(&p, passes, 1, attachments, SR_TILE_ATTACHMENT_NUM));

    CRASH_CALL(TileInit(&t, &p.p, "resources/tilesets/example.jpg",0, (sm_vec2i){6, 3}, (sm_vec2i){3, 3}));



    
    int data[9] = {0, 4, 2,
                    9, 7, 11,
                    12, 16, 14};
    TileSetData(&t, data);


    unsigned int frameCounter = 0;
    double last = 0.0;
    bool flip = FALSE;
    unsigned int counter = 5000;
    unsigned int second = 0;
    while (!glfwWindowShouldClose(sr_context.w)) {
        glfwPollEvents();

        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(&p, frameCounter);
        TileDrawFrame(&t, &p, frameCounter);
        SubmitFrame(&p, frameCounter); 

    }

    
    DestroyTile(&t);
    DestroyPresent(&p);
    DestroyVulkan();
    return 0;
}
