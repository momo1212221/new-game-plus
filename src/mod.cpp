#include <cstdint>
#include <string>
#include "openrac/mod.hpp"
#include "logic.hpp"

namespace {
struct SaveNewGamePlus {
    static constexpr const char* id="rac1.save.new_game_plus";
    static constexpr const char* name="Save_NewGamePlus";
    static constexpr const char* signature="void(i32)";
    using fn=void(*)(std::int32_t);
};
openrac::Mod* g=nullptr;
bool pending=false, skip_scene=false;
int skip_window=0;
int bonus=0;
}
void load(openrac::Mod& mod){
    if(mod.game()!="rac1" || !mod.has_world()) throw openrac::Error("NG+ Improvements requires RAC1 interface 1.9+");
    g=&mod;
    bonus=qol::clamp_bonus(mod.setting<std::int64_t>("bonus_bolts"));
    skip_scene=mod.setting<bool>("skip_first_scene");
    mod.hook<SaveNewGamePlus>([](auto original,std::int32_t slot){ original(slot); pending=true; });
    mod.on_frame([]{
        const auto s=g->play_status();
        if(pending && s.phase==OPENRAC_PLAY_PLAYING){
            if(bonus>0) g->bolts(bonus);
            if(g->has_notice()) g->play_notice(bonus>0 ? "New Game+ ready - bonus bolts added" : "New Game+ ready");
            pending=false;
            if(skip_scene) skip_window=600;
        }
        if(skip_window>0){
            --skip_window;
            if(g->has_scenes()){
                int frame=0;
                if(g->scene_playing(&frame)>=0){
                    if(g->play_skip()) skip_window=0;
                }
            }
        }
    });
    mod.info("NG+ start improvements");
}
OPENRAC_MOD_CPP("new-game-plus","0.1.0",load)
