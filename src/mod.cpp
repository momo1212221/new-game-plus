#include <cstdint>
#include "openrac/mod.hpp"
#include "openrac/games/rac1.hpp"
#include "logic.hpp"

namespace {
openrac::Mod* g=nullptr;
bool pending=false;
bool hooked=false;
bool challenge_known=false;
bool challenge_last=false;
openrac::gaddr challenge_address=0;
int bonus=0;
}

void load(openrac::Mod& mod){
    if(mod.game()!="rac1" || !mod.has_world())
        throw openrac::Error("NG+ Improvements requires RAC1 interface 1.9+");
    g=&mod;
    bonus=qol::clamp_bonus(mod.setting<std::int64_t>("bonus_bolts"));

    if(mod.api_minor()>=11){
        try {
            mod.hook<openrac::rac1::Save_NewGamePlus>(
                [](auto original,std::int32_t slot){ original(slot); pending=true; }
            );
            hooked=true;
        } catch(const openrac::Error&) {
            hooked=false;
        }
    }
    if(!hooked){
        challenge_address=mod.relocate(0x0015EFA0u);
        mod.info("API 1.10 fallback: detecting Challenge Mode from game state");
    }

    mod.on_frame([]{
        if(!hooked){
            const bool challenge=g->memory().read<std::int32_t>(challenge_address)!=0;
            if(!challenge_known){
                challenge_last=challenge;
                challenge_known=true;
            }else{
                if(challenge && !challenge_last) pending=true;
                challenge_last=challenge;
            }
        }

        const auto s=g->play_status();
        const bool in_level =
            s.level>=0 && (s.phase==OPENRAC_PLAY_PLAYING || s.phase==OPENRAC_PLAY_SCENE
                           || s.phase==OPENRAC_PLAY_MENU);
        if(pending && in_level){
            if(bonus>0) g->bolts(bonus);
            pending=false;
        }
    });
}
OPENRAC_MOD_CPP("new-game-plus","0.2.0",load)
