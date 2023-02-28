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
        left -= right;
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

    inline friend bool operator==(const Vector3& left, const Vector3& right)
    {
        return left.x == right.x && left.y == right.y && left.z == right.z;
    }
    inline friend bool operator!=(const Vector3& left, const Vector3& right)
    {
        return !(left == right);
    }

    inline friend bool operator<(const Vector3& left, const Vector3& right)
    {
        return left.magnitude() < right.magnitude();
    }
    inline friend bool operator>(const Vector3& left, const Vector3& right)
    {
        return right < left;
    }
    inline friend bool operator<=(const Vector3& left, const Vector3& right)
    {
        return !(left > right);
    }
    inline friend bool operator>=(const Vector3& left, const Vector3& right)
    {
        return !(left < right);
    }

    float x, y, z;

    static const Vector3 zero;
    static const Vector3 one;
    static const Vector3 right;
    static const Vector3 up;
    static const Vector3 left;
    static const Vector3 down;
};

Vector3 const Vector3::zero = Vector3(0, 0, 0);
Vector3 const Vector3::one = Vector3(1, 1, 1);
Vector3 const Vector3::right = Vector3(1, 0, 0);
Vector3 const Vector3::up = Vector3(0, 1, 0);
Vector3 const Vector3::left = Vector3(-1, 0, 0);
Vector3 const Vector3::down = Vector3(0, -1, 0);

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

bool PointInCube(const Vector3& _pos, const Cube& _cube)
{
    return PointInCuboid(_pos, _cube);
}

bool RayIntersectSphere(const Vector3& _startPos, const Vector3& _targetPos, const Sphere& _sphere, Vector3& _outProjectedPos)
{
    // Calculate target axis relative to start
    Vector3 targetAxis = _targetPos - _startPos;
    // Calculate obstacle axis relative to start
    Vector3 obstacleAxis = _sphere.position - _startPos;

    // Calculate angle between target axis and obstacle axis
    float angle = Vector3::angle(targetAxis, obstacleAxis);

    // Calculate scalar projection of obstacle axis onto target axis
    float projectionValue = obstacleAxis.magnitude() * cos(angle);
    // Clamp projection value to target axis length
    projectionValue = std::min(projectionValue, targetAxis.magnitude());
    projectionValue = std::max(projectionValue, 0.0f);
    // Calculate projected axis with normalized direction of target axis
    Vector3 projectedAxis = targetAxis.normalize() * projectionValue;

    // Return projected pos relative to world
    _outProjectedPos = projectedAxis + _startPos;

    // Compare length of perpendicular vector between obstacle and projected to length of the obstacle radius
    return (projectedAxis - obstacleAxis).magnitude() <= _sphere.radius;
}

bool RaycastTarget(const Vector3& _startPos, const Vector3& _targetPos, std::vector<Sphere> _obstacles)
{
    for (int i = 0; i < _obstacles.size(); ++i)
    {
        Vector3 _;
        if (RayIntersectSphere(_startPos, _targetPos, _obstacles[i], _))
            return false;
    }
    return true;
}

struct Runner
{
    Runner(sf::RenderWindow& _window)
        : window(_window) {}

    sf::RenderWindow& window;

    sf::Vector2i mousePosition;

    void DrawLine(Vector3 a, Vector3 b, sf::Color color)
    {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2(a.x, a.y), color),
            sf::Vertex(sf::Vector2(b.x, b.y), color)
        };

        window.draw(line, 2, sf::Lines);
    }

    void DrawSphere(Vector3 center, float radius, sf::Color fillColor, sf::Color outlineColor)
    {
        sf::CircleShape circle(radius);

        circle.setFillColor(fillColor);
        circle.setOutlineThickness(1);
        circle.setOutlineColor(outlineColor);

        circle.setPosition(center.x - radius, center.y - radius);

        window.draw(circle);
    }

    void RunLineIntersectSphereExample()
    {
        Vector3 startPos(360, 480, 0);
        Vector3 targetPos(470, 60, 0);
        Vector3 spherePos(mousePosition.x, mousePosition.y, 0);
        Sphere sphere(spherePos, 100);

        Vector3 projectedPos;
        bool intersect = RayIntersectSphere(startPos, targetPos, sphere, projectedPos);

        sf::Color outlineColor = sf::Color::Transparent;
        if (intersect == true)
            outlineColor = sf::Color::Green;

        DrawSphere(sphere.position, sphere.radius, sf::Color(20, 20, 20), outlineColor);

        DrawLine(startPos, targetPos, sf::Color::White);
        DrawLine(startPos, spherePos, sf::Color::Yellow);
        DrawLine(spherePos, projectedPos, sf::Color::Red);
    }

    void RunLineIntersectSpheresExample()
    {
        Vector3 startPos(480, 500, 0);
        Vector3 targetPos(mousePosition.x, mousePosition.y, 0);

        int sphereCount = 3;
        Sphere spheres[] = {
            Sphere(Vector3(100, 100, 0), 50),
            Sphere(Vector3(800, 350, 0), 70),
            Sphere(Vector3(500, 250, 0), 100)
        };

        for (int i = 0; i < sphereCount; ++i)
        {
            Sphere& sphere = spheres[i];
            Vector3 projectedPos;
            bool intersect = RayIntersectSphere(startPos, targetPos, sphere, projectedPos);

            sf::Color outlineColor = intersect ? sf::Color::Green : sf::Color::Transparent;

            DrawSphere(sphere.position, sphere.radius, sf::Color(20, 20, 20), outlineColor);

            if (intersect == true)
            {
                DrawLine(startPos, sphere.position, sf::Color::Yellow);
                DrawLine(sphere.position, projectedPos, sf::Color::Red);
            }
        }

        DrawLine(startPos, targetPos, sf::Color::White);
    }

    void Run()
    {
        window.clear(sf::Color::Black);

        mousePosition = sf::Mouse::getPosition(window);
        RunLineIntersectSpheresExample();

        window.display();
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(960, 540), "Math Test");
    window.setPosition(sf::Vector2(960, 540));

    Runner runner(window);

    while (window.isOpen())
    {
        runner.Run();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return 0;
}
