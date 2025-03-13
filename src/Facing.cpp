#include "Facing.h"

#include "level/BlockPos.h"
#include "phys/Vec3.h"
#include "deps/Math.h"

const Facing::Name Facing::DIRECTIONS[6] = {
    Facing::DOWN,
    Facing::UP,
    Facing::NORTH,
    Facing::SOUTH,
    Facing::WEST,
    Facing::EAST,
};

const Vec3 Facing::NORMAL[6] = {
    {0.0f,  -1.0f, 0.0f },
    {0.0f,  1.0f,  0.0f },
    {0.0f,  0.0f,  1.0f },
    {0.0f,  0.0f,  -1.0f},
    {-1.0f, 0.0f,  0.0f },
    {1.0f,  0.0f,  0.0f },
};

Facing::Name const Facing::BY2DDATA[4] = {
    Facing::NORTH,
    Facing::SOUTH,
    Facing::WEST,
    Facing::EAST,
};

const uint8_t Facing::FACINGMASK[6] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20};

const uint8_t Facing::FACINGMASK_INV[6] = {0x3E, 0x3D, 0x3B, 0x37, 0x2F, 0x1F};

std::vector<FacingID> const Facing::ALL_FACES = {1, 0, 2, 3, 4, 5};

std::array<std::vector<FacingID>, 6> const Facing::ALL_EXCEPT = {
    std::vector<FacingID>{1, 2, 3, 4, 5},
    std::vector<FacingID>{0, 2, 3, 4, 5},
    std::vector<FacingID>{1, 0, 3, 4, 5},
    std::vector<FacingID>{1, 0, 2, 4, 5},
    std::vector<FacingID>{1, 0, 2, 3, 5},
    std::vector<FacingID>{1, 0, 2, 3, 4},
};

std::array<std::vector<FacingID>, 6> const Facing::ALL_EXCEPT_AXIS_Y = {
    std::vector<FacingID>{},
    std::vector<FacingID>{},
    std::vector<FacingID>{3, 4, 5},
    std::vector<FacingID>{2, 4, 5},
    std::vector<FacingID>{2, 3, 5},
    std::vector<FacingID>{2, 3, 4},
};

const int Facing::STEP_X[6] = {0, 0, 0, 0, -1, 1};
const int Facing::STEP_Y[6] = {-1, 1, 0, 0, 0, 0};
const int Facing::STEP_Z[6] = {0, 0, -1, 1, 0, 0};

const BlockPos Facing::DIRECTION[6] = {
    {0,  -1, 0 }, // DOWN
    {0,  1,  0 }, // UP
    {0,  0,  -1}, // NORTH
    {0,  0,  1 }, // SOUTH
    {-1, 0,  0 }, // WEST
    {1,  0,  0 }, // EAST
};

const FacingID Facing::OPPOSITE_FACING[7] = {1, 0, 3, 2, 5, 4, 6};

std::vector<FacingID> Facing::Plane::HORIZONTAL = {2, 5, 3, 4};

Facing::Name Facing::from2DDataValue(int data)
{
    return Facing::BY2DDATA[std::abs(data % 4)];
}

FacingID Facing::getClockWise(FacingID face)
{
    switch (face)
    {
    case 2:
        return 5;
    case 3:
        return 4;
    case 4:
        return 2;
    case 5:
        return 3;
    default:
        return 2;
    }
}

FacingID Facing::getCounterClockWise(FacingID face)
{
    switch (face)
    {
    case 2:
        return 4;
    case 3:
        return 2;
    case 4:
        return 3;
    case 5:
        return 2;
    default:
        return 2;
    }
}

FacingID Facing::getOpposite(FacingID face)
{
    switch (face)
    {
    case 0u:
        return 1;
    case 1u:
        return 0;
    case 2u:
        return 3;
    case 3u:
        return 2;
    case 4u:
        return 5;
    case 5u:
        return 4;
    default:
        return face;
    }
}

FacingID Facing::rotateFace(FacingID face, Facing::Rotation rot)
{
    if (rot == Facing::Rotation::NONE || face == Facing::DOWN || face == Facing::UP)
        return face;
    switch (rot) // NOLINT
    {
    case Facing::Rotation::CCW:
        return Facing::getCounterClockWise(face);
    case Facing::Rotation::OPP:
        return Facing::getOpposite(face);
    case Facing::Rotation::CW:
        return Facing::getClockWise(face);
    }
    return face;
}

FacingID Facing::fromVec3(const Vec3 &dir)
{
    Vec3 magnitude{std::abs(dir.x), std::abs(dir.y), std::abs(dir.z)};
    if (magnitude.lengthSquared() > 0.01f)
    {
        if (magnitude.y <= magnitude.x || magnitude.y <= magnitude.z)
        {
            if (magnitude.x <= magnitude.y || magnitude.x <= magnitude.z)
            {
                return dir.z <= 0.0 ? 2 : 3;
            }
            else
            {
                return dir.x <= 0.0f ? 4 : 5;
            }
        }
        else
        {
            return dir.y > 0.0;
        }
    }
    return 1;
}

int Facing::getStepX(const FacingID facing)
{
    return facing == Facing::WEST ? -1 : facing == Facing::EAST;
}

int Facing::getStepY(const FacingID facing)
{
    return facing == Facing::DOWN ? -1 : facing == Facing::UP;
}

int Facing::getStepZ(const FacingID facing)
{
    return facing == Facing::NORTH ? -1 : facing == Facing::SOUTH;
}

bool Facing::isHorizontal(const FacingID facing)
{
    return facing >= Facing::NORTH && facing <= Facing::EAST;
}

FacingID Facing::convertYRotationToFacingDirection(float yRotation)
{
    unsigned int cardinalDirection = mce::Math::floor(((4.0f * yRotation) / 360.0f) + 0.5f) & 3;
    return Facing::Plane::HORIZONTAL[cardinalDirection];
}
