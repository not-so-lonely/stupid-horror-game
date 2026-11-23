#include "platformer.h"
#include "raylib.h"
#include "raymath.h"

void RespawnPlayer(Player *player) {
    player->position = player->safepos;
    player->speed = 0.0f;
    player->canJump = false;
    player->isDead = false;
    player->standingOn = -1;
}

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta) {
    if (player->isDead) return;

    player->standingOn = -1;

    if (IsKeyDown(KEY_LEFT)) player->position.x -= kPlayerHorizontalSpd*delta;
    if (IsKeyDown(KEY_RIGHT)) player->position.x += kPlayerHorizontalSpd*delta;
    if (IsKeyDown(KEY_SPACE) && player->canJump) {
        player->speed = -kPlayerJumpSpd;
        player->canJump = false;
    }

    bool hitObstacle = false;
    for (int i = 0; i < envItemsLength; i++) {
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
            player->standingOn = i;
            break;
        }
    }
    if (!hitObstacle) {
        player->position.y += player->speed*delta;
        player->speed += kG*delta;
        player->canJump = false;
    } else player->canJump = true;

    Rectangle playerRect = {
        player->position.x - kPlayerWidth/2.0f,
        player->position.y - kPlayerHeight,
        kPlayerWidth, kPlayerHeight
    };
    for (int i = 0; i < envItemsLength; i++) {
        if (envItems[i].isDanger && CheckCollisionRecs(playerRect, envItems[i].rect)) {
            player->isDead = true;
            player->speed = 0.0f;
            player->standingOn = -1;
            return;
        }
    }
}

void UpdateTimeScale(TimeState *t, float realDelta) {
    if (IsKeyDown(KEY_Z))      t->targetScale = kTimeScaleSlow;
    else if (IsKeyDown(KEY_X)) t->targetScale = kTimeScaleFast;
    else                       t->targetScale = kTimeScaleNormal;

    float k = 1.0f - expf(-kTimeScaleLerp * realDelta);
    t->scale += (t->targetScale - t->scale) * k;

    if (t->targetScale == kTimeScaleNormal && fabsf(t->scale - 1.0f) < 0.005f)
        t->scale = 1.0f;

    t->gameTime += (double)t->scale * (double)realDelta;
}

void UpdateMovingPlatforms(Player *player, EnvItem *envItems, int count, float delta) {
    for (int i = 0; i < count; i++) {
        EnvItem *item = &envItems[i];
        if (!item->isMoving) continue;

        const float prevX = item->rect.x;
        const float prevY = item->rect.y;

        item->rect.x += item->velocity.x * delta;
        item->rect.y += item->velocity.y * delta;

        float dist = Vector2Distance((Vector2){ item->rect.x, item->rect.y }, item->startPos);
        if (dist >= item->moveRange) {
            if (item->moveDirection > 0) item->rect.x = item->startPos.x + item->moveRange;
            else                         item->rect.x = item->startPos.x - item->moveRange;
            item->moveDirection *= -1;
            item->velocity.x *= -1;
        }

        if (player != nullptr && !player->isDead && player->standingOn == i) {
            player->position.x += item->rect.x - prevX;
            player->position.y += item->rect.y - prevY;
        }
    }
}