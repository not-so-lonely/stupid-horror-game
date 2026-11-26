#include "assets.h"
#include <string.h>

int AssetLoad(AssetManager *am, const char *path) {
    if (am->count >= kMaxAssets) return -1;

    for (int i = 0; i < am->count; i++) {
        if (strcmp(am->paths[i], path) == 0) return i;
    }

    int idx = am->count;
    Texture2D t = LoadTexture(path);
    if (t.id == 0) return -1;

    am->textures[idx] = t;
    strncpy(am->paths[idx], path, kMaxAssetPath - 1);
    am->paths[idx][kMaxAssetPath - 1] = '\0';
    am->count++;
    return idx;
}

Texture2D *AssetGet(AssetManager *am, int index) {
    if (index < 0 || index >= am->count) return NULL;
    return &am->textures[index];
}

void AssetUnloadAll(AssetManager *am) {
    for (int i = 0; i < am->count; i++) {
        UnloadTexture(am->textures[i]);
    }
    am->count = 0;
}