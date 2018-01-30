#include "../statichook.h"
#include "MinigameManager.h"
#include "skywars/SkyWarsMinigame.h"
#include <minecraft/entity/Player.h>

class BlockSource;
class BlockPos;

TClasslessInstanceHook(void, _ZNK11PortalBlock12entityInsideER11BlockSourceRK8BlockPosR6Entity,
                       BlockSource& blockSource, BlockPos const& pos, Entity& entity) {
    if ((int) entity.getEntityTypeId() == 0x13F) {
        for (auto const& p : MinigameManager::instance.getGames()) {
            if (p.second->isJoinable()) {
                p.second->addPlayer((Player*) &entity);
                return;
            }
        }
        auto game = MinigameManager::instance.createGame<SkyWarsMinigame>(MinigameManager::instance.getMinigameNameForPrefix("SW"), entity.getLevel(), "swmap");
        if (game != nullptr)
            game->addPlayer((Player*) &entity);
    }
}