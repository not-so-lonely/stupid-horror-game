#include "raylib.h"
#include "raymath.h"
#include <math.h>     

#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f

typedef struct Player {
    Vector2 position;
    float speed;
    bool canJump;
    Vector2 safepos;
} Player;

typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    Color color;

    bool isMoving;
    Vector2 startPos;
    Vector2 velocity;
    float moveRange;
    int moveDirection;
    bool isDanger;
} EnvItem;

void UpdateMovingPlatforms(EnvItem *envItems, int count, float delta) {
    for (int i = 0; i < count; i++) {
        EnvItem *item = &envItems[i];
        if (!item->isMoving) continue;

        item->rect.x += item->velocity.x * delta;
        item->rect.y += item->velocity.y * delta;

        float dist = Vector2Distance(
            (Vector2){ item->rect.x, item->rect.y }, item->startPos);

        if (dist >= item->moveRange) {
            // Clamp to the limit in the direction we were moving
            if (item->moveDirection > 0) {
                item->rect.x = item->startPos.x + item->moveRange;
            } else {
                item->rect.x = item->startPos.x - item->moveRange;
            }
            // (do the same for y if you allow vertical platforms)

            item->moveDirection *= -1;
            item->velocity.x *= -1;
            item->velocity.y *= -1; // only if you allow vertical movement
        }
    }
}

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta) {
    if (IsKeyDown(KEY_LEFT)) player->position.x -= PLAYER_HOR_SPD*delta;
    if (IsKeyDown(KEY_RIGHT)) player->position.x += PLAYER_HOR_SPD*delta;
    if (IsKeyDown(KEY_SPACE) && player->canJump)
    {
        player->speed = -PLAYER_JUMP_SPD;
        player->canJump = false;
    }

    bool hitObstacle = false;
   
    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;
        Vector2 *p = &(player->position);
        if (ei->blocking &&
            ei->rect.x <= p->x &&
            ei->rect.x + ei->rect.width >= p->x &&
            ei->rect.y >= p->y &&
            ei->rect.y <= p->y + player->speed*delta)
        {
            hitObstacle = true;
            player->speed = 0.0f;
            p->y = ei->rect.y;
            break;
        }
        
    }

    if (!hitObstacle)
    {
        player->position.y += player->speed*delta;
        player->speed += G*delta;
        player->canJump = false;
    }
    else player->canJump = true;
    /*
    bool hitDanger = false;
    for (int i = 0; i < envItemsLength; i++)
    {
        EnvItem *ei = envItems + i;
        Vector2 *p = &(player->position);
        if (ei->isDanger && ei->rect.x <= p->x && ei->rect.x + ei->rect.width >= p->x && ei->rect.y >= p->y)
        {
            hitDanger = true;
            p->x -= 200;
            p->y -= 50;
            break;
        }
    }
        */
}

//######################################################################

int main() {
    const int sW = 800;
    const int sH = 600;

    InitWindow(sW, sH, "example");

    Vector2 scleraLeftPosition = { GetScreenWidth()/2.0f - 300.0f, GetScreenHeight()/2.0f - 100.0f };
    Vector2 scleraRightPosition = { GetScreenWidth()/2.0f + 100.0f, GetScreenHeight()/2.0f - 100.0f };
    float scleraRadius = 80;

    Vector2 irisLeftPosition = { GetScreenWidth()/2.0f - 100.0f, GetScreenHeight()/2.0f - 100.0f };
    Vector2 irisRightPosition = { GetScreenWidth()/2.0f + 100.0f, GetScreenHeight()/2.0f - 100.0f };
    float irisRadius = 24;
    Vector2 jgg = { 650, 300};
    float angle = 0.0f;
    float dx = 0.0f, dy = 0.0f, dxx = 0.0f, dyy = 0.0f;


    Player player = { 0 };
    player.position = (Vector2){ 3500, 350 };
    player.speed = 0;
    player.canJump = false;
    player.safepos = (Vector2){ player.position.x, 350};
    EnvItem envItems[] = {
        //watch room
        {{ 0, 0, 1000, 400 }, 0, DARKGRAY, false, false},
        {{ 0, 400, 2000, 200 }, 1, GRAY, false, false  }, //bottom platf
        {{ jgg.x, jgg.y, 100, 10 }, 1, GRAY, true, jgg, { 200, 0 }, 500, 1, false },
        {{ GetScreenWidth()/2.0f - 200.0f, GetScreenHeight()/2.0f - 100.0f, 200, 200}, 0, BLACK, false, false }, //door
        {{ 0, -500, 1000, 500}, 1, GRAY, false, false },
        //main room
        {{3000,0,2000,400},0,DARKGRAY,false, false},
        {{3000,400,2000,400},1,GRAY,false, false},
        {{3000,-300,2000,300},1,GRAY,false, false},
        //danger
        {{1500, 300, 100, 100}, 0, GOLD, false, true},
        //switch room
        {{7000,0,1000,700}, 0, BLUE, false, false},
       {{7000,-300,1000,400}, 1, DARKGRAY, false, false},
       {{7000,400,1000,400}, 1, DARKGRAY, false, false},
        
    
    };
    //envItems[2]
    int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);

    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.position.x + 20.0f, player.position.y + 20.0f };
    camera.offset = (Vector2){ GetRenderWidth()/2.0f, GetRenderHeight()/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        UpdateMovingPlatforms(envItems,envItemsLength, deltaTime);
        UpdatePlayer(&player, envItems, envItemsLength, deltaTime);

        camera.target = (Vector2){ player.position.x + 20.0f, player.position.y + 20.0f };

        irisLeftPosition = player.position;
        irisRightPosition = player.position;

        // Check not inside the left eye sclera
        if (!CheckCollisionPointCircle(irisLeftPosition, scleraLeftPosition, scleraRadius - irisRadius))
        {
            dx = irisLeftPosition.x - scleraLeftPosition.x;
            dy = irisLeftPosition.y - scleraLeftPosition.y;

            angle = atan2f(dy, dx);

            dxx = (scleraRadius - irisRadius)*cosf(angle);
            dyy = (scleraRadius - irisRadius)*sinf(angle);

            irisLeftPosition.x = scleraLeftPosition.x + dxx;
            irisLeftPosition.y = scleraLeftPosition.y + dyy;
        }

        // Check not inside the right eye sclera
        if (!CheckCollisionPointCircle(irisRightPosition, scleraRightPosition, scleraRadius - irisRadius))
        {
            dx = irisRightPosition.x - scleraRightPosition.x;
            dy = irisRightPosition.y - scleraRightPosition.y;

            angle = atan2f(dy, dx);

            dxx = (scleraRadius - irisRadius)*cosf(angle);
            dyy = (scleraRadius - irisRadius)*sinf(angle);

            irisRightPosition.x = scleraRightPosition.x + dxx;
            irisRightPosition.y = scleraRightPosition.y + dyy;
        }

        if (player.position.x > 1950 && player.position.x < 2000 ) {
            player.position.x = 3100;
            player.safepos.x = 3100;
        }
        if (player.position.x > 3000 && player.position.x < 3050) {
            player.position.x = 1900;
            player.safepos.x = 1900;
        }
        if (player.position.x > 4950 && player.position.x < 5000) {
            player.position.x = 7100;
            player.safepos.x = 7100;
        }
        if (player.position.x > 7000 && player.position.x < 7100) {
            player.position.x = 4900;
            player.safepos.x = 4900;
        }

        BeginDrawing();

            ClearBackground(LIGHTGRAY);

            
            BeginMode2D(camera);

                for (int i = 0; i < envItemsLength; i++)
                DrawRectangleRec(envItems[i].rect, envItems[i].color);
                //DrawRectangle(pos2,300,100,10,GRAY);                                               

                DrawCircleV(scleraLeftPosition, scleraRadius, LIGHTGRAY);
                DrawCircleV(irisLeftPosition, irisRadius, BROWN);
                DrawCircleV(irisLeftPosition, 10, BLACK);

                DrawCircleV(scleraRightPosition, scleraRadius, LIGHTGRAY);
                DrawCircleV(irisRightPosition, irisRadius, DARKGREEN);
                DrawCircleV(irisRightPosition, 10, BLACK);


                Rectangle playerRect = { player.position.x - 20, player.position.y - 40, 40.0f, 40.0f };

                DrawRectangleRec(playerRect, RED);

                DrawCircleV(player.position, 5.0f, GOLD);

                if (CheckCollisionRecs(playerRect, envItems[8].rect))
                {
                    DrawText("SEX",1300, 200,36,BLACK);
                    player.position = player.safepos;
                }
            EndMode2D();


            


        EndDrawing();
    }



    CloseWindow();

    return 0;
}