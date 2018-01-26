#include <minecraft/level/WorldGenerator.h>
#include <minecraft/level/ChunkSource.h>
#include <minecraft/level/Dimension.h>

enum class StructureFeatureType;

class EmptyWorldGenerator : public ChunkSource, public WorldGenerator {

public:
    EmptyWorldGenerator(Dimension* dimension) : ChunkSource(dimension, 0x10) {}

    virtual void loadChunk(LevelChunk&, bool);

    virtual int getFeatureTypeAt(BlockPos const&) { return 0; }
    virtual int findNearestFeature(StructureFeatureType, BlockPos const&, BlockPos&) { return 0; }
    virtual void prepareHeights(BlockVolume&, int, int) { }
    virtual void prepareStructureBlueprints(ChunkPos const&) { }
    virtual void garbageCollectBlueprints(buffer_span<ChunkPos>) { }

};