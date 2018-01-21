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

public:
    static std::map<Player*, int> sendPlayerToDimension;

    static int defineDimension(std::unique_ptr<LevelStorage> storage);

    static std::unique_ptr<Dimension> createDimension(Level& level, int index);

    static void sendPlayer(Player* player, int dimension);

    MinigameDimension(Level& level, DimensionId dimensionId, short maxHeight = 0x100);

    virtual std::string getName() const {
        return "MinigameDimension";
    }
    virtual Vec3 translatePosAcrossDimension(Vec3 const& v, DimensionId) const {
        return v;
    }

};