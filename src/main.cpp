#include <Arduino.h>
#include <Servo.h>
#include "../gcode/points.hpp"

#include "../gcode/points.hpp"

struct pair {
  float x, y;
};

const int CIRCLE_RES = 64;
pair circle[CIRCLE_RES];

Servo ml, mr;

const float LEN = 7; // Length of link in cm
const float DIST = 4; // Distance between origins in cm
float x, y, dl, dr, hl, hr;
float pl1, pl2, pr1, pr2;
int tl, tr;


/// @brief Function to calculate the angle of the left motor
/// @return Returns the angle of the left motor
float angle_left(float x, float y) {
  
  dl = sqrt((x * x) + (y * y)); // Distance between origin left and (x,y) point
  hl = sqrt((LEN * LEN) - ((dl / 2) * (dl / 2))); // Height of the triangle
  
  pl1 = atan2(hl, dl / 2); // Phi 1, angle between d and axis
  pl2 = atan2(y, x); // Phi 2, angle between d and l 
  
  tl = (pl1 + pl2) * 180 / PI; // Angle between axis and l

  //Serial.println("Left: ");
  // Serial.println(hl);
  // Serial.println(pl1 * 180 / pi);
  // Serial.println(pl2 * 180 / pi);
  // Serial.println(tl);

  return tl;
}

/// @brief Function to calculate the angle of the right motor
/// @return Returns the angle of the right motor 
float angle_right(float x, float y) {
  x -= DIST;

  
  dr = sqrt((x * x) + (y * y)); // Distance between origin right and (x,y) point
  hr = sqrt((LEN * LEN) - ((dr / 2) * (dr / 2))); // Height of the triangle

  pr1 = atan2(hr, dr / 2); // Phi 1, angle between d and axis
  pr2 = atan2(y, x); // Phi 2, angle between d and l 

  tr = (pr2 - pr1) * 180 / PI;

  //Serial.println("Right: ");
  // Serial.println(hr);
  // Serial.println(pr1 * 180 / pi);
  // Serial.println(pr2 * 180 / pi);
  // Serial.println(tr);

  return tr;
}

// Extreme Left Point
void xleft() { 
  mr.write(110);
  ml.write(110);
}

// Extreme Right Point
void xright() { 
  mr.write(45);
  ml.write(45);
}

// Extreme Top Point
void xtop() {
  mr.write(90);
  ml.write(90);
}

// Extreme Bottom Point 
void xbottom() {
  mr.write(60);
  ml.write(120);
}

/// @brief Function to precompute points of circle
/// @param pos Center of the circle
/// @param points Array to store the points
/// @param circle_radius Radius of the circle
void precompute_compute_circle(pair pos, pair points[], int circle_radius) {
  const float angle = radians(360.0 / CIRCLE_RES);

  for (int i = 0; i < CIRCLE_RES; i++)
    points[i] = {pos.x + sin(angle * i) * circle_radius, pos.y + cos(angle * i) * circle_radius};
}

void setup() {
  ml.attach(5);
  mr.attach(3);
  Serial.begin(115200);
  ml.write(90);
  mr.write(90);
  delay(2000);

  precompute_compute_circle({0, 9}, circle, 1);
  ml.write(angle_left(2, 5));
  mr.write(angle_right(2, 5));
}

void loop() {
 /* for (int i = 0; i < CIRCLE_RES; i++) {
    //Serial.println("Circle: ");
    Serial.println(circle[i].x);
    Serial.println(circle[i].y);
    ml.write(angle_left(circle[i].x, circle[i].y));
    mr.write(angle_right(circle[i].x, circle[i].y));
    delay(10);
  }*/

  for(int i = 0; i < sizeof(points) / sizeof(Point); i++){
    ml.write(angle_left(points[i].x, points[i].y));
    mr.write(angle_right(points[i].x, points[i].y));
    delay(50);
  }
}