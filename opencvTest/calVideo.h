#pragma once

#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


Mat getMask(const Mat& img, Ptr<BackgroundSubtractor> pMOG2);
Mat findHand(Mat & img, Mat &);