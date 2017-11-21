//This program is copied from Open CV Image Smoothing Page

#include <iostream>
#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp"
#include "glut.h"

using namespace std;
using namespace cv;

Mat limage, rimage;		//OpenCV images
int lwidth, lheight;	//dimensions of left window
int rwidth, rheight;	//dimensions of right window
int lwin = 0, rwin = 0;	//;eft and right window ids
uchar *image1, *image2;	//OpenGL original images
uchar *image1e, *image2e;//OpenGL edge images
int nr1, nc1, nr2, nc2;	//dimensions of OpenGL left and right images
float zoom1=1.0f, zoom2=1.0f;//zoom factors of left and right images when displayed
int isedgeimage = 0;

void initMenu(void);

//convert OpenCV image to OpenGL img
void getImageData(Mat image, uchar *img, int nr, int nc)
{
	Vec3b color;

	for(int i=0; i<nr; i++)
		for (int j = 0; j < nc; j++) {
			color = image.at<Vec3b>(nr-1-i, j);
			img[3 * (i*nc + j)] = color.val[2];
			img[3 * (i*nc + j)+1] = color.val[1];
			img[3 * (i*nc + j)+2] = color.val[0];
		}
}

//reshape function for left window
void lreshape(int w, int h) 
{
	float zoomx, zoomy;

	zoomx = (float)w / (float)nc1;
	zoomy = (float)h / (float)nr1;
	if (zoomx>zoomy)zoom1 = zoomy; else zoom1 = zoomx;
	glPixelZoom(zoom1, zoom1);

	w = (int)(zoom1*(float)nc1);
	h = (int)(zoom1*(float)nr1);
	lwidth = w; lheight = h;
	glutReshapeWindow(lwidth, lheight);
	glViewport(0, 0, (GLsizei)lwidth, (GLsizei)lheight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)nc1, 0.0, (GLdouble)nr1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//reshape function for right window
void rreshape(int w, int h) 
{
	float zoomx, zoomy;

	zoomx = (float)w / (float)nc2;
	zoomy = (float)h / (float)nr2;
	if (zoomx>zoomy)zoom2 = zoomy; else zoom2 = zoomx;
	glPixelZoom(zoom2, zoom2);

	w = (int)(zoom2*(float)nc2);
	h = (int)(zoom2*(float)nr2);
	rwidth = w; rheight = h;
	glutReshapeWindow(rwidth, rheight);
	glViewport(0, 0, (GLsizei)rwidth, (GLsizei)rheight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)nc2, 0.0, (GLdouble)nr2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//display function for left image
void ldisplay(void)
{
	glRasterPos2i(0, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if(isedgeimage)
		glDrawPixels((GLsizei)nc1, (GLsizei)nr1, GL_RGB, GL_UNSIGNED_BYTE, image1e);
	else
		glDrawPixels((GLsizei)nc1, (GLsizei)nr1, GL_RGB, GL_UNSIGNED_BYTE, image1);
	glutSwapBuffers();
}

//display function for right image
void rdisplay(void)
{
	glRasterPos2i(0, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if(isedgeimage)
		glDrawPixels((GLsizei)nc2, (GLsizei)nr2, GL_RGB, GL_UNSIGNED_BYTE, image2e);
	else 
		glDrawPixels((GLsizei)nc2, (GLsizei)nr2, GL_RGB, GL_UNSIGNED_BYTE, image2);
	glutSwapBuffers();
}

//mouse function for left window
//note that y shows row number, increasing from top to bottom
//while image1 rows increase bottom to top
void lmouse(int button, int state, int x, int y) 
{
	if (zoom1 == 1.0f && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		cout << "RGB at (col:" << x << ", row:" << nr1 - 1 - y << ") is (" << (int)image1[3 * ((nr1 - 1 - y)*nc1 + x)]<<", "<< (int)image1[3 * ((nr1 - 1 - y)*nc1 + x) + 1] << ", " << (int)image1[3 * ((nr1 - 1 - y)*nc1 + x)+2] << ")." << endl;
	} 
	//include code so that if a point is selected in left image, the corresponding
	//epipolar line is drawn in right image.
}

//mouse function for right window
//note that y shows row number, increasing from top to bottom
//while image2 rows increase bottom to top
void rmouse(int button, int state, int x, int y) 
{
	if (zoom2 == 1.0f && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		cout << "RGB at (col:" << x << ", row: " << nr2 - 1 - y << ") is (" << (int)image2[3*((nr2-1-y)*nc2+x)] << ", " <<(int)image2[3 * ((nr2 - 1 - y)*nc2 + x) + 1] << ", " << (int)image2[3 * ((nr2 - 1 - y)*nc2 + x)+2] << ")." << endl;
	} 
	//include code so that if a point is selected in right image, the corresponding
	//epipolar line is drawn in left image.
}


void menu(int cmnd)
{
	switch (cmnd) {
	case 1:	//display original images
		isedgeimage = 0;
		glutSetWindow(lwin);
		initMenu();
		glutPostRedisplay();
		glutSetWindow(rwin);
		initMenu();
		glutPostRedisplay();
		break;
	case 2:	//display edge images
		isedgeimage = 1;
		glutSetWindow(lwin);
		initMenu();
		glutPostRedisplay();
		glutSetWindow(rwin);
		initMenu();
		glutPostRedisplay();
		break;
	case 3://include code to replace left image (image1) with disparity-map of scene
		   //map the disparity values to 0-255. Note that each image is color. 
			//Therefore, R,G, and B at [i,j] in left image should be all set to disparity at [i,j].
		glutSetWindow(lwin);
		glutPostRedisplay();
		break;
	case 9:
		delete[] image1;
		delete[] image2;
		limage.release();
		rimage.release();
		glutDestroyWindow(lwin);
		glutDestroyWindow(rwin);
		exit(0);
	}
}

void initMenu(void)
{
	glutCreateMenu(menu);
	if (isedgeimage) glutAddMenuEntry("Display original images", 1);
	else glutAddMenuEntry("Display edge images", 2);
	glutAddMenuEntry("Find depth map of scene", 3);
	glutAddMenuEntry("Quit", 9);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//main program: read the images in OpenCV,
//convert the images to OpenGL format
//define the windows, and display the images
int main(int argc, char** argv)
{
	char fn[80];
	

	cout << "Enter left image: ";
	cin >> fn;

	// Reading left image
	limage = imread(fn, 1);

	//Checking for invalid input
	while (!limage.data)                              
	{
		cout << "Could not load image." << endl;
		cout << "Enter another image: ";
		cin >> fn;
		limage = imread(fn, 1);
	}
	nc1=lwidth = limage.cols; 
	nr1=lheight = limage.rows;
	cout << "Left image dimensions: " << nc1 << "x" << nr1 << endl;
	image1 = new uchar[nr1*nc1 * 3]();
	getImageData(limage, image1, nr1, nc1);

	cout << "Enter left edge image: ";
	cin >> fn;

	// Reading left image
	limage = imread(fn, 1);

	//Checking for invalid input
	while (!limage.data)
	{
		cout << "Could not load edge image." << endl;
		cout << "Enter another image: ";
		cin >> fn;
		limage = imread(fn, 1);
	}
	cout << "Left edge image dimensions: " << limage.cols << "x" << limage.rows << endl;
	if (nc1 != limage.cols || nr1 != limage.rows) {
		cout << "Edge and original images should have the same dimensions." << endl;
		return 1;
	}
	
	image1e = new uchar[nr1*nc1 * 3]();
	getImageData(limage, image1e, nr1, nc1);

	cout << "Enter right image: ";
	cin >> fn;

	// Reading right image
	rimage = imread(fn, 1);

	//Checking for invalid input
	while (!rimage.data)
	{
		cout << "Could not load image." << endl;
		cout << "Enter another image: ";
		cin >> fn;
		rimage = imread(fn, 1);
	}
	nc2=rwidth = rimage.cols; 
	nr2=rheight = rimage.rows;
	cout << "Right image dimensions: " << nc2 << "x" << nr2 << endl;
	image2 = new uchar[nr2*nc2 * 3]();
	getImageData(rimage, image2, nr2, nc2);

	cout << "Enter right edge image: ";
	cin >> fn;

	// Reading right image
	rimage = imread(fn, 1);

	//Checking for invalid input
	while (!rimage.data)
	{
		cout << "Could not load image." << endl;
		cout << "Enter another image: ";
		cin >> fn;
		rimage = imread(fn, 1);
	}

	cout << "Right image dimensions: " << rimage.cols << "x" << rimage.rows << endl;

	if (nc2 != limage.cols || nr2 != limage.rows) {
		cout << "Edge and original images should have the same dimensions." << endl;
		return 1;
	}

	image2e = new uchar[nr2*nc2 * 3]();
	getImageData(rimage, image2e, nr2, nc2);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(lwidth, lheight);
	glutInitWindowPosition(0, 0);

	lwin = glutCreateWindow("Left image");
	initMenu();
	glutDisplayFunc(ldisplay);
	glutReshapeFunc(lreshape);
	glutMouseFunc(lmouse);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(rwidth, rheight);
	glutInitWindowPosition(nc1+16,0);

	rwin = glutCreateWindow("Right image");
	initMenu();
	glutDisplayFunc(rdisplay);
	glutReshapeFunc(rreshape);
	glutMouseFunc(rmouse);

	glutMainLoop();

	return 0;
}

