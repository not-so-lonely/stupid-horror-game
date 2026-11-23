#ifndef LABGIT_PLATFORMER_H_
#define LABGIT_PLATFORMER_H_
#include "raylib.h"

constexpr int kScreenWidth = 800;
constexpr int kScreenHeight = 600;
constexpr int kG = 1350;
constexpr float kPlayerJumpSpd = 500.0f;
constexpr float kPlayerHorizontalSpd = 400.0f;
constexpr float kPlayerWidth  = 40.0f;
constexpr float kPlayerHeight = 40.0f;
constexpr float kTimeScaleSlow   = 0.25f;
constexpr float kTimeScaleNormal = 1.00f;
constexpr float kTimeScaleFast   = 3.00f;
constexpr float kTimeScaleLerp   = 6.0f;

typedef struct TimeState {
    float  scale;
    float  targetScale;
    double gameTime;
} TimeState;

typedef struct Player {
    Vector2 position;
    float speed;
    bool canJump;
    Vector2 safepos;
    bool isDead;
    int standingOn;
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

void UpdateTimeScale(TimeState *t, float realDelta);
void UpdateMovingPlatforms(Player *player, EnvItem *envItems, int count, float delta);
void RespawnPlayer(Player *player);
void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta);
void UpdateMovingPlatforms(EnvItem *envItems, int count, float delta);
void UpdateCameraCenter(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterInsideMap(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraCenterSmoothFollow(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraEvenOutOnLanding(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);
void UpdateCameraPlayerBoundsPush(Camera2D *camera, Player *player, EnvItem *envItems, int envItemsLength, float delta, int width, int height);

#endif //LABGIT_PLATFORMER_H_