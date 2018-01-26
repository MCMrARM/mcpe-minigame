#include "EmptyWorldGenerator.h"

#include <minecraft/level/LevelChunk.h>

void EmptyWorldGenerator::loadChunk(LevelChunk& chunk, bool) {
    chunk._setGenerator(this);
    chunk.setSaved();
    chunk.changeState((ChunkState) 1, (ChunkState) 2);
}
