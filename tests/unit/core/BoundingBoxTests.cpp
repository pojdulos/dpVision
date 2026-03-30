#include "../../support/TestFramework.h"

#include "../../../src/core/BoundingBox.h"

namespace {

CPoint3d point(double x, double y, double z)
{
    return CPoint3d(x, y, z);
}

void requirePointEq(const CPoint3d& actual, const CPoint3d& expected)
{
    REQUIRE_EQ(actual.X(), expected.X());
    REQUIRE_EQ(actual.Y(), expected.Y());
    REQUIRE_EQ(actual.Z(), expected.Z());
}

} // namespace

TEST_CASE(BoundingBoxDefaultConstructorCreatesInvalidBox)
{
    CBoundingBox box;

    REQUIRE(box.isInvalid());
    REQUIRE(!box.isValid());
    REQUIRE(!box.isNull());
}

TEST_CASE(BoundingBoxNullResetCreatesValidNullBoxAtOrigin)
{
    CBoundingBox box;

    box.reset(CBoundingBox::NullBB);

    REQUIRE(box.isValid());
    REQUIRE(box.isNull());
    requirePointEq(box.getMin(), point(0.0, 0.0, 0.0));
    requirePointEq(box.getMax(), point(0.0, 0.0, 0.0));
}

TEST_CASE(BoundingBoxExpandPointBuildsExtentsFromInvalidBox)
{
    CBoundingBox box;

    box.expand(point(3.0, -2.0, 5.0));
    box.expand(point(-1.0, 4.0, 2.0));

    REQUIRE(box.isValid());
    requirePointEq(box.getMin(), point(-1.0, -2.0, 2.0));
    requirePointEq(box.getMax(), point(3.0, 4.0, 5.0));
}

TEST_CASE(BoundingBoxExpandByBoxMergesBothExtents)
{
    CBoundingBox base(point(-1.0, 0.0, 2.0), point(2.0, 3.0, 5.0));
    CBoundingBox other(point(-4.0, 1.0, -2.0), point(1.0, 7.0, 4.0));

    base.expand(other);

    requirePointEq(base.getMin(), point(-4.0, 0.0, -2.0));
    requirePointEq(base.getMax(), point(2.0, 7.0, 5.0));
}

TEST_CASE(BoundingBoxContainsTreatsBoundaryAsInside)
{
    CBoundingBox box(point(-1.0, -2.0, -3.0), point(4.0, 5.0, 6.0));

    REQUIRE(box.contains(point(-1.0, -2.0, -3.0)));
    REQUIRE(box.contains(point(4.0, 5.0, 6.0)));
    REQUIRE(box.contains(point(0.0, 0.0, 0.0)));
    REQUIRE(!box.contains(point(4.1, 5.0, 6.0)));
    REQUIRE(!box.contains(point(4.0, 5.1, 6.0)));
    REQUIRE(!box.contains(point(4.0, 5.0, 6.1)));
}

TEST_CASE(BoundingBoxIntersectionReturnsOverlapRegion)
{
    CBoundingBox a(point(0.0, 0.0, 0.0), point(5.0, 5.0, 5.0));
    CBoundingBox b(point(3.0, -1.0, 2.0), point(7.0, 4.0, 8.0));

    CBoundingBox overlap = CBoundingBox::intersection(a, b);

    REQUIRE(overlap.isValid());
    requirePointEq(overlap.getMin(), point(3.0, 0.0, 2.0));
    requirePointEq(overlap.getMax(), point(5.0, 4.0, 5.0));
}

TEST_CASE(BoundingBoxIntersectionReturnsInvalidForDisjointBoxes)
{
    CBoundingBox a(point(0.0, 0.0, 0.0), point(1.0, 1.0, 1.0));
    CBoundingBox b(point(2.0, 2.0, 2.0), point(3.0, 3.0, 3.0));

    CBoundingBox overlap = CBoundingBox::intersection(a, b);

    REQUIRE(overlap.isInvalid());
}

TEST_CASE(BoundingBoxHitReturnsEntryPointForIncomingRay)
{
    CBoundingBox box(point(0.0, 0.0, 0.0), point(2.0, 2.0, 2.0));
    CPoint3d hitPoint;

    const bool hit = box.hit(point(-1.0, 1.0, 1.0), CVector3d(1.0, 0.0, 0.0), hitPoint);

    REQUIRE(hit);
    requirePointEq(hitPoint, point(0.0, 1.0, 1.0));
}

TEST_CASE(BoundingBoxHitReturnsFalseWhenRayMissesBox)
{
    CBoundingBox box(point(0.0, 0.0, 0.0), point(2.0, 2.0, 2.0));
    CPoint3d hitPoint;

    const bool hit = box.hit(point(-1.0, 3.0, 1.0), CVector3d(1.0, 0.0, 0.0), hitPoint);

    REQUIRE(!hit);
}

TEST_CASE(BoundingBoxLongestAxisReturnsAxisWithGreatestSpan)
{
    CBoundingBox xLongest(point(0.0, 0.0, 0.0), point(5.0, 4.0, 3.0));
    CBoundingBox yLongest(point(0.0, 0.0, 0.0), point(2.0, 6.0, 3.0));
    CBoundingBox zLongest(point(0.0, 0.0, 0.0), point(2.0, 4.0, 7.0));

    REQUIRE_EQ(xLongest.longest_axis(), 0);
    REQUIRE_EQ(yLongest.longest_axis(), 1);
    REQUIRE_EQ(zLongest.longest_axis(), 2);
}