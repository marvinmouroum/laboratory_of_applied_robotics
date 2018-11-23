//
//  Visualizer.hpp
//  LAR_1_0
//
//  Created by Air Marvin on 22.11.18.
//  Copyright © 2018 Air Marvin. All rights reserved.
//

#ifndef Visualizer_hpp
#define Visualizer_hpp

#include "../Headers/RoboticMapping.hpp"
#include "../../dubins/PathPlanning.hpp"


#include <stdio.h>

/**
 \brief class that visualizes the steps taken by robotic mapping and path planning
 */
class Visualizer {
    
public:
    Visualizer();
    Visualizer(Map &map);
    Visualizer(Map &map, Path* &path);
    ~Visualizer();
    
    ///override the map object to visualize
    void assign_map(Map &map);
    ///override path object to visualize
    void assign_path(Path *&path);
    ///create an image based on the information
    void visualize();
    ///simulate the robots way along the path
    void simulate();
    
private:
    Map * p_map;
    Path * p_path;
    
    const std::string windowtitle = "window";
    
    void play();
    
    cv::Mat print_arena(cv::Mat &result);
    cv::Mat print_grid(cv::Mat &result);
    cv::Mat print_shapes(cv::Mat &result);
    cv::Mat print_path(cv::Mat &result);
    
    cv::Mat merge(cv::Mat &input, cv::Mat &overlay, cv::Scalar color);
    
    Robot * car;
};

#endif /* Visualizer_hpp */