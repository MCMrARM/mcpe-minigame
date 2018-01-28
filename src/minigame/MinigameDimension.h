#pragma once

#include <vector>
#include <memory>
#include <map>
#include <minecraft/level/Dimension.h>
#include <minecraft/math/Vec3.h>

class LevelStorage;

class MinigameDimension : public Dimension {

private:
    struct DefinedDimension {
        std::unique_ptr<LevelStorage> storage;
    };
    static std::vector<DefinedDimension> dimensions;

    LevelStorage* storage;

    bool shouldDelete = false;

public:
    static int defineDimension(std::unique_ptr<LevelStorage> storage);

    static std::unique_ptr<Dimension> createDimension(Level& level, int index);

    static void sendPlayerToDimension(Player* player, int dimension, Vec3 targetPos);

    static void deleteUnusedDimensions(Level& level);

    MinigameDimension(Level& level, LevelStorage* storage, DimensionId dimensionId, short maxHeight = 0x100);

    virtual void init();
    virtual std::string getName() const {
        return "MinigameDimension";
    }
    virtual Vec3 translatePosAcrossDimension(Vec3 const& v, DimensionId) const {
        return v;
    }

    void requestDeletion();

};