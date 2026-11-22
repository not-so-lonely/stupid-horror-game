#include "raylib.h"
#include "raymath.h"
#include "math.h"   

#define G 400
#define PLAYER_JUMP_SPD 350.0f
#define PLAYER_HOR_SPD 200.0f


//classes
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


//functions
void UpdateMovingPlatforms(EnvItem *envItems, int count, float delta) {
    //TODO: MOVING PLAYER WITH THE PLATFORM
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
            //item->velocity.y *= -1; // only if you allow vertical movement
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
 
}



//main
int main() {
    //initialazing windows W and H and window itself
    const int sW = 960;
    const int sH = 720;

    InitWindow(sW, sH, "example");

    //section for ALL the textures AND images
    Image image = LoadImage("resources/brickswide.png");
    Texture2D wall = LoadTextureFromImage(image);
    UnloadImage(image);
    Image bgim = LoadImage("resources/bg.png");
    Texture2D bagr = LoadTextureFromImage(bgim);
    UnloadImage(bgim);


    //all the parametres section
    const int pUS = 64; //platform universal size measure
    const int pDW = 1024;
    const int pDH = 256;

    //moving eyes init from the ending room !!DONT TOUCH!!
    Vector2 scleraLeftPosition = { GetScreenWidth()/2.0f - 300.0f, GetScreenHeight()/2.0f - 100.0f };
    Vector2 scleraRightPosition = { GetScreenWidth()/2.0f + 100.0f, GetScreenHeight()/2.0f - 100.0f };
    float scleraRadius = 92;

    Vector2 irisLeftPosition = { GetScreenWidth()/2.0f - 100.0f, GetScreenHeight()/2.0f - 100.0f };
    Vector2 irisRightPosition = { GetScreenWidth()/2.0f + 100.0f, GetScreenHeight()/2.0f - 100.0f };
    float irisRadius = 48;
    float angle = 0.0f;
    float dx = 0.0f, dy = 0.0f, dxx = 0.0f, dyy = 0.0f;
    //*probably need to be fixed+hardcoded for not to go to random coords +maybe some textures


    //PLAYER AND ENVITEMS (PLATFORMS) 
    Player player = { 0 };
    player.position = (Vector2){ pDW*2+pDW/2, pDH+128-32 };
    player.speed = 0;
    player.canJump = false;
    player.safepos = (Vector2){ player.position.x, 352};
    //TODO: Scale all platform widths and heights to 64*n to apply textures accurately
    EnvItem envItems[] = {
        //ending room
        {{ 0, 0, pDW, pDH*2 }, 0, DARKGRAY, false, false}, //bg
        {{ 0, pDH*2, pDW, pDH }, 1, GRAY, false, false  }, //bottom platf
        {{ 0, -pDH, pDW, pDH}, 1, GRAY, false, false }, //roof
        {{ -pDH, -pDH, pDH, pDW}, 1, GRAY, false, false }, //left wall
        {{ pDW, -pDH, pDH, pDW}, 1, GRAY, false, false }, //right wall


        //main room
        {{pDW*2,0,pDW,pDH*2},0,DARKGRAY,false, false}, //bg
        {{pDW*2,pDH*2,pDW,pDH},1,GRAY,false, false}, //the floor 
        {{pDW*2,-pDH,pDW,pDH},1,GRAY,false, false}, //roof
        {{ pDW*2-pDH, -pDH, pDH, pDW}, 1, GRAY, false, false }, //left wall
        {{ pDW*3, -pDH, pDH, pDW}, 1, GRAY, false, false }, //right wall

        //switch room
        {{pDW*4,0,pDW,pDH*2}, 0, BLUE, false, false}, //bg
        {{pDW*4,-pDH,pDW,pDH}, 1, DARKGRAY, false, false}, //roof
        {{pDW*4,pDH*2,pDW,pDH}, 1, DARKGRAY, false, false}, //floor
        {{ pDW*4-pDH, -pDH, pDH, pDW}, 1, GRAY, false, false }, //left wall
        {{ pDW*5, -pDH, pDH, pDW}, 1, GRAY, false, false }, //right wall


        //"top" room - add more height
        {{pDW*6,0,pDW,pDH*2},0,DARKGRAY,false, false}, //bg
        {{pDW*6,pDH*2,pDW,pDH},1,GRAY,false, false}, //the floor 
        {{pDW*6,-pDH,pDW,pDH},1,GRAY,false, false}, //roof
        {{ pDW*6-pDH, -pDH, pDH, pDW}, 1, GRAY, false, false }, //left wall
        {{ pDW*7, -pDH, pDH, pDW}, 1, GRAY, false, false }, //right wall


        //"water" room - add more height
        {{pDW*8,0,pDW,pDH*2},0,DARKGRAY,false, false}, //bg
        {{pDW*8,pDH*2,pDW,pDH},1,GRAY,false, false}, //the floor 
        {{pDW*8,-pDH,pDW,pDH},1,GRAY,false, false}, //roof
        {{ pDW*8-pDH, -pDH, pDH, pDW}, 1, GRAY, false, false }, //left wall
        {{ pDW*9, -pDH, pDH, pDW}, 1, GRAY, false, false }, //right wall


        //"bullet" room
        {{pDW*10,0,pDW,pDH*2},0,DARKGRAY,false, false}, //bg
        {{pDW*10,pDH*2,pDW,pDH},1,GRAY,false, false}, //the floor 
        {{pDW*10,-pDH,pDW,pDH},1,GRAY,false, false}, //roof
        {{ pDW*10-pDH, -pDH, pDH, pDW}, 1, GRAY, false, false }, //left wall
        {{ pDW*12, -pDH, pDH, pDW}, 1, GRAY, false, false }, //right wall
        {{pDW*11,0,pDW,pDH*2},0,DARKGRAY,false, false}, //bg
        {{pDW*11,pDH*2,pDW,pDH},1,GRAY,false, false}, //the floor 
        {{pDW*11,-pDH,pDW,pDH},1,GRAY,false, false}, //roof


        //other
        //need rework - {{ GetScreenWidth()/2.0f - 200.0f, GetScreenHeight()/2.0f - 100.0f, 200, 200}, 0, BLACK, false, false }, //door

        //moving objects
        {{ pDW/2, pDH+128-32, pDW/10, pDH/10 }, 1, GRAY, true, { pDW/2, pDH+128-32 }, { pUS*2, 0 }, pUS*8, 1, false },



        //dangerous objects
        {{pDW*2+256, pDH+128, pUS, pUS}, 0, GOLD, false, true},
        
        
    
    };
    int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);


    //CAMERA - !!DONT TOUCH!!
    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.position.x + 20.0f, player.position.y + 20.0f };
    camera.offset = (Vector2){ GetRenderWidth()/2.0f, GetRenderHeight()/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;


    //game target fps
    SetTargetFPS(60);


    //MAIN GAME LOOP
    while (!WindowShouldClose()) {
        //making player state update
        float deltaTime = GetFrameTime();
        UpdateMovingPlatforms(envItems,envItemsLength, deltaTime);
        UpdatePlayer(&player, envItems, envItemsLength, deltaTime);


        //binding camera to player
        camera.target = (Vector2){ player.position.x + 20.0f, player.position.y - pUS*3+32 };
        
    
        //TRACKING EYES !!DONT TOUCH AT ALL!!
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


        //Room-transition (maybe rework)
        //from tracking eyes room to main room and vice versa
        if (player.position.x > pDW-pUS && player.position.x < pDW ) {
            player.position.x = pDW*2+pUS*2;
            player.safepos.x = pDW*2+pUS*2;
        }
        if (player.position.x > pDW*2 && player.position.x < pDW*2+pUS) {
            player.position.x = pDW-pUS*2;
            player.safepos.x = pDW-pUS*2;
        }
        //from main room to choosing room and vice versa
        if (player.position.x > 4950 && player.position.x < 5000) {
            player.position.x = 7100;
            player.safepos.x = 7100;
        }
        if (player.position.x > 7000 && player.position.x < 7100) {
            player.position.x = 4900;
            player.safepos.x = 4900;
        }
        //like 3 more transitions
        

        //START OF DRAWING OF GAME 
        BeginDrawing();
            //color of background:
            ClearBackground(LIGHTGRAY); 
            

            //starting camera
            BeginMode2D(camera);
                //drawing all the envItems platforms
                for (int i = 0; i < envItemsLength; i++)
                DrawRectangleRec(envItems[i].rect, envItems[i].color);                                           
                
                //TRACKING EYES !!DONT TOUCH!!
                DrawCircleV(scleraLeftPosition, scleraRadius, LIGHTGRAY);
                DrawCircleV(irisLeftPosition, irisRadius, BROWN);
                DrawCircleV(irisLeftPosition, 32, BLACK);

                DrawCircleV(scleraRightPosition, scleraRadius, LIGHTGRAY);
                DrawCircleV(irisRightPosition, irisRadius, DARKGREEN);
                DrawCircleV(irisRightPosition, 32, BLACK);


                //Drawing all the textures
                for ( int k = 1; k < 15; k++) {
                for (int i =0; i<4; i++) { 
                    for (int j=0; j<4; j++){
                            if (envItems[k].rect.height < 300) {
                                DrawTexturePro(wall, { 0.0f, 0.0f, (float)wall.width, (float)wall.height}, {envItems[k].rect.x+i*pUS*4, envItems[k].rect.y+j*pUS, (float)wall.width, (float)wall.height}, { 0.0f, 0.0f }, 0.0f, WHITE);
                            }
                            if (envItems[k].rect.height < 600 && envItems[k].rect.height > 300) {
                                if (j>1) {continue;}
                                DrawTexturePro(bagr, { 0.0f, 0.0f, (float)bagr.width, (float)bagr.height}, {envItems[k].rect.x+i*pUS*4, envItems[k].rect.y+j*pUS*4, (float)bagr.width, (float)bagr.height}, { 0.0f, 0.0f }, 0.0f, WHITE);
                            }
                            if (envItems[k].rect.height > 600) {
                                for (int h=0; h < 16; h++) {
                                    DrawTexturePro(wall, { 0.0f, 0.0f, (float)wall.width, (float)wall.height}, {envItems[k].rect.x, envItems[k].rect.y+h*pUS, (float)wall.width, (float)wall.height}, { 0.0f, 0.0f }, 0.0f, WHITE);
                                }
                            }
                            //DrawTexturePro(wall, { 0.0f, 0.0f, (float)wall.width, (float)wall.height}, {envItems[2].rect.x+i*pUS*4, envItems[2].rect.y+j*pUS, (float)wall.width, (float)wall.height}, { 0.0f, 0.0f }, 0.0f, WHITE);
                    }
                }
                }


                //creating and drawing the player box (just to be visible, prob will need a texture)
                Rectangle playerRect = { player.position.x - 20, player.position.y - 40, 40.0f, 40.0f };
                DrawRectangleRec(playerRect, RED);
                DrawCircleV(player.position, 5.0f, GOLD);
                

                //DEATH - tp to safepos when collision with [exact object] (prob need a rework to connect to envItems struct, not hardcoded)
                if (CheckCollisionRecs(playerRect, envItems[31].rect))
                {
                    player.position = player.safepos;
                }


            EndMode2D();

            
        EndDrawing();  
    }


    //necessary - UNLOAD ALL THE TEXTURES
    UnloadTexture(wall);
    //*probably can make an list of all textures as class like as envItems to just unload all


    //closing window !!DONT TOUCH!!
    CloseWindow();

    return 0;
}

