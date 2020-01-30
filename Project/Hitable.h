#pragma once
#include "Structs.h"
#include "AxisAllignedBox.h"
#include <memory>

struct Ray;

class Hitable
{
public:

    virtual Hitable* Intersect(Ray& a_Ray, float& a_Dist) = 0;

    virtual void CalculateBoundingBox() = 0;

    virtual Color GetColor() const = 0;

    const AxisAllignedBox* GetBoundingBox() const { return m_Box.get(); }
protected:

    std::unique_ptr<AxisAllignedBox> m_Box;
};
