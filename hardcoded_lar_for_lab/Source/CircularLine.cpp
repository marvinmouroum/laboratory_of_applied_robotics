#include "../Headers/CircularLine.hpp"

using namespace PathE2D;
using namespace Element;

CircularLine::CircularLine(Position start_point, double curvature, double length) {
    setStartPoint(start_point);
    setCurvature(curvature);
    setLength(length);
    Position end_point = findPointDistance(curvature, start_point, length);
    setEndPoint(end_point);
};


CircularLine::~CircularLine() {
};







