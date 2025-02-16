#include "frame.h"
#include "init.h"
#include "log.h"
#include "tile.h"
#include "text.h"
#include "util.h"
#include "vec2.h"
#include "stdio.h"
#include <GLFW/glfw3.h>
#include <time.h>
#define B_WIDTH 10
#define B_HEIGHT 10
#define B_SIZE B_WIDTH * B_HEIGHT

static const char* ships[5] = {
    "Carrier",
    "Battleship",
    "Cruiser",
    "Submarine",
    "Destroyer"
};

static const int ship_sizes[] = {
    5,
    4,
    3,
    3,
    2
};


typedef struct {
    char p_board[B_SIZE];
    //AI doesn't need a view
    char a_board[B_SIZE];

    //status of current ships
    char p_ships[5];
    char a_ships[5];

    bool gameover;
} GameState;

typedef enum {
    EMPTY = 0,
    CARRIER,
    BATTLE,
    CRUISER,
    SUBMARINE,
    DESTROYER,
    HIT,
    MISS
} TileState;

static int screen[2 * B_SIZE];

sm_vec2i getInput(bool* selected);
void setTile(GameState* g);
void PlaceShip(GameState* s, TextRenderer* t, int* shipsPlaced, sm_vec2i pos);
void TakeTurn(GameState* s, TextRenderer* t, sm_vec2i pos);

int main() {
    //initialize gamestate
    GameState s = {0};    
    s.p_ships[0] = 5;
    s.p_ships[1] = 4;
    s.p_ships[2] = 3;
    s.p_ships[3] = 3;
    s.p_ships[4] = 2;

    s.a_ships[0] = 5;
    s.a_ships[1] = 4;
    s.a_ships[2] = 3;
    s.a_ships[3] = 3;
    s.a_ships[4] = 2;


    //initialize rendering
    TileRenderer r = {0};
    TextRenderer t = {0};
    PresentInfo p = {0};
    CRASH_CALL(CreateVulkan());

    Attachment attachments[SR_TILE_ATTACHMENT_NUM + SR_TEXT_NUM_ATTACHMENTS];
    SubPass passes[2];
    TileGetSubpass(passes, attachments, 0);
    TextGetSubpass(passes, attachments, SR_TILE_ATTACHMENT_NUM);
    CRASH_CALL(InitPresent(&p, passes, 2, attachments, SR_TILE_ATTACHMENT_NUM + SR_TEXT_NUM_ATTACHMENTS));
    CRASH_CALL(TileInit(&r, &p.p, "resources/tilesets/battleship.jpg", 0, (sm_vec2i){4, 2}, (sm_vec2i){10, 20}));
    CRASH_CALL(TextInit(&t, "resources/fonts/JetBrainsMonoNLNerdFontPropo-Regular.ttf", 60,  &p.p, 1));
    SetColor(&t, (sm_vec3f){0.3, 0.3, 0.3});

    //clear board
    for (int i = 0; i < B_SIZE; i++) {
        s.p_board[i] = EMPTY;
        s.a_board[i] = EMPTY;
    }

    //ai places ships
    //init rand

    srand(time(NULL));
    {
        int shipsplaced = 0;
        while (shipsplaced < 5) {
            sm_vec2i pos;
            pos.x = rand() % B_WIDTH;
            pos.y = rand() % B_HEIGHT;

            sm_vec2i dir;
            dir = (rand() % 2) == 0 ? (sm_vec2i){1, 0} : (sm_vec2i){0, 1};

            int len = ship_sizes[shipsplaced];
            
            bool valid = TRUE;
            sm_vec2i curr = pos;
            for (int i = 0; i < len; i++) {
                if (s.a_board[curr.x + curr.y * B_WIDTH] != EMPTY) 
                    valid = FALSE;
                if (curr.x < 0 || curr.x > (B_WIDTH-1))
                    valid = FALSE;
                if (curr.y < 0 || curr.y > (B_HEIGHT-1))
                    valid = FALSE;
                if (!valid) break;
                curr = sm_vec2_i32_add(curr, dir);
            }
            if (!valid) continue;

            curr = pos;
            for (int i = 0; i < len; i++) {
                s.a_board[curr.x + curr.y * B_WIDTH] = CARRIER + shipsplaced; 
                curr = sm_vec2_i32_add(curr, dir);
            }
            shipsplaced++;
        }
    }

    setTile(&s);
    TileSetData(&r, screen);

    int frameCounter = 0;
    int shipsPlaced = 0;
    while (!glfwWindowShouldClose(sr_context.w)) {
        glfwPollEvents();
        bool selected;
        sm_vec2i pos = getInput(&selected);

        if (shipsPlaced < 5 && selected) {
            PlaceShip(&s, &t, &shipsPlaced, pos);
        } else if (selected && !s.gameover) {
            TakeTurn(&s, &t, pos);
        }
        setTile(&s);
        TileSetData(&r, screen);



        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        StartFrame(&p, frameCounter);
        TileDrawFrame(&r, &p, frameCounter);
        NextPass(&p, frameCounter);
        TextDrawFrame(&t, &p, frameCounter);
        SubmitFrame(&p, frameCounter); 
    }


    TextDestroy(&t);
    DestroyTile(&r);
    DestroyPresent(&p);
    DestroyVulkan();
    return 0;
}

sm_vec2i getInput(bool* selected) {
    static bool reset = FALSE;

    *selected = FALSE;
    int state = glfwGetMouseButton(sr_context.w, GLFW_MOUSE_BUTTON_1);
    if (state != GLFW_PRESS) {
        reset = TRUE;
        return (sm_vec2i){0,0};
    }
    if (!reset) {
        return (sm_vec2i){0,0};
    }
    *selected = TRUE;
    reset = FALSE;

    double xpos, ypos;
    int width, height;
    glfwGetCursorPos(sr_context.w, &xpos, &ypos);
    glfwGetWindowSize(sr_context.w, &width, &height);
    xpos = floor(xpos);
    ypos = floor(ypos);
    xpos /= width;
    ypos /= height;
    xpos *= 10;
    ypos *= 20;
    SR_LOG_DEB("(%d, %d)", (int)xpos, (int)ypos);


    return (sm_vec2i){xpos, ypos};
}

void setTile(GameState* g) {
    const int p_map[] = {
        2,      //EMPTY
        1,      //CARRIER
        1,      //BATTLE
        1,      //CRUISER
        1,      //SUBMARINE
        1,      //DESTROYER
        6,      //HIT
        4,      //MISS
    };
    const int a_map[] = {
        0,      //EMPTY
        0,      //CARRIER
        0,      //BATTLE
        0,      //CRUISER
        0,      //SUBMARINE
        0,      //DESTROYER
        5,      //HIT
        3,      //MISS
    };
    for (int i = 0; i < B_SIZE; i++) {
        screen[i] = a_map[g->a_board[i]];
    }
    for (int i = 0; i < B_SIZE; i++) {
        screen[i + B_SIZE] = p_map[g->p_board[i]];
    }
}

void PlaceShip(GameState* s, TextRenderer* t, int* shipsPlaced, sm_vec2i pos) {
    static bool final = FALSE;
    static sm_vec2i last = (sm_vec2i){0, 0};

    pos.y -= B_HEIGHT;

    if (pos.x < 0 || pos.x > (B_WIDTH-1)) {
        final = FALSE;
        return;
    }

    if (pos.y < 0 || pos.y > (B_HEIGHT-1)) {
        final = FALSE;
        return;
    }


    if (!final) {
        final = TRUE;
        last = pos; 
        return;
    }
    SR_LOG_DEB("(%d, %d) -> (%d, %d)", last.x, last.y, pos.x, pos.y); 
    sm_vec2i dir = sm_vec2_i32_sub(pos, last);
    if (dir.x == 0 && dir.y == 0 || dir.x != 0 && dir.y != 0) {
        dir = (sm_vec2i){1, 0};
    }
    int dirlen = sm_vec2_i32_len(dir);
    dir.x /= dirlen;
    dir.y /= dirlen;

    int len = ship_sizes[*shipsPlaced];

    bool valid = TRUE;
    sm_vec2i curr = last;
    for (int i = 0; i < len; i++) {
        if (s->p_board[curr.x + curr.y * B_WIDTH] != EMPTY) 
            valid = FALSE;
        if (curr.x < 0 || curr.x > (B_WIDTH-1))
            valid = FALSE;
        if (curr.y < 0 || curr.y > (B_HEIGHT-1))
            valid = FALSE;
        if (!valid) break;
        curr = sm_vec2_i32_add(curr, dir);
    }

    if (!valid) {
        final = FALSE;
        ClearText(t);
        AppendText(t, "Invalid Position", 16, (sm_vec2f){10, 10}, 1);
        return;
    }

    curr = last;
    for (int i = 0; i < len; i++) {
        s->p_board[curr.x + curr.y * B_WIDTH] = CARRIER + *shipsPlaced; 
        curr = sm_vec2_i32_add(curr, dir);
    }
    (*shipsPlaced)++;
    final = FALSE;
}

void TakeTurn(GameState* s, TextRenderer* t, sm_vec2i pos) {
    if (pos.x > B_WIDTH || pos.y > B_HEIGHT)
        return;

    TileState state = s->a_board[pos.x + pos.y * B_WIDTH];
    if (state != EMPTY && state != HIT && state != MISS) {
        state -= CARRIER;
        s->a_ships[state]--;
        if (s->a_ships[state] <= 0) {
            char buf[32];
            int len = snprintf(buf, 32, "You sunk their %s", ships[state]);
            ClearText(t);
            AppendText(t, buf, len, (sm_vec2f){10, 10}, 1);
        } else {
            ClearText(t);
            AppendText(t, "Hit", 3, (sm_vec2f){10, 10}, 1);
        }
        s->a_board[pos.x + pos.y * B_WIDTH] = HIT;
    } else if (state == HIT || state == MISS) {
            ClearText(t);
            AppendText(t, "Invalid Target", 14, (sm_vec2f){10, 10}, 1);
    } else {
        ClearText(t);
        AppendText(t, "Miss", 4, (sm_vec2f){10, 10}, 1);
        s->a_board[pos.x + pos.y * B_WIDTH] = MISS;
    }

    bool valid = FALSE;
    while (!valid) {
        //take ai turns until it misses
        sm_vec2i pos;
        pos.x = rand() % B_WIDTH;
        pos.y = rand() % B_HEIGHT;

        TileState state = s->p_board[pos.x + pos.y * B_WIDTH];
        if (state != EMPTY && state != HIT && state != MISS) {
            state -= CARRIER;
            s->p_ships[state]--;
            if (s->p_ships[state] <= 0) {
                char buf[32];
                int len = snprintf(buf, 32, "They sunk your %s", ships[state]);
                ClearText(t);
                AppendText(t, buf, len, (sm_vec2f){10, 10}, 1);
            }
            s->p_board[pos.x + pos.y * B_WIDTH] = HIT;
        } else if (state == HIT || state == MISS) {
            continue;
        } else {
            //ClearText(t);
            //AppendText(t, "Miss", 4, (sm_vec2f){10, 10}, 1);
            s->p_board[pos.x + pos.y * B_WIDTH] = MISS;
            valid = TRUE;
        }

    }

    //check for winner
    bool PlayerDead = TRUE;
    bool AIDead = TRUE;
    for (int i = 0; i < 5; i++) {
        if (s->p_ships[i] != 0) {
            PlayerDead = FALSE; 
            break;
        }
    }
    for (int i = 0; i < 5; i++) {
        if (s->a_ships[i] != 0) {
            AIDead = FALSE; 
            break;
        }

    }

    if (PlayerDead || AIDead) {
        char buf[32];
        int len = snprintf(buf, 32, "%s Wins!", PlayerDead ? "AI" : "Player");
        ClearText(t);
        SetColor(t, (sm_vec3f){0.0f, 1.0f, 0.0f});
        AppendText(t, buf, len, (sm_vec2f){10, 50}, 3);
        s->gameover = TRUE;
    }
}
