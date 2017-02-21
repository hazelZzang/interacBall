#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;


Mat getSkinColorMask(const Mat& img) {
	Mat imgSkin;

	// 3x3
	Mat element(3, 3, CV_8U, cv::Scalar(1));

	//skin color
	cvtColor(img, imgSkin, CV_BGR2YCrCb);
	inRange(imgSkin, Scalar(0, 133, 77), Scalar(255, 173, 127), imgSkin);
	cvtColor(imgSkin, imgSkin, CV_GRAY2RGB);

	// expansion operation
	morphologyEx(img, img, MORPH_CLOSE, element);
	morphologyEx(img, img, MORPH_OPEN, element);

	bitwise_and(imgSkin, img, imgSkin);

	return imgSkin;
}

Point findHand(Mat & img) {
	Mat dist, imgGray;
	int maxIdx[2];
	double radius;
	
	cvtColor(img, imgGray, CV_RGB2GRAY);
	distanceTransform(imgGray, dist, CV_DIST_L2, 5);

	//maximum -> hand's center
	minMaxIdx(dist, NULL, &radius, NULL, maxIdx);
	Point p(maxIdx[1], maxIdx[0]);

	circle(img, p, (int)radius, Scalar(0, 255, 0), 2);
	return p;
}


int main(int argc, char** argv) {
	VideoCapture webCam(0);
	if (!webCam.isOpened()) {
		cout << "webcam open error" << endl;
		return 1;
	}

	webCam.set(CAP_PROP_FRAME_WIDTH, 320);
	webCam.set(CAP_PROP_FRAME_HEIGHT, 240);

	namedWindow("window", 1);
	Mat img, imgGray;
	Mat imgSkin;
	while (1) {
		webCam.read(img);
		cvtColor(img, imgGray, CV_RGB2GRAY);
		
		if (img.empty()) break;
						
		imgSkin = getSkinColorMask(img);
		
		findHand(imgSkin);
		
		/*
		//canny edge
		Canny(imgGray, imgGray, 50, 100);
		*/

		imshow("window", imgSkin);
		if (waitKey(10) == 27) break;
	}
}
