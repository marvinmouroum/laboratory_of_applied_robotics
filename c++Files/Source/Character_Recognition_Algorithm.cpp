//
//  Character_Recognition_Algorithm.cpp
//  LAR_1_0
//
//  Created by Air Marvin on 10.10.18.
//  Copyright © 2018 Air Marvin. All rights reserved.
//

#include "Character_Recognition_Algorithm.hpp"

cv::Mat Character_Recognition_Algorithm::loadImage(const std::string& filename){
    // Load image from file
    cv::Mat img = cv::imread(filename.c_str());
    if(img.empty()) {
        throw std::runtime_error("Failed to open the file " + filename);
    }
    return img;
}

void Character_Recognition_Algorithm::displayImage(cv::Mat & image,std::string windowTitle){
    cv::imshow(windowTitle, image);
}

void Character_Recognition_Algorithm::convert_bgr_to_hsv(cv::Mat &original, cv::Mat &converted){
    // Convert color space from BGR to HSV
    cv::cvtColor(original, converted, cv::COLOR_BGR2HSV);
}

void Character_Recognition_Algorithm::apply_mask(cv::Mat &original, cv::Mat &converted, cv::Scalar lowerbound, cv::Scalar upperbound){
    cv::inRange(original,lowerbound,upperbound, converted);
}

cv::Mat Character_Recognition_Algorithm::apply_some_filtering(cv::Mat &img){
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size((1*2) + 1, (1*2)+1));
    cv::dilate(img, img, kernel);
    cv::erode(img, img, kernel);
    return kernel;
}

std::vector<cv::Rect> Character_Recognition_Algorithm::extract_regions_of_interest(cv::Mat &original_img,cv::Mat & filtered_img,cv::Mat &returnedImg){
    
    std::vector<std::vector<cv::Point>> contours, contours_approx;
    std::vector<cv::Point> approx_curve;
    
    returnedImg = original_img.clone();
    cv::findContours(filtered_img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    std::vector<cv::Rect> boundRect(contours.size());
    for (int i=0; i<contours.size(); ++i)
    {
        double area = cv::contourArea(contours[i]);
        if (area < this->MIN_AREA_SIZE) continue; // filter too small contours to remove false positives
        approxPolyDP(contours[i], approx_curve, 2, true);
        contours_approx = {approx_curve};
        drawContours(returnedImg, contours_approx, -1, cv::Scalar(0,170,220), 3, cv::LINE_AA);
        boundRect[i] = boundingRect(cv::Mat(approx_curve)); // find bounding box for each green blob
    }
    
    return boundRect;
}

std::tuple<cv::Mat,cv::Mat> Character_Recognition_Algorithm::invert_masked_image(cv::Mat &original, cv::Mat &masked_image){
    
    cv::Mat mask_inv, filtered(original.rows, original.cols, CV_8UC3, cv::Scalar(255,255,255));
    cv::bitwise_not(masked_image, mask_inv); // generate binary mask with inverted pixels w.r.t. green mask -> black numbers are part of this mask
    original.copyTo(filtered, mask_inv);   // create copy of image without green shapes
    return std::tuple<cv::Mat,cv::Mat>(mask_inv, filtered);
}

void Character_Recognition_Algorithm::rotate_image(cv::Mat &src, double angle, cv::Mat &result){
    
    // get rotation matrix for rotating the image around its center in pixel coordinates
    cv::Point2f center((src.cols-1)/2.0, (src.rows-1)/2.0);
    cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
    // determine bounding rectangle, center not relevant
    cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), src.size(), angle).boundingRect2f();
    // adjust transformation matrix
    rot.at<double>(0,2) += bbox.width/2.0 - src.cols/2.0;
    rot.at<double>(1,2) += bbox.height/2.0 - src.rows/2.0;
    
    cv::warpAffine(src, result, rot, bbox.size(),cv::INTER_LINEAR,
                   cv::BORDER_CONSTANT,
                   cv::Scalar(255, 255, 255));
}

void Character_Recognition_Algorithm::preprocessing(cv::Mat &img, cv::Mat &filtered, std::vector<cv::Rect> &boundRect){
    
    // Display original image
    displayImage(img, "Original");
    //cv::waitKey(0);
    
    // Convert color space from BGR to HSV
    cv::Mat hsv_img;
    convert_bgr_to_hsv(img, hsv_img);
    //displayImage(hsv_img, "hsv");
    //cv::waitKey(0);
    
    // Find green regions
    cv::Mat green_mask;
    apply_mask(hsv_img, green_mask, cv::Scalar(65, 30, 80), cv::Scalar(75, 255, 255));
    //displayImage(green_mask, "GREEN_filter1");
    //cv::waitKey(0);
    
    // Apply some filtering
    cv::Mat kernel = apply_some_filtering(green_mask);
    
    // Find contours
    cv::Mat contours_img;
    boundRect = extract_regions_of_interest(img, green_mask,contours_img);
    
    //displaying
    //displayImage(contours_img, "Original");
    //cv::waitKey(0);
    
    //invert the pixels black white
    std::tuple<cv::Mat,cv::Mat> inversionResult = invert_masked_image(img, green_mask);
    cv::Mat green_mask_inv  = std::get<0>(inversionResult); //only for displaying purposes
    filtered        = std::get<1>(inversionResult); // needed to detect digit
    
    //displaying some more
    //displayImage(green_mask_inv, "Numbers");
    //cv::waitKey(0);
    
}