// camera_calibration.cpp:
// Tool to find the camera matrix and distortion coefficients from a set of
// images containing a chessboard pattern.
// Taken from: samples/cpp/tutorial_code/calib3d/camera_calibration
// Takes as input a configuration file (see calib_config.xml)

#include "Settings.hpp"
#include "Calibration_Intrinsic.hpp"

using namespace cv;
using namespace std;

static void help()
{
    cout <<  "This is a camera calibration sample." << endl
         <<  "Usage: camera_calibration [configuration_file -- default ./default.xml]"  << endl
         <<  "Near the sample file you'll find the configuration file, which has detailed help of "
             "how to edit it.  It may be any OpenCV supported file format XML/YAML." << endl;
}


//static inline void read(const FileNode& node, Settings& x, const Settings& default_value = Settings())
//{
//    if(node.empty())
//        x = default_value;
//    else
//        x.read(node);
//}

//int main(int argc, char* argv[])
//{
//    //help();
//
//    //! [file_read]
//    Settings s;
//    const string inputSettingsFile = argc > 1 ? argv[1] : "default.xml";
//    FileStorage fs(inputSettingsFile, FileStorage::READ); // Read the settings
//    if (!fs.isOpened())
//    {
//        cout << "Could not open the configuration file: \"" << inputSettingsFile << "\"" << endl;
//        return -1;
//    }
//    fs["Settings"] >> s;
//    fs.release();                                         // close Settings file
//    //! [file_read]
//
//    //FileStorage fout("settings.yml", FileStorage::WRITE); // write config as YAML
//    //fout << "Settings" << s;
//
//    if (!s.goodInput)
//    {
//        cout << "Invalid input detected. Application stopping. " << endl;
//        return -1;
//    }
//
//    vector<vector<Point2f> > imagePoints;
//    Mat cameraMatrix, distCoeffs;
//    Size imageSize;
//    int mode = s.inputType == Settings::IMAGE_LIST ? CAPTURING : DETECTION;
//    clock_t prevTimestamp = 0;
//    const Scalar RED(0,0,255), GREEN(0,255,0);
//    const char ESC_KEY = 27;
//
//    //! [get_input]
//    for(;;)
//    {
//        Mat view;
//        bool blinkOutput = false;
//
//        view = s.nextImage();
//
//        //-----  If no more image, or got enough, then stop calibration and show result -------------
//        if( mode == CAPTURING && imagePoints.size() >= (size_t)s.nrFrames )
//        {
//          if( Calibration_Instrinsic::runCalibrationAndSave(s, imageSize,  cameraMatrix, distCoeffs, imagePoints))
//              mode = CALIBRATED;
//          else
//              mode = DETECTION;
//        }
//        if(view.empty())          // If there are no more images stop the loop
//        {
//            // if calibration threshold was not reached yet, calibrate now
//            if( mode != CALIBRATED && !imagePoints.empty() )
//                Calibration_Instrinsic::runCalibrationAndSave(s, imageSize,  cameraMatrix, distCoeffs, imagePoints);
//            break;
//        }
//        //! [get_input]
//
//        imageSize = view.size();  // Format input image.
//        if( s.flipVertical )    flip( view, view, 0 );
//
//        //! [find_pattern]
//        vector<Point2f> pointBuf;
//
//        bool found;
//
//        int chessBoardFlags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE;
//
//        if(!s.useFisheye) {
//            // fast check erroneously fails with high distortions like fisheye
//            chessBoardFlags |= CALIB_CB_FAST_CHECK;
//        }
//
//        switch( s.calibrationPattern ) // Find feature points on the input format
//        {
//        case Settings::CHESSBOARD:
//            found = findChessboardCorners( view, s.boardSize, pointBuf, chessBoardFlags);
//            break;
//        case Settings::CIRCLES_GRID:
//            found = findCirclesGrid( view, s.boardSize, pointBuf );
//            break;
//        case Settings::ASYMMETRIC_CIRCLES_GRID:
//            found = findCirclesGrid( view, s.boardSize, pointBuf, CALIB_CB_ASYMMETRIC_GRID );
//            break;
//        default:
//            found = false;
//            break;
//        }
//        //! [find_pattern]
//        //! [pattern_found]
//        if ( found)                // If done with success,
//        {
//              // improve the found corners' coordinate accuracy for chessboard
//                if( s.calibrationPattern == Settings::CHESSBOARD)
//                {
//                    Mat viewGray;
//                    cvtColor(view, viewGray, COLOR_BGR2GRAY);
//                    cornerSubPix( viewGray, pointBuf, Size(11,11),
//                        Size(-1,-1), TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.1 ));
//                }
//
//                if( mode == CAPTURING &&  // For camera only take new samples after delay time
//                    (!s.inputCapture.isOpened() || clock() - prevTimestamp > s.delay*1e-3*CLOCKS_PER_SEC) )
//                {
//                    imagePoints.push_back(pointBuf);
//                    prevTimestamp = clock();
//                    blinkOutput = s.inputCapture.isOpened();
//                }
//
//                // Draw the corners.
//                drawChessboardCorners( view, s.boardSize, Mat(pointBuf), found );
//        }
//        //! [pattern_found]
//        //----------------------------- Output Text ------------------------------------------------
//        //! [output_text]
//        string msg = (mode == CAPTURING) ? "100/100" :
//                      mode == CALIBRATED ? "Calibrated" : "Press 'g' to start";
//        int baseLine = 0;
//        Size textSize = getTextSize(msg, 1, 1, 1, &baseLine);
//        Point textOrigin(view.cols - 2*textSize.width - 10, view.rows - 2*baseLine - 10);
//
//        if( mode == CAPTURING )
//        {
//            if(s.showUndistorsed)
//                msg = format( "%d/%d Undist", (int)imagePoints.size(), s.nrFrames );
//            else
//                msg = format( "%d/%d", (int)imagePoints.size(), s.nrFrames );
//        }
//
//        putText( view, msg, textOrigin, 1, 1, mode == CALIBRATED ?  GREEN : RED);
//
//        if( blinkOutput )
//            bitwise_not(view, view);
//        //! [output_text]
//        //------------------------- Video capture  output  undistorted ------------------------------
//        //! [output_undistorted]
//        if( mode == CALIBRATED && s.showUndistorsed )
//        {
//            Mat temp = view.clone();
//            if (s.useFisheye)
//              cv::fisheye::undistortImage(temp, view, cameraMatrix, distCoeffs);
//            else
//              undistort(temp, view, cameraMatrix, distCoeffs);
//        }
//        //! [output_undistorted]
//        //------------------------------ Show image and check for input commands -------------------
//        //! [await_input]
//        imshow("Image View", view);
//        char key = (char)waitKey(s.inputCapture.isOpened() ? 50 : s.delay);
//
//        if( key  == ESC_KEY )
//            break;
//
//        if( key == 'u' && mode == CALIBRATED )
//           s.showUndistorsed = !s.showUndistorsed;
//
//        if( s.inputCapture.isOpened() && key == 'g' )
//        {
//            mode = CAPTURING;
//            imagePoints.clear();
//        }
//        //! [await_input]
//    }
//
//    // -----------------------Show the undistorted image for the image list ------------------------
//    //! [show_results]
//    if( s.inputType == Settings::IMAGE_LIST && s.showUndistorsed )
//    {
//        Mat view, rview, map1, map2;
//
//        if (s.useFisheye)
//        {
//            Mat newCamMat;
//            fisheye::estimateNewCameraMatrixForUndistortRectify(cameraMatrix, distCoeffs, imageSize,
//                                                                Matx33d::eye(), newCamMat, 1);
//            fisheye::initUndistortRectifyMap(cameraMatrix, distCoeffs, Matx33d::eye(), newCamMat, imageSize,
//                                             CV_16SC2, map1, map2);
//        }
//        else
//        {
//            initUndistortRectifyMap(
//                cameraMatrix, distCoeffs, Mat(),
//                getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0), imageSize,
//                CV_16SC2, map1, map2);
//        }
//
//        for(size_t i = 0; i < s.imageList.size(); i++ )
//        {
//            view = imread(s.imageList[i], IMREAD_COLOR);
//            if(view.empty())
//                continue;
//            remap(view, rview, map1, map2, INTER_LINEAR);
//            imshow("Image View", rview);
//            char c = (char)waitKey();
//            if( c  == ESC_KEY || c == 'q' || c == 'Q' )
//                break;
//        }
//    }
//    //! [show_results]
//
//    return 0;
//}
