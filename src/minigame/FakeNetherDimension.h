#pragma once

#include <vector>
#include <memory>
#include <map>
#include <minecraft/level/Dimension.h>
#include <minecraft/math/Vec3.h>

class LevelStorage;

class FakeNetherDimension : public Dimension {

public:
    FakeNetherDimension(Level& level);

    virtual void init();
    virtual std::string getName() const {
        return "FakeNetherDimension";
    }
    virtual Vec3 translatePosAcrossDimension(Vec3 const& v, DimensionId) const {
        return v;
    }

};