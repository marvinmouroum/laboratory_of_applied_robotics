//
//  robot_project.cpp
//  LAR_1_0
//
//  Created by Air Marvin on 20.12.18.
//  Copyright © 2018 Air Marvin. All rights reserved.
//

#include "robot_project.h"
#include <chrono>


RobotProject::RobotProject(int argc, char *argv[]) {
    int shift = 1;//1;
    //this->source_img_path       = argv[1];
    this->calibration_filepath = argv[2 + shift];
    this->intrinsic_calibration = argv[3 + shift];

    this->mission = std::atol(argv[4 + shift]);
    std::cout << "mission :" << mission << std::endl;
    this->ipm = Inverse_Perspective_Mapping();

    this->map = new Map(DigitRecognitionAlgo::tesseractOCP, 70, 15, 3.0, 0);

    if (argv[5 + shift] == "quick")
        this->map->quickCalculation = true;
}

void RobotProject::start() {
    cv::Mat img = cv::imread(this->source_img_path);
    preprocessMap(img);
    planPath(img, path);
}

bool RobotProject::preprocessMap(cv::Mat const &img) {

    using fsec = std::chrono::duration<float>;
    auto t0 = std::chrono::steady_clock::now();

    //take the image and preprocess
    cv::Mat persp_img, robot_plane;
    // cv::imshow("input", img);
    //  cv::waitKey(0);

    persp_img = ipm.run(this->intrinsic_calibration, img, calibration_filepath);
    //detect the robot plane
    //cv::imshow("persp", persp_img);
    //cv::waitKey(1);
    robot_plane = ipm.detectRobotPlane(img);
//    cv::imshow("robot_plane",robot_plane);
//    cv::waitKey(0);
    //important settings
    // - img width and height in pixels
//    Settings::IMG_WIDTH = persp_img.cols;
//    Settings::IMG_LENGHT = persp_img.rows;

    //important Parameters (in constructor of map)
    //  - DigitRecognition min confidence
    //  - consecutive delusions
    //  - main CRA
    //  - min rotation angle
    std::vector<std::string> result = map->findBestFilters({
                                                                

                                                           }, persp_img);


    if (result.empty())
        std::cout << "!!! no filter found !!!" << std::endl;
    else {
        std::cout << "best filter was ->" << result[0] << std::endl;

        for (int i = 1; i < result.size(); i++)
            std::cout << i + 1 << ".best filter was ->" << result[i] << std::endl;
    }


    if (!result.empty())
        map->setFilterPathE(result[0]);
    else {
        //map->setFilterPathE("data/calib/filter_3.png");
        map->setFilterPathE("data/calib/filter_11.png");
    }

    map->createMap(persp_img, robot_plane);
    map->save("savedMap.json");

    auto t1 = std::chrono::steady_clock::now();
    fsec delta = t1 - t0;
    std::cout << "TOT TIME : " << delta.count() << "s\n";

    cv::imwrite("../data/exam_dataset/img/testsave.jpg", persp_img);
    std::cout << map->wasSuccess() << std::endl;
    return 1;//map->wasSuccess();
}

bool RobotProject::planPath(cv::Mat const &img, ApiPath &path) {


    MissionPlanning m = MissionPlanning(map);
    //Visualizer v(*m.map_p, m.path_p);
    //qv.visualize();
    switch (mission) {
        case 1:
            m.plan_mission_one();
            break;
        case 2:
            m.plan_mission_two();
            break;
        case 3:
            m.plan_mission_two_fast();
            break;
    }
    std::vector<Pose> pose;
    std::cout << "init pose" << std::endl;
    //initialization of pose vector
    double int_point_counter = 0;
    int points_number = 0;
    for (int i = 0; i < m.path_p->getLines().size(); i++) {
        std::cerr << "line size" << m.path_p->getLines().size() << std::endl;
        std::cerr << "int size size" << m.path_p->getLines()[i].getIntermediatePoints().size() << std::endl;

        points_number += m.path_p->lines[i].getIntermediatePoints().size();
    }

    std::cout << "points number" << std::endl;

    for (int i = 0; i < m.path_p->lines.size(); i++) {
        std::vector<cv::Point2d> intermediate_points = m.path_p->lines[i].getIntermediatePoints();
        for (int j = 0; j < intermediate_points.size(); j++) {
            //int_point_counter*5*Setting::PIXEL_SCALE i the distance from the starting point

            cv::Point endpoint = cv::Point(0, 0);
            cv::Point start_p = cv::Point(0, 0);
            if (j < intermediate_points.size() - 2) {
                start_p = intermediate_points[j];
                endpoint = intermediate_points[j + 1];
            } else {
                start_p = intermediate_points[j-1];
                endpoint = intermediate_points[j];
            }
            if (start_p != endpoint){
                 map->robo->angle = Geometry::angle_rad(start_p, endpoint);
            }

            std::pair<double, double> mm_point = Geometry::convertPixelToMillimeterInMapPlane(intermediate_points[j],
                                                                                              map->getStartPoint(),
                                                                                              map->robo->map_pixelscale);
            std::pair<double, double> fin_length = Geometry::convertPixelToMillimeterInMapPlane(
                    cv::Point2d(m.path_p->length, m.path_p->length),
                    cv::Point2d(0, 0),
                    map->robo->map_pixelscale);

            pose.push_back(Pose(
                    (int_point_counter / points_number) * (fin_length.first / 1000), mm_point.second / 1000.0,
                    mm_point.first / 1000.0,
                    map->robo->angle * -1 + M_PI / 2.0,
                    -1 * m.path_p->lines[i].getCurvature() * map->robo->map_pixelscale *
                    1000.0)); //TODO check theta and kappa values and trasform the x and y in meters
            int_point_counter++;

        }
    }

    path.setPoints(pose);
    std::cout << "pose setted" << std::endl;
    return true;
}

bool RobotProject::localize(cv::Mat const &img,
                            std::vector<double> &state) {

    state.clear();
    //USE THIS FUNCTION TO DETECT THE ROBOT USING THE ROBOT PLANE
    //cv::Mat robot_plane = ipm.detectRobotPlane(img);
    //USE THIS FUNCTION TO DETECT THE ROBOT USING THE MAP PLANE
    cv::Mat robot_plane = ipm.detectMapPlane(img);


    //do not reconstruct map because too expensive
    //find robot shape
    //calculate COM
    //calculate orientation
    //Robot robo;
    bool result = map->robo->findRobot(robot_plane);

    cv::Point start = map->getStartPoint();

    //OLD WAY
    //cv::Point2d coordinates = map->robo->getPosition2dRobotFrame(start);
    //NEW WAY we are simply picking the center of the robot and using the pixel scale value of the map to do the conversion
    //TODO ask to marvin if the conversion is right
    //cv::Point2d coordinates  = cv::Point2d(map->robo->center_wheel.x/map->robo->map_pixelscale,map->robo->center_wheel.y/map->robo->map_pixelscale);
    //cv::Point2d coordinates  = cv::Point2d(map->robo->center.x/map->robo->map_pixelscale,map->robo->center.y/map->robo->map_pixelscale);
    std::pair<double, double> coordinates = Geometry::convertPixelToMillimeterInMapPlane(map->robo->center,
                                                                                         map->getStartPoint(),
                                                                                         map->robo->map_pixelscale);
    /*std::pair<double, double> coordinates = Geometry::convertPixelToMillimeterInMapPlane(map->robo->center_wheel,
                                                                                              map->getStartPoint(),
                                                                                              map->robo->map_pixelscale);*/

    double x = coordinates.first / 1000.0;
    //MIRROR X
    //x = 1-x;
    double y = coordinates.second / 1000.0;
    double theta = map->robo->angle * -1 + M_PI / 2.0;
    std::cout << "x :" << x << " y : " << y << " theta : " << theta << std::endl;
    state = {y, x, theta};

    //put info to state

    return result;
}
