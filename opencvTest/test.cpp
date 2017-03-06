#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

Mat getMask(const Mat& img, Ptr<BackgroundSubtractor> pMOG2) {
	Mat skinMsk; //mask from skin color
	Mat backMsk; //mask from detecting background
	Mat mask; // mask result 
	
	// 5x5
	Mat element(5, 5, CV_8U, cv::Scalar(1));

	//skin color
	cvtColor(img, skinMsk, CV_BGR2YCrCb);
	inRange(skinMsk, Scalar(0, 133, 77), Scalar(255, 173, 127), skinMsk);
	cvtColor(skinMsk, skinMsk, CV_GRAY2RGB);

	// mopology operation
	GaussianBlur(skinMsk, skinMsk, Size(5, 5), 0.5);
	morphologyEx(skinMsk, skinMsk, MORPH_CLOSE, element);
	morphologyEx(skinMsk, skinMsk, MORPH_OPEN, element);
	
	//background subtraction
	pMOG2->apply(img, backMsk);
	GaussianBlur(backMsk, backMsk, Size(5, 5), 0.5);
	morphologyEx(backMsk, backMsk, MORPH_CLOSE, element);
	morphologyEx(backMsk, backMsk, MORPH_OPEN, element);
	cvtColor(backMsk, backMsk, CV_GRAY2RGB);

	/*
	//test
	cout << "skinmsk" << skinMsk.channels() << endl;
	cout << "backmsk" << backMsk.channels() << endl;
	*/

	bitwise_and(backMsk, skinMsk, mask);
	return mask;
}

Point findHand(Mat & img) {
	Mat dist;
	int maxIdx[2];
	int imgSize = (int)img.rows * (int)img.cols;
	double radius;
	cvtColor(img, img, CV_RGB2GRAY);
	distanceTransform(img, dist, CV_DIST_L2, 5);

	threshold(img, img, 2, 255, THRESH_BINARY);

	//maximum -> hand's center
	minMaxIdx(dist, NULL, &radius, NULL, maxIdx);
	Point p(maxIdx[1], maxIdx[0]);

	// 5x5
	Mat element(5, 5, CV_8U, cv::Scalar(1));
	
	// mopology operation
	GaussianBlur(img, img, Size(5, 5), 0.5);
	morphologyEx(img, img, MORPH_CLOSE, element);
	morphologyEx(img, img, MORPH_OPEN, element);
	morphologyEx(img, img, MORPH_OPEN, element);

	// labeling
	Mat img_labels, stats, centroids;
	int slabel;

	int numOfLables = connectedComponentsWithStats(img, img_labels,
		stats, centroids, 8, CV_32S);
	for (int j = 1; j < numOfLables; j++) {
		int area = stats.at<int>(j, CC_STAT_AREA);
		int left = stats.at<int>(j, CC_STAT_LEFT);
		int top = stats.at<int>(j, CC_STAT_TOP);
		int width = stats.at<int>(j, CC_STAT_WIDTH);
		int height = stats.at<int>(j, CC_STAT_HEIGHT);
		
		if (area < imgSize * 0.05 ) continue;
		rectangle(img, Point(left, top), Point(left + width, top + height),
			255, 4);
	}
	
	cout << numOfLables << endl;

	circle(img, p, (int)radius, Scalar(0, 255, 0), 2);
	imshow("window", img);
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
	Mat msk, result;
	Ptr<BackgroundSubtractor> pMOG2;
	pMOG2 = createBackgroundSubtractorMOG2();

	while (1) {
		webCam.read(img);
		cvtColor(img, imgGray, CV_RGB2GRAY);

		if (img.empty()) break;

		msk = getMask(img, pMOG2);
		bitwise_and(img, msk, result);
		
		findHand(msk);
	
		/*canny edge
		//Canny(msk, result, 50, 200);
		*/

		//imshow("window", result);
		if (waitKey(10) == 27) break;
	}
}
