#include "GL/freeglut.h"
#include "GL/gl.h"
#include "opencv2/opencv.hpp" 
#include "calVideo.h"
#include <iostream>  
#include <string> 

using namespace cv;
using namespace std;

VideoCapture webCam(0);

Mat imgCam;
int screenW;
int screenH;

GLuint texture_background;
Ptr<BackgroundSubtractor> pMOG2 = createBackgroundSubtractorMOG2();

//convert Mat to texture
GLuint MatToTexture(Mat image)
{
	if (image.empty())  return -1;

	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows,
		0, GL_RGB, GL_UNSIGNED_BYTE, image.ptr());

	return textureID;
}


void draw_background()
{
	int x = screenW/100.0;
	int y = screenH/100.0;

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0); glVertex3f(-x, -y, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(x, -y, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(x, y, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-x, y, 0.0);
	glEnd();
}

void cameraInit()
{
	if (!webCam.isOpened()) {
		cout << "webcam open error" << endl;
		exit(0);
	}

	webCam.set(CAP_PROP_FRAME_WIDTH, 320);
	webCam.set(CAP_PROP_FRAME_HEIGHT, 240);

	Mat img_frame;
	webCam.read(img_frame);

	screenW = img_frame.cols;
	screenH = img_frame.rows;
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	texture_background = MatToTexture(imgCam);
	if (texture_background < 0) return;


	glEnable(GL_TEXTURE_2D);

	glColor3f(1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, texture_background);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -9.0);
	draw_background();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -4.0);
	glColor3f(0.0f, 0.0f, 1.0f);
	glutSolidSphere(0.2,20,10);
	glPopMatrix();

	glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45, (GLfloat)width / (GLfloat)height, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

void getWebCam(int value) {
	Mat imgCamOrigin, msk, result;

	if (waitKey(10) == 27) {
		webCam.release();
		exit(0);
	}

	webCam.read(imgCamOrigin);
	if (imgCamOrigin.empty()) {
		cout << "no image" << endl;
		webCam.release();
		exit(0);
	}

	msk = getMask(imgCamOrigin, pMOG2);
	msk = findHand(msk, imgCamOrigin);
	cout << "된다" << endl;

	//bitwise_and(imgCamOrigin, msk, result);
	cout << "된다" << endl;
	//cvtColor(result, imgCam, COLOR_GRAY2RGB);
	cvtColor(msk, imgCam, COLOR_BGR2RGB);


	glutPostRedisplay();
	glutTimerFunc(1, getWebCam, 0);
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	cameraInit();
	glutInitWindowSize(screenW*3, screenH*3);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("window");

	init();

	//callback
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutTimerFunc(0, getWebCam, 0);

	glutMainLoop();
	return 0;
}
