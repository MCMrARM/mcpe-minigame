#include "../statichook.h"

#include <memory>
#include <minecraft/nbt/CompoundTag.h>

TClasslessInstanceHook(std::unique_ptr<CompoundTag>, _ZN9DBStorage14loadPlayerDataERKSsS1_S1_) {
    return nullptr;
}
TClasslessInstanceHook(void, _ZN12LevelStorage14savePlayerDataERKSsRK11CompoundTag) {
}