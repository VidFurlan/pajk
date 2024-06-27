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
const uint32_t DEFAULT_POINT_AMOUNT = 256; //chosen at random lol

struct Point {
    float x, y;
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


    std::vector<Point> points = {};

    int i = -1;

    float maxX = 0;
    float maxY = 0;

    for (auto line : inputLines) {
        i++;
        if(i % 1 != 0){
            continue;
        }
        
        Point point;

        // Read coordinates
        if (line.find('X') != std::string::npos)
            point.x = std::stod(line.substr(line.find('X') + 1, line.find(' ', line.find('X')) - line.find('X') - 1));
        else 
            point.x = points.back().x;

        if (line.find('Y') != std::string::npos)
            point.y = std::stod(line.substr(line.find('Y') + 1, line.find(' ', line.find('Y')) - line.find('Y') - 1));
        else
            point.y = points.back().y;

        maxX = std::max(maxX, point.x);
        maxY = std::max(maxY, point.y);

        if(points.empty()){
            points.push_back(point);
            continue;
        }


        // Create additional points if distance is too large
        if (distance(points.back(), point) > maxDistance) {
            points.push_back(point);
        }else{
            Point pointBack = points.back();
            Point newPoint = {(pointBack.x + point.x) / 2, (pointBack.y + point.y) / 2};
            
            points.push_back(newPoint);
        }

    }

    std::vector<Point> processedPoints;

    uint32_t pointNum = (argc >= 3) ? std::stoi(argv[2]) : DEFAULT_POINT_AMOUNT; 

    uint32_t pointsToBeCondensed = floor(points.size()/pointNum);
    for(uint32_t i = 0; i < points.size()-pointsToBeCondensed; i += pointsToBeCondensed){
        uint32_t sumX = 0;
        uint32_t sumY = 0;
        for(uint32_t y = 0; y < pointsToBeCondensed; y++){
            sumX += points[i+y].x;
            sumY += points[i+y].y;
        }

        processedPoints.push_back({(float)sumX / pointsToBeCondensed, (float)sumY / pointsToBeCondensed});
    }

    float xBorderSize = abs(TOPLEFT_BORDER.x - TOPRIGHT_BORDER.x);
    float yBorderSize = abs(TOPLEFT_BORDER.y - BOTTOMLEFT_BORDER.y);
    float xOffset = BOTTOMLEFT_BORDER.x;
    float yOffset = BOTTOMLEFT_BORDER.y;


    // Create output file
    std::ofstream outputFile("points.hpp");
    outputFile << "#pragma once\n\n";
    outputFile << "const float maxDistance = " << maxDistance << ";\n\n";
    outputFile << "struct Point {\n" ;
    outputFile << "    float x, y;\n";
    outputFile << "};\n\n";
    outputFile << "const Point points[] = {\n";
    for (Point point : processedPoints){
        point.x = (point.x / maxX) * xBorderSize + xOffset;
        point.y = (point.y / maxY) * yBorderSize + yOffset;

        outputFile << "    {" << point.x << ", " << point.y << "},\n";
    }
    outputFile << "};\n";

    outputFile.close();
}
