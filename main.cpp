#include "platformer.h"
#include "raylib.h"
#include "raymath.h"
#include "assets.h"

int main() {
    //initialazing windows W and H and window itself
    const int sW = 800;
    const int sH = 600;

    InitWindow(sW, sH, "example");


    //all the parametres section
    const int pUS = 64; //platform universal size measure
    const int pDW = 1024;
    const int pDH = 256;

    //moving eyes init from the ending room !!DONT TOUCH!!
    Vector2 scleraLeftPosition = { GetScreenWidth()/2.0f - 300.0f, GetScreenHeight()/2.0f - 100.0f };
    Vector2 scleraRightPosition = { GetScreenWidth()/2.0f + 100.0f, GetScreenHeight()/2.0f - 100.0f };
    float scleraRadius = 80;

    Vector2 irisLeftPosition = { GetScreenWidth()/2.0f - 100.0f, GetScreenHeight()/2.0f - 100.0f };
    Vector2 irisRightPosition = { GetScreenWidth()/2.0f + 100.0f, GetScreenHeight()/2.0f - 100.0f };
    float irisRadius = 24;
    float angle = 0.0f;
    float dx = 0.0f, dy = 0.0f, dxx = 0.0f, dyy = 0.0f;
    //*probably need to be fixed+hardcoded for not to go to random coords +maybe some textures


    //PLAYER AND ENVITEMS (PLATFORMS) 
    Player player = { 0 };
    player.position = (Vector2){ pDW*2+pDW/2, pDH+128-32 };
    player.speed = 0;
    player.canJump = false;
    player.safepos = (Vector2){ player.position.x, 352};
    player.isDead = false;
    player.standingOn = -1;
    TimeState time = { 1.0f, 1.0f, 0.0 };


    //ASSET MANAGER (TEXTURES, SOUNDS ETC)
    AssetManager assets = { 0 };
    int wallId  = AssetLoad(&assets, "resources/brickswide.png");
    
    //TODO: Scale all platform widths and heights to 64*n to apply textures accurately
    EnvItem envItems[] = {
        //ending room
        {{ 0, pDH, pDW, pDH }, 0, DARKGRAY, false, false},
        {{ 0, pDH*2, pDW, pDH }, 1, GRAY, false, false  }, //bottom platf
        //need rework - {{ GetScreenWidth()/2.0f - 200.0f, GetScreenHeight()/2.0f - 100.0f, 200, 200}, 0, BLACK, false, false }, //door
        {{ 0, 0, pDW, pDH}, 1, GRAY, false, false },


        //main room
        {{pDW*2,pDH,pDW,pDH},0,DARKGRAY,false, false}, //bg
        {{pDW*2,pDH*2,pDW,pDH},1,GRAY,false, false}, //the floor 
        {{pDW*2,0,pDW,pDH},1,GRAY,false, false}, //roof


        //switch room
        {{pDW*4,pDH,pDW,pDH}, 0, BLUE, false, false},
        {{pDW*4,0,pDW,pDH}, 1, DARKGRAY, false, false},
        {{pDW*4,pDH*2,pDW,pDH}, 1, DARKGRAY, false, false},
        

        //"top" room - add more height
        {{pDW*6,pDH,pDW,pDH},0,DARKGRAY,false, false}, //bg
        {{pDW*6,pDH*2,pDW,pDH},1,GRAY,false, false}, //the floor 
        {{pDW*6,0,pDW,pDH},1,GRAY,false, false}, //roof


        //"water" room - add more height
        {{pDW*8,pDH,pDW,pDH},0,DARKGRAY,false, false}, //bg
        {{pDW*8,pDH*2,pDW,pDH},1,GRAY,false, false}, //the floor 
        {{pDW*8,0,pDW,pDH},1,GRAY,false, false}, //roof


        //"bullet" room
        {{pDW*10,pDH,pDW,pDH},0,DARKGRAY,false, false}, //bg
        {{pDW*10,pDH*2,pDW,pDH},1,GRAY,false, false}, //the floor 
        {{pDW*10,0,pDW,pDH},1,GRAY,false, false}, //roof


        //moving objects
        {{ pDW/2, pDH+128-32, pDW/10, pDH/10 }, 1, GRAY, true, { pDW/2, pDH+128-32 }, { pUS*2, 0 }, pUS*8, 1, false },



        //dangerous objects
        {{pDW*2+256, pDH+128, pUS, pUS}, 0, GOLD, false, {0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 0, true},
        
        
    
    };
    int envItemsLength = sizeof(envItems)/sizeof(envItems[0]);

    //LOADING TEXTURES
    envItems[4].texture  = AssetGet(&assets, wallId);

    //CAMERA - !!DONT TOUCH!!
    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.position.x + 20.0f, player.position.y + 20.0f };
    camera.offset = (Vector2){ GetRenderWidth()/2.0f, GetRenderHeight()/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 0.7f;


    //game target fps
    SetTargetFPS(60);


    //MAIN GAME LOOP
    while (!WindowShouldClose()) {
        //making player state update
        float deltaTime = GetFrameTime();
        UpdateTimeScale(&time, deltaTime);

        if (deltaTime > 0.05f) deltaTime = 0.05f;

        float scaledDelta = deltaTime * time.scale;

        UpdateMovingPlatforms(&player, envItems, envItemsLength, scaledDelta);
        UpdatePlayer(&player, envItems, envItemsLength, deltaTime);
        
        //respawning player after death
        if (player.isDead && IsKeyPressed(KEY_R)) {
            RespawnPlayer(&player);
        }

        //binding camera to player
        camera.target = (Vector2){ player.position.x + 20.0f, player.position.y + 20.0f };
        
    
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

        if (!player.isDead) {
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
        /*
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
        */
        }
        //START OF DRAWING OF GAME 
        BeginDrawing();
            //color of background:
            ClearBackground(LIGHTGRAY); 
            

            //starting camera
            BeginMode2D(camera);
                //drawing all the envItems platforms
                for (int i = 0; i < envItemsLength; i++){
                    EnvItem *e = &envItems[i];
                    if (e->texture) {
                        Rectangle src = { 0.0f, 0.0f,
                                        (float)e->texture->width,
                                        (float)e->texture->height };
                        DrawTexturePro(*e->texture, src, e->rect,
                                      (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
                    } else {
                        DrawRectangleRec(e->rect, e->color);
                    }
                DrawRectangleRec(envItems[i].rect, envItems[i].color);                                           
                }
                //TRACKING EYES !!DONT TOUCH!!
                DrawCircleV(scleraLeftPosition, scleraRadius, LIGHTGRAY);
                DrawCircleV(irisLeftPosition, irisRadius, BROWN);
                DrawCircleV(irisLeftPosition, 10, BLACK);

                DrawCircleV(scleraRightPosition, scleraRadius, LIGHTGRAY);
                DrawCircleV(irisRightPosition, irisRadius, DARKGREEN);
                DrawCircleV(irisRightPosition, 10, BLACK);
  

                //creating and drawing the player box (just to be visible, prob will need a texture)
                Rectangle playerRect = { player.position.x - kPlayerWidth/2.0f,
                                         player.position.y - kPlayerHeight,
                                         kPlayerWidth, kPlayerHeight };

                if (!player.isDead) {
                    DrawRectangleRec(playerRect, RED);
                    DrawCircleV(player.position, 5.0f, GOLD);
                }
                


            EndMode2D();

            if (time.scale < 0.99f) {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                Fade(BLUE, 0.10f * (1.0f - time.scale)));
            } else if (time.scale > 1.01f) {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                Fade(ORANGE, 0.06f * (time.scale - 1.0f)));
            }

            const char *timeStr = TextFormat("TIME x%.2f", time.scale);
            DrawText(timeStr, 10, 10, 20, (time.scale < 1.0f) ? BLUE :
                                          (time.scale > 1.0f) ? ORANGE : DARKGRAY);
            
            //PLAYER RESPAWN
            if (player.isDead)
            {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.35f));

                const char *deathText = "YOU DIED";
                const int deathFontSize = 80;
                int deathTextWidth = MeasureText(deathText, deathFontSize);
                DrawText(deathText,
                         (GetScreenWidth() - deathTextWidth) / 2,
                         GetScreenHeight() / 2 - 100,
                         deathFontSize, MAROON);

                const char *hintText = "R - respawn";
                const int hintFontSize = 24;
                int hintTextWidth = MeasureText(hintText, hintFontSize);
                DrawText(hintText,
                         (GetScreenWidth() - hintTextWidth) / 2,
                         GetScreenHeight() / 2 + 10,
                         hintFontSize, WHITE);
            }

            
        EndDrawing();  
    }


    //necessary - UNLOAD ALL THE TEXTURES
    AssetUnloadAll(&assets);
    //*probably can make an list of all textures as class like as envItems to just unload all


    //closing window !!DONT TOUCH!!
    CloseWindow();

    return 0;
}