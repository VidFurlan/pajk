#pragma once

const float maxDistance = 0.2;

struct Point {
    float x, y, z, time = 0;
    bool positional;
};

const Point points[] = {
    {0, 0, 0, 0, 0},
};
