#include "sprite.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include <string.h>
#include <mat4.h>


typedef struct {
    sm_mat4f model;
//    i32 texture;
} SpriteEntry;


static SpriteEntry denseSetVals[SR_MAX_INSTANCES] = {0};
static u32 denseSetIdx[SR_MAX_INSTANCES] = {0};
static i32 sparseSet[SR_MAX_INSTANCES] = {0};
static u32 denseSize = 0;


SpriteHandle CreateSprite(sm_vec3f pos, sm_vec3f size) {
    //sparse set is index + 1,
    //all valid handles must be >0, since 0 is
    //a tombstone value
    
    
    u32 newIndex = 0;
    while (newIndex < SR_MAX_INSTANCES && sparseSet[newIndex++] != 0);
    if (newIndex >= SR_MAX_INSTANCES) {
        SR_LOG_ERR("Failed to Create Sprite! Not Enough Instances");
        return -1;
    }
    u32 denseIndex = denseSize;
    sparseSet[--newIndex] = ++denseSize; 


    sm_mat4f scaler = {
        {size.x, 0,      0,      0},
        {0,      size.y, 0,      0},
        {0,      0,      size.z, 0},
        {0,      0,      0,      1.0f}
    };


    sm_mat4_f32_identity(&denseSetVals[denseIndex].model);
    sm_mat4_f32_comp(&denseSetVals[denseIndex].model, &denseSetVals[denseIndex].model, &scaler);
    sm_mat4_f32_translate(&denseSetVals[denseIndex].model, pos); 

    denseSetIdx[denseIndex] = newIndex;

    return newIndex;
}

ErrorCode DestroySprite(SpriteHandle s) {

    u32 denseIndex = sparseSet[s] - 1;
    {
        SpriteEntry temp = denseSetVals[denseSize - 1];
        denseSetVals[denseSize - 1] = denseSetVals[denseIndex];
        denseSetVals[denseIndex] = temp;
    }

    {
        u32 temp = denseSetIdx[denseSize - 1];
        denseSetIdx[denseSize - 1] = denseSetIdx[denseIndex];
        denseSetIdx[denseIndex] = temp;
    }

    denseSize -= 1;
    sparseSet[s] = 0;

    return SR_NO_ERROR;
}



ErrorCode PushBuffer(void* buf) {

    memcpy(buf, denseSetVals, sizeof(SpriteEntry) * denseSize);
    return SR_NO_ERROR;
}
sm_mat4f* GetModel(SpriteHandle s) {
    if (!sparseSet[s]) return NULL;
    return &denseSetVals[(sparseSet[s] - 1)].model;
}



u32 GetNum() {
    return denseSize;
}
