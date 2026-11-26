#ifndef LABGIT_ASSETS_H_
#define LABGIT_ASSETS_H_
#include "raylib.h"

constexpr int kMaxAssets = 64;
constexpr int kMaxAssetPath = 256;

typedef struct AssetManager {
    Texture2D textures[kMaxAssets];
    char paths[kMaxAssets][kMaxAssetPath];
    int count;
} AssetManager;

int AssetLoad(AssetManager *am, const char *path);
Texture2D* AssetGet(AssetManager *am, int index);
void AssetUnloadAll(AssetManager *am);

#endif //LABGIT_ASSETS_H_