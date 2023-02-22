// Math Algebra Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cmath>
#include <vector>
#include <SFML\Graphics.hpp>

struct Vector3
{
    Vector3()
        : x(0), y(0), z(0) {}
    Vector3(float _x, float _y, float _z)
        : x(_x), y(_y), z(_z) {}

    Vector3(const Vector3& other) = default;
    Vector3(Vector3&& other) = default;

    ~Vector3() = default;

    float magnitude() const
    {
        return sqrt((x * x) + (y * y) + (z * z));
    }

    Vector3 normalize() const
    {
        float length = magnitude();
        return Vector3(x / length, y / length, z / length);
    }

    static float dot(const Vector3& a, const Vector3& b)
    {
        return (a.x * b.x) + (a.y * b.y);
    }

    static float angle(const Vector3& a, const Vector3& b)
    {
        return acos(
            dot(a, b) / (a.magnitude() * b.magnitude())
        );
    }

    Vector3& operator=(const Vector3& right) = default;
    Vector3& operator=(Vector3&& right) noexcept = default;

    Vector3& operator+=(const Vector3& right)
    {
        x += right.x;
        y += right.y;
        z += right.z;

        return *this;
    }
    friend Vector3 operator+(Vector3 left, const Vector3& right)
    {
        left += right;
        return left;
    }
    
    Vector3& operator-=(const Vector3& right)
    {
        x -= right.x;
        y -= right.y;
        z -= right.z;

        return *this;
    }
    friend Vector3 operator-(Vector3 left, const Vector3& right)
    {
        left += right;
        return left;
    }

    Vector3& operator*=(float right)
    {
        x *= right;
        y *= right;
        z *= right;

        return *this;
    }
    friend Vector3 operator*(Vector3 left, float right)
    {
        left *= right;
        return left;
    }

    Vector3& operator/=(float right)
    {
        x /= right;
        y /= right;
        z /= right;

        return *this;
    }
    friend Vector3 operator/(Vector3 left, float right)
    {
        left /= right;
        return left;
    }

    inline bool operator==(const Vector3& right)
    {
        return x == right.x && y == right.y && z == right.z;
    }
    inline bool operator!=(const Vector3& right)
    {
        return !(*this == right);
    }

    float x, y, z;
};

struct Sphere
{
    Sphere(const Vector3& _position, float _radius)
        : position(_position), radius(_radius) {}

    Vector3 position;
    float radius;
};

struct Cuboid
{
    Cuboid(const Vector3& _position, float _length, float _height, float _depth)
        : position(_position), length(_length), height(_height), depth(_depth) {}

    Vector3 position;

    float length;
    float height;
    float depth;
};

struct Cube : public Cuboid
{
    Cube(const Vector3& _position, float _length)
        : Cuboid(_position, _length, _length, _length) {}
};

bool PointInRange(const Vector3& _pos, const Vector3& _targetPos, float _range)
{
    return (_pos - _targetPos).magnitude() <= _range;
}
bool PointInSphere(const Vector3& _pos, const Sphere& _sphere)
{
    return (_pos - _sphere.position).magnitude() <= _sphere.radius;
}

bool PointInCube(const Vector3& _pos, const Cube& _cube)
{
    return PointInCuboid(_pos, _cube);
}

bool PointInCuboid(const Vector3& _pos, const Cuboid& _cuboid)
{
    float cubeMinX = _cuboid.position.x - _cuboid.length / 2;
    float cubeMaxX = _cuboid.position.x + _cuboid.length / 2;

    float cubeMinY = _cuboid.position.y - _cuboid.height / 2;
    float cubeMaxY = _cuboid.position.y + _cuboid.height / 2;

    float cubeMinZ = _cuboid.position.z - _cuboid.depth / 2;
    float cubeMaxZ = _cuboid.position.z + _cuboid.depth / 2;

    return (_pos.x <= cubeMaxX && _pos.x >= cubeMinX) && (_pos.y <= cubeMaxY && _pos.y >= cubeMinY) && (_pos.z <= cubeMaxZ && _pos.z >= cubeMinZ);
}

bool RayIntersectSphere(const Vector3& _startPos, const Vector3& _targetPos, const Sphere& _sphere)
{
    // Calculate dir to target
    Vector3 targetDir = _targetPos - _startPos;
    // Calculate dir to obstacle
    Vector3 obstacleDir = _sphere.position - _startPos;

    // Calculate angle between target dir and obstacle dir
    float angle = Vector3::angle(targetDir, obstacleDir);

    // Calculate scalar projection of obstacle dir onto target dir
    Vector3 projectedDir = targetDir.normalize() * (obstacleDir.magnitude() * cos(angle));

    // Compare length of perpendicular vector between obstacle and projected to length of the obstacle radius
    return (projectedDir - obstacleDir).magnitude() <= _sphere.radius;
}

bool RaycastTarget(const Vector3& _startPos, const Vector3& _targetPos, std::vector<Sphere> _obstacles)
{
    for (int i = 0; i < _obstacles.size(); ++i)
    {
        if (RayIntersectSphere(_startPos, _targetPos, _obstacles[i]))
            return false;
    }
    return true;
}

int main()
{
    // Test your code with SFML graphics library
}
