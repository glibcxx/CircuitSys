#pragma once

#include <string>
#include <array>
#include <vector>
#include <cstdint>

// 下上北南西东
typedef uint8_t FacingID;

class BlockPos;
class Vec3;

class Facing
{
public:
    enum /*class*/ Name
    {
        DOWN = 0,
        UP = 1,
        NORTH = 2,
        SOUTH = 3,
        WEST = 4,
        EAST = 5,
        MAX = 6,
        NOT_DEFINED = 6,
        NUM_CULLING_IDS = 7,
    };

    enum class Rotation
    {
        NONE = 0,
        CCW = 1,
        OPP = 2,
        CW = 3,
    };

    static const Name                                 DIRECTIONS[6];
    static const BlockPos                             DIRECTION[6];
    static const Vec3                                 NORMAL[6];
    static const int                                  FACING2D[6];
    static const Name                                 BY2DDATA[4];
    static const uint8_t                              FACINGMASK[6];
    static const uint8_t                              FACINGMASK_INV[6];
    static const std::vector<FacingID>                ALL_FACES;
    static const std::array<std::vector<FacingID>, 6> ALL_EXCEPT;
    static const std::array<std::vector<FacingID>, 6> ALL_EXCEPT_AXIS_Y;

    static Name from2DDataValue(int data);

    static FacingID getClockWise(FacingID face);

    static FacingID getCounterClockWise(FacingID face);

    static FacingID getOpposite(FacingID face);

    static FacingID rotateFace(FacingID face, Facing::Rotation rot);

    static FacingID fromVec3(const Vec3 &dir);

    static const FacingID OPPOSITE_FACING[7];
    static const int      STEP_X[6];
    static const int      STEP_Y[6];
    static const int      STEP_Z[6];

    static int getStepX(const FacingID facing);

    static int getStepY(const FacingID facing);

    static int getStepZ(const FacingID facing);

    static bool isHorizontal(const FacingID facing);

    static FacingID convertYRotationToFacingDirection(float yRotation);

    class Plane
    {
    public:
        static std::vector<FacingID> HORIZONTAL;
    };
};
