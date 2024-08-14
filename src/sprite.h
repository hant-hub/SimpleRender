#ifndef SR_SPRITE_H
#define SR_SPRITE_H


#include "common.h"
#include <mat4.h>
#include "error.h"

//sprite ID
typedef i32 SpriteHandle;


SpriteHandle CreateSprite(sm_vec3f pos, sm_vec3f size);
ErrorCode DestroySprite(SpriteHandle s);

ErrorCode PushBuffer(void* buf);
sm_mat4f* GetModel(SpriteHandle s);
u32 GetNum();








#endif
