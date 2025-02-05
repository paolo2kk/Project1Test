#pragma once

#include "raylib.h"
#include "Globals.h"
#include <unordered_map>
#include <string>

enum class Resource {
    IMG_TEXT,
    IMG_MENU,
    IMG_ANIMATIONS,
    IMG_LIFE_BOB,
    IMG_LIFE_BUB,
    IMG_TUTORIAL,
    IMG_INTRO,
    IMG_GAME_OVER,
    IMG_PATO,
    IMG_UPC_CITM,
    IMG_EXPL,
    IMG_GOOD_ENDING,
    IMG_FALSE_ENDING,
    IMG_PLAYER_2,
    IMG_SCORE,
    IMG_MENU2,
    IMG_MENU3,
    IMG_MENU4,
    IMG_INSCOIN,
    IMG_PLAYER_1,
    IMG_PLAYER,
    IMG_TILES,
    IMG_TILES2,
    IMG_ITEMS,
    IMG_CINEMATIC_LETTERS,
    IMG_BONUS,
    IMG_BUBBLES,
    IMG_STAGE1,
    IMG_STAGE2,
    MUSIC_INTRO,
    MUSIC_BACKGROUND,
    MUSIC_TITLE,
    MUSIC_FALSE_ENDING,
    MUSIC_GAME_OVER,
    MUSIC_MAIN_THEME_HURRY,
    MUSIC_REAL_ENDING,
    MUSIC_MAIN_THEME,
    MUSIC_SUPER_DRUNK,
    SFX_INSERT_COIN,
    SFX_JUMP,
    SFX_PICKUP,
    SFX_BUBBLE,
    SFX_BOSS_ATTAK,
    SFX_BUBBLE_POP,
    SFX_DEATH,
    SFX_ELECTRIC_HIT,
    IMG_ZENCHAN,
    IMG_ENEMIES,
    IMG_WINDOW,
};

class ResourceManager {
public:
    //Singleton instance retrieval
    static ResourceManager& Instance()
    {
        static ResourceManager instance; //Guaranteed to be initialized only once
        return instance;
    }

    //Load music
    AppStatus LoadMusic(Resource id, const std::string& file_path);
    void UnloadMusic(Resource id);
    const Music* GetMusic(Resource id) const;

    //Load Sound Effect
    AppStatus LoadSoundEffect(Resource id, const std::string& file_path);
    void PlaySoundEffect(Resource id);


    //Load and unload texture
    AppStatus LoadTexture(Resource id, const std::string& file_path);
    AppStatus LoadAudio(Resource id, const std::string& file_path);
    void ReleaseTexture(Resource id);

    //Get texture by key
    const Texture2D* GetTexture(Resource id) const;

    //Release resources
    void Release();

    //Ensure Singleton cannot be copied or assigned
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

private:
    //Private constructor to prevent instantiation
    ResourceManager();
    //Destructor
    ~ResourceManager();

    //Dictionary to store loaded textures
    std::unordered_map<Resource, Texture2D> textures;
    std::unordered_map<Resource, Sound> sounds;
    std::unordered_map<Resource, Music> musicResources;
    std::unordered_map<Resource, Sound> soundEffects;
};