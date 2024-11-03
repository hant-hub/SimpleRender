#include "text.h"

#include "error.h"
#include "util.h"
#include "vec2.h"
#include "vec4.h"

typedef struct {
    sm_vec2f pos;
    sm_vec2f size;
} Vertex;

typedef struct {
    uint glyph;
    f32 rotation;
    f32 scale;
    //4 bytes of additional padding
} __attribute__ ((aligned(sizeof(sm_vec4f)))) Character;



ErrorCode TextInit(TextRenderer* r) {
    return SR_NO_ERROR;
}

void TextDestroy(TextRenderer* r) {
}

ErrorCode UpdateText(TextRenderer* r) {
    return SR_NO_ERROR;
}

void TextDrawFrame(TextRenderer* r) {
}
