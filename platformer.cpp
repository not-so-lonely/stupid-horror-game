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

static Rectangle PlayerAABB(const Player *p) {
    return (Rectangle){
        p->position.x - kPlayerWidth/2.0f,
        p->position.y - kPlayerHeight,
        kPlayerWidth, kPlayerHeight
    };
}

static bool CheckDangers(Player *p, EnvItem *items, int n) {
    Rectangle pr = PlayerAABB(p);
    for (int i = 0; i < n; i++) {
        EnvItem *e = &items[i];
        if (!e->isDanger) continue;
        if (CheckCollisionRecs(pr, e->rect)) {
            p->isDead = true;
            p->speed = 0.0f;
            p->standingOn = -1;
            return true;
        }
    }
    return false;
}

static bool ResolveHorizontal(Player *p, EnvItem *items, int n, float dx) {
    if (dx == 0.0f) return false;

    p->position.x += dx;
    Rectangle pr = PlayerAABB(p);
    bool hit = false;
    for (int i = 0; i < n; i++) {
        EnvItem *e = &items[i];
        if (!e->blocking) continue;
        if (!CheckCollisionRecs(pr, e->rect)) continue;

        hit = true;
        if (dx > 0.0f) p->position.x = e->rect.x - kPlayerWidth / 2.0f;
        else           p->position.x = e->rect.x + e->rect.width + kPlayerWidth / 2.0f;
        pr = PlayerAABB(p);
    }
    return hit;
}

static int ResolveVertical(Player *p, EnvItem *items, int n, float dy) {
    if (dy == 0.0f) return 0;
    float oldY = p->position.y;
    p->position.y += dy;
    int result = 0;

    for (int i = 0; i < n; i++) {
        EnvItem *e = &items[i];
        if (!e->blocking) continue;

        float pl = p->position.x - kPlayerWidth/2.0f;
        float pr = p->position.x + kPlayerWidth/2.0f;
        if (pr <= e->rect.x || pl >= e->rect.x + e->rect.width) continue;

        if (dy > 0.0f) {
            if (e->rect.y >= oldY && e->rect.y <= p->position.y) {
                p->position.y = e->rect.y;
                result = 1;
            }
        } else {
            float oldHead = oldY - kPlayerHeight;
            float newHead = p->position.y - kPlayerHeight;
            if (e->rect.y + e->rect.height >= newHead &&
                e->rect.y + e->rect.height <= oldHead) {
                p->position.y = e->rect.y + e->rect.height + kPlayerHeight;
            result = 2;
            }
        }
    }
    return result;
}

void UpdatePlayer(Player *player, EnvItem *envItems, int envItemsLength, float delta) {
    if (player->isDead) return;

    if (player->isDead) return;
    player->standingOn = -1;

    float dx = 0.0f;
    if (IsKeyDown(KEY_LEFT))  dx -= kPlayerHorizontalSpd * delta;
    if (IsKeyDown(KEY_RIGHT)) dx += kPlayerHorizontalSpd * delta;
    ResolveHorizontal(player, envItems, envItemsLength, dx);

    if (IsKeyDown(KEY_SPACE) && player->canJump) {
        player->speed = -kPlayerJumpSpd;
        player->canJump = false;
    }

    int v = ResolveVertical(player, envItems, envItemsLength,
                            player->speed * delta);
    if (v == 1) {
        player->speed = 0.0f;
        player->canJump = true;
        Rectangle pr = PlayerAABB(player);
        for (int i = 0; i < envItemsLength; i++) {
            EnvItem *e = &envItems[i];
            if (!e->blocking) continue;
            if (fabsf(pr.y + pr.height - e->rect.y) < 0.5f &&
                pr.x + pr.width > e->rect.x &&
                pr.x < e->rect.x + e->rect.width) {
                player->standingOn = i;
                break;
            }
        }
    } else if (v == 2) {
        player->speed = 0.0f;
    } else {
        player->speed += kG * delta;
        player->canJump = false;
    }

    player->standingOn = -1;
    Rectangle pr = PlayerAABB(player);
    for (int i = 0; i < envItemsLength; i++) {
        EnvItem *e = &envItems[i];
        if (!e->blocking) continue;
        if (fabsf(pr.y + pr.height - e->rect.y) < 1.5f &&
            pr.x + pr.width > e->rect.x &&
            pr.x < e->rect.x + e->rect.width) {
            player->standingOn = i;
            break;
        }
    }

    CheckDangers(player, envItems, envItemsLength);
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