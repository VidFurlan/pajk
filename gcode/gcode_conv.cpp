#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <cstdint>

const float maxDistance = 0.2;

struct Point {
    float x, y, z, time = 0;
    bool positional;
};

//Adjust as needed
const Point TOPLEFT_BORDER = {-1, 10};
const Point TOPRIGHT_BORDER = {5, 10};
const Point BOTTOMLEFT_BORDER= {-1, 4};
const Point BOTTOMRIGHT_BORDER = {5, 4};

float distance(Point a, Point b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

/// @brief 
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, char** argv) {
    if(argc < 2){
        std::cerr << "Please provide the path for the gcode file\n";
        return -1;
    }
    std::ifstream inputFile(argv[1]);
    std::vector<std::string> inputLines;
    std::string line;

    if(!inputFile.is_open()){
        std::cerr << "Invalid file path\n";
        return -1;
    }
    
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
    int i = -1;
    for (auto line : inputLines) {
        i++;
        if(i % 1 != 0){
            continue;
        }
        
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
            }
        }

        points.push_back(point);
        std::cout << "x: " << std::setw(10) << point.x << " \ty: " << std::setw(10) << point.y << " \tz: " << std::setw(10) << point.z << " \tt: " << std::setw(10) << point.time << std::endl;
    }

    float xDivisor = abs(TOPLEFT_BORDER.x - TOPRIGHT_BORDER.x);
    float yDivisor = abs(TOPLEFT_BORDER.y - BOTTOMLEFT_BORDER.y);



    // Create output file
    std::ofstream outputFile("points.hpp");
    outputFile << "#pragma once\n\n";
    outputFile << "const float maxDistance = " << maxDistance << ";\n\n";
    outputFile << "struct Point {\n" ;
    outputFile << "    double x, y;\n";
    outputFile << "    int z, time, positional; \n";
    outputFile << "};\n\n";
    outputFile << "const Point points[] = {\n";
    for (Point point : processedPoints){
        outputFile << "    {" << point.x << ", " << point.y << ", " << point.z << ", " << point.time << ", " << point.positional << "},\n";
    }
    outputFile << "};\n";

    outputFile.close();
}
