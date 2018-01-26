#include "FakeNetherDimension.h"

#include <minecraft/level/BiomeSource.h>
#include <minecraft/level/MainChunkSource.h>
#include <minecraft/level/BlockSource.h>
#include <minecraft/level/RuntimeLightingManager.h>
#include "EmptyWorldGenerator.h"

FakeNetherDimension::FakeNetherDimension(Level& level) : Dimension(level, (DimensionId) 1, 16) {
    maxBrightness = 0xff;
    unk_76 = true;
    unk_14 = 0x3F;
    biomeSource = ThreadLocal<BiomeSource>([]() { return std::unique_ptr<BiomeSource>(new BiomeSource(7, (GeneratorType) 2)); });
}

void FakeNetherDimension::init() {
    EmptyWorldGenerator* generator = new EmptyWorldGenerator(this);
    this->generator = generator;
    std::unique_ptr<ChunkSource> chunkSource (generator);
    chunkSource = std::unique_ptr<ChunkSource>(new MainChunkSource(std::move(chunkSource)));
    this->chunkSource = std::move(chunkSource);
    this->blockSource = std::unique_ptr<BlockSource>(new BlockSource(*level, *this, *this->chunkSource, false, true));
    this->runtimeLightingManager = std::unique_ptr<RuntimeLightingManager>(new RuntimeLightingManager(*this));
}