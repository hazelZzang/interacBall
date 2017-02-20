#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	VideoCapture webCam(0);
	if (!webCam.isOpened()) {
		cout << "��ķ ���� ���� ��Ʈ�� �ѼŻѼ�" << endl;
		return 1;
	}

	webCam.set(CAP_PROP_FRAME_WIDTH, 320);
	webCam.set(CAP_PROP_FRAME_HEIGHT, 240);

	namedWindow("window", 1);
	Mat img, imgGray;
	Mat imgSkin, imgLabel;
	Mat stats, centroids;
	Mat imgHand;
	while (1) {
		webCam.read(img);
		cvtColor(img, imgGray, CV_RGB2GRAY);
		
		if (img.empty()) break;
		
		/*
		//ĳ�Ͽ�������
		Canny(imgGray, imgGray, 20, 50);
		*/
		
		/*
		//�󺧸�
		
		int c = connectedComponentsWithStats(imgGray, imgLabel, stats, centroids);
		for (int y = 0; y<imgLabel.rows; ++y) {

			int *label = imgLabel.ptr<int>(y);
			Vec3b* pixel = imgGray.ptr<Vec3b>(y);

			for (int x = 0; x < imgLabel.cols; ++x) {
				if (label[x] == 0) {
					pixel[x][2] = 0;
					pixel[x][1] = 255;
					pixel[x][0] = 0;
				}
			}
		}
		*/

		//��
		cvtColor(img, imgSkin, CV_BGR2YCrCb);
		inRange(imgSkin, Scalar(0, 133, 77), Scalar(255, 173, 127), imgSkin);
		cvtColor(imgSkin, imgSkin, CV_GRAY2RGB);
		

		// ���� ȿ���� �� �ε巯���� 3x3 ���� ��Ҹ� ���
		Mat element5(3, 3, CV_8U, cv::Scalar(1));

		// ���� ������ ���� ���� (����, ���� ������)
		morphologyEx(img, img, MORPH_CLOSE, element5);
		morphologyEx(img, img, MORPH_OPEN, element5);

		bitwise_and(imgSkin, img, imgSkin);

		imshow("window", imgSkin);
		if (waitKey(10) == 27) break;
	}


}