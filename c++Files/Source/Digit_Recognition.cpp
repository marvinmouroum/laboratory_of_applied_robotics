//
//  Digit_Recognition.cpp
//  LAR_1_0
//
//  Created by Air Marvin on 09.10.18.
//  Copyright © 2018 Air Marvin. All rights reserved.
//

#include "../Headers/Digit_Recognition.hpp"

Digit_Recognition::Digit_Recognition(){
    std::cout << "basic digit recognition constructor selected" << std::endl;
    
    //initiate variables
    set_algo(DigitRecognitionAlgo::tesseractOCP);
}

Digit_Recognition::Digit_Recognition(DigitRecognitionAlgo algorithm){
    std::cout << "specific digit recognition constructor selected" << std::endl;
    
    //initiate variables
    set_algo(algorithm);
}

void Digit_Recognition::run_demo(const std::string filename){
    this->algortihm->processImage(filename);
}

///initialized the algorithm variable with the picked algorithm
void Digit_Recognition::initialize_algorithm(){
    
    
    HSVFilterRange filter = HSVFilterRange("bad");
    if(this->algortihm != nullptr){
        filter = this->algortihm->filter;
    }
    
    switch (picked_algorithm) {
    case templateMatching:
            std::cout << "open cv algorithm is beeing used" << std::endl;
            this->algortihm = new Template_Character_Recognition;
            set_filter(filter);
            break;
        case tesseractOCP:
            std::cout << "tesseract ocr algorithm is beeing used" << std::endl;
            this->algortihm = new Optical_Character_Recognition;
            set_filter(filter);
            break;
        default:
            std::cout << "no algorithm could be set" << std::endl;
    }
}

void Digit_Recognition::set_algo(DigitRecognitionAlgo algorithm){
    this->picked_algorithm = algorithm;
    initialize_algorithm();
}

void Digit_Recognition::set_filter(HSVFilterRange filterRange){
    this->algortihm->filter = filterRange;
}

int Digit_Recognition::detect_single_digit(cv::Mat &img){

    //run the image detection of the regions
    std::vector<int> results = detect_digits(img);
    //jump if no result
    if(results.empty()) return -99;
    
    return results[0];
}

std::vector<int> Digit_Recognition::detect_digits(cv::Mat &img){
    cv::Mat filtered;
    std::vector<cv::Rect> boundRect;
    //apply filters
    //extract regions of interest
    this->algortihm->preprocessing(img, filtered, boundRect);

    //run the image detection of the regions
    std::vector<std::pair<int,cv::Rect>> results = this->algortihm->detection_algorithm(boundRect, filtered);
    //some insides
    std::cout << "detected " << results.size() << " digits" << std::endl;
    
    //collect the integers
    std::vector<int> res;
    
    for(int i = 0;i<results.size();i++)
        res.push_back(results[i].first);
    
    return res;
}

std::vector<PeopleData> Digit_Recognition::detect_peopleData(cv::Mat &img){
    cv::Mat filtered;
    std::vector<cv::Rect> boundRect;
    
    std::vector<PeopleData> data;
    //apply filters
    //extract regions of interest
    this->algortihm->preprocessing(img, filtered, boundRect);
    
    //some insides
    std::cout << "detected " << boundRect.size() << " circles" << std::endl;
    
    //collect the data
    std::vector<PeopleData> res;
    
    for(int i = 0;i<boundRect.size();i++)
        res.push_back(PeopleData(0,boundRect[i]));
    
    cv::imshow("proprocessing", filtered);
   // cv::waitKey(0);
    
    //run the image detection of the regions
    std::vector<std::pair<int,cv::Rect>> results = this->algortihm->detection_algorithm(boundRect, filtered);
    //some insides
    std::cout << "detected " << results.size() << " digits" << std::endl;
    
    for(int i = 0;i<boundRect.size();i++)
        for(int j = 0;j<results.size();j++)
            if(boundRect[i] == results[j].second)
                res[j].digit = results[j].first;
        
    
    return data;
}

std::vector<cv::Rect> Digit_Recognition::get_regions_of_interest(cv::Mat &img){
    
    cv::Mat fil;
    std::vector<cv::Rect> bR;
    
    //do the preprocessing to apply filters and extract filtered regions
    this->algortihm->preprocessing(img, fil, bR);
    
    std::string name = "filtered_" + std::to_string(this->algortihm->filter.lb[0]) + "_" + std::to_string(this->algortihm->filter.lb[1]) + "_" + std::to_string(this->algortihm->filter.lb[2]);
    
    cv::imshow("input_", fil);
    
    std::vector<cv::Rect> results;
    
    //return all non empty rects
    for(int i = 0; i<bR.size();i++)
        if(!bR[i].empty())
            results.push_back(bR[i]);
    
    return results;
}

std::vector<PeopleData> Digit_Recognition::detect_digits_for_map(const cv::Mat img_input){
    
    //*** new Marvin
    
    cv::Mat filtered;
    std::vector<cv::Rect> rects;
    
    cv::Mat source = img_input.clone();
    
    std::vector<cv::Mat> digit_images = this->algortihm->preprocessing(source, filtered, rects);
    
    for(int i = 0;i<digit_images.size();i++){
        cv::imshow("digit_"+std::to_string(i), digit_images[i]);
        this->algortihm->prepare_uniform_window(digit_images[i]);
        
        std::cout << this->algortihm->detect_digit(digit_images[i]) << std::endl;
        cv::waitKey(0);
    }
    
    cv::waitKey(0);
    
    std::vector<PeopleData> results;
    
    //*** old Marvin
//    cv::Mat img = img_input;
//    std::vector<cv::Rect> rects = get_regions_of_interest(img);
//
//    std::vector<PeopleData> results;
//
//    bool one = false;
//    bool two = false;
//    bool three = false;
//    bool four = false;
//
//    std::cout << "detected " << rects.size() << " images" << std::endl;
//
//    for(int i = 0; i<rects.size();i++){
//
//        //crop the image to get the part with digit
//        cv::Mat newimg(img, rects[i]);
//
//        //detect the digit
//        int result = detect_digit_for_map(newimg);
//
//        //check if the result makes sence
//        if(is_valid(result)){
//            //save the result
//            results.push_back(PeopleData(result,rects[i]));
//            std::cout << "got a result of " << result << std::endl;
//            if(result == 1)
//                one = true;
//            else if(result == 2)
//                two = true;
//            else if(result == 3)
//                three = true;
//            else if(result == 4)
//                four = true;
//        }
//        else {
//            results.push_back(PeopleData(0,rects[i]));
//        }
//    }
//
//    if(!one){
//        std::cout << "ONE is missing" << std::endl;
//    }
//    if(!two){
//        std::cout << "TWO is missing" << std::endl;
//    }
//    if(!three){
//        std::cout << "THREE is missing" << std::endl;
//    }
//    if(!four){
//        std::cout << "FOUR is missing" << std::endl;
//    }
//   // cv::waitKey(0);
    
    return results;
}

bool Digit_Recognition::is_valid(int &detectedDigit){
    switch (detectedDigit) {
        case 1:
            return true;
        case 2:
            return true;
        case 3:
            return true;
        case 4:
            return true;
    
        default:
            return false;
    }
}

int Digit_Recognition::detect_digit_for_map(cv::Mat &img){
    
    //reset the filter
    set_filter(HSVFilterRange(this->algortihm->filter.saved_quality));
    
    //reset the algo
    set_algo(tesseractOCP);
    
    //use the tesseract detection algorithm
    std::vector<int> digits = detect_digits(img);
    
    //when no result has been archieved
    if(digits.empty()){
        //change the filter
//        std::string saved = this->algortihm->filter.saved_quality;
//        HSVFilterRange range("bad");
//        range.saved_quality = saved;
//        set_filter(range);
        //try again
        digits = detect_digits(img);
        //try the template matching
        if(digits.empty()){
            //reset filter
//            HSVFilterRange filter = HSVFilterRange(this->algortihm->filter.saved_quality);
            set_algo(templateMatching);
//            set_filter(filter);
            digits = detect_digits(img);
        }
    }
    
    //several numbers should not have been detected
    if(digits.size() > 1){
        std::runtime_error("too many digits in given image - use this function only for detecting one digit at a time !");
    }
    
    if(digits.empty()){
        std::cout << "was not able to detect a digit" << std::endl;
        return -1;
    }
    
    return digits[0];
}
