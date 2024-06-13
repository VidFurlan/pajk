#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

const float maxDistance = 0.2;

struct Point {
    float x, y, z, time = 0;
    bool positional;
};

float distance(Point a, Point b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

int main() {
    std::ifstream inputFile("input.gcode");
    std::vector<std::string> inputLines;
    std::string line;
    
    // Skip to the first movement command
    while (std::getline(inputFile, line)) {
        //std::cout << (int)(line.substr(0, 3) == "M03") << std::endl;
        if (line.size() >= 3 && line.substr(0, 3) == "M03")
            break;
    }


    // Read movement commands
    while (std::getline(inputFile, line))
        if (!line.empty() && line[0] == 'G' && line.find('F') == std::string::npos)
            inputLines.push_back(line);

    inputFile.close();

    std::vector<Point> points = {Point{0, 0, 0, 0, false}};
    for (auto line : inputLines) {
        Point point;
        point.positional = (line[2] == '0');
        

        // Read coordinates
        if (line.find('X') != std::string::npos)
            point.x = std::stod(line.substr(line.find('X') + 1, line.find(' ', line.find('X')) - line.find('X') - 1));
        else 
            point.x = points.back().x;

        if (line.find('Y') != std::string::npos)
            point.y = std::stod(line.substr(line.find('Y') + 1, line.find(' ', line.find('Y')) - line.find('Y') - 1));
        else
            point.y = points.back().y;

        if (line.find('Z') != std::string::npos)
            point.z = std::stod(line.substr(line.find('Z') + 1, line.find(' ', line.find('Z')) - line.find('Z') - 1));
        else
            point.z = points.back().z;


        // Create additional points if distance is too large
        if (distance(points.back(), point) > maxDistance) {
            Point pointBack = points.back();
            std::cout << std::endl;
            for (int i = 1; i * maxDistance < distance(pointBack, point); i++) {
                Point newPoint;
                newPoint.x = pointBack.x + (point.x - pointBack.x) * i / ceil(distance(pointBack, point) / maxDistance);
                newPoint.y = pointBack.y + (point.y - pointBack.y) * i / ceil(distance(pointBack, point) / maxDistance);
                newPoint.z = pointBack.z + (point.z - pointBack.z) * i / ceil(distance(pointBack, point) / maxDistance);

                points.push_back(newPoint);
                std::cout << "x: " << std::setw(10) << newPoint.x << " \ty: " << std::setw(10) << newPoint.y << " \tz: " << std::setw(10) << newPoint.z << " \tt: " << std::setw(10) << newPoint.time << std::endl;
            }
        }

        points.push_back(point);
        std::cout << "x: " << std::setw(10) << point.x << " \ty: " << std::setw(10) << point.y << " \tz: " << std::setw(10) << point.z << " \tt: " << std::setw(10) << point.time << std::endl;
    }


    // Create output file
    std::ofstream outputFile("output.hpp");
    outputFile << "#pragma once\n\n";
    outputFile << "const float maxDistance = " << maxDistance << ";\n\n";
    outputFile << "struct Point {\n";
    outputFile << "    float x, y, z, time = 0;\n";
    outputFile << "    bool positional;\n";
    outputFile << "};\n\n";
    outputFile << "const Point points[] = {\n";
    for (Point point : points) {
        outputFile << "    {" << point.x << ", " << point.y << ", " << point.z << ", " << point.time << ", " << point.positional << "},\n";
    }
    outputFile << "};\n";

    outputFile.close();
}
