//#include <iostream> 
#include <raylib.h>
#include <math.h>
#include <raymath.h>

#define G 400
#define PLAYER_JUMP_SPEED 350.0f
#define PLAYER_HOR_SPEED 200.0f



int main() {

    const int screenW = 800;
    const int screenH = 450;

    //PLACE FOR ADDING PIXELSMOOTHER


    InitWindow(screenW, screenH, "just an example");

    Rectangle player = { 400,280,40,40 };

    Camera2D camera = { 0 };
    camera.target = (Vector2){ player.x + 20.0f, player.y + 20.0f};
    camera.offset = (Vector2){ GetRenderWidth()/2.0f, GetRenderHeight()/2.0f};
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;

    //Vector2 ballPosition = { (float)screenW/2, (float)screenH/2 };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        
        if (IsKeyDown(KEY_RIGHT)) player.x += 2;
        else if (IsKeyDown(KEY_LEFT)) player.x -= 2;
        //if (IsKeyDown(KEY_UP)) ballPosition.y -= 2.0f;
        //if (IsKeyDown(KEY_DOWN)) ballPosition.y += 2.0f;
        camera.target = (Vector2){ player.x + 20, player.y + 20};
        
        camera.zoom = expf(logf(camera.zoom) + ((float)GetMouseWheelMove()*0.1f));

        if (camera.zoom > 3.0f) camera.zoom = 3.0f;
        else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

        BeginDrawing();

            ClearBackground(RAYWHITE);

            //DrawCircleV(ballPosition, 50, RED);
    
            BeginMode2D(camera);
                
                DrawRectangle(-2000, 320, 3000, 8000, DARKGRAY);

                DrawRectangleRec(player, RED);

                //DrawLine((int)camera.target.x, -screenH*10, (int)camera.target.x, screenH*10, GREEN);
                //DrawLine(-screenW*10, (int)camera.target.y, screenW*10, (int)camera.target.y, GREEN);

            EndMode2D();

            DrawText("hello", 350, 100, 20, BLACK);
        
            DrawText("SIX SEVEN", 640, 10, 20, RED);

        EndDrawing();
        
    }

    CloseWindow();
    return 0;
}