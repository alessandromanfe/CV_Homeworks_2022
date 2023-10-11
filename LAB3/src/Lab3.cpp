#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

struct pData {
	Mat* img;
	int thsld;
	Scalar scl;
};

void mouseCallback1(int ev, int x, int y, int flags, void* param) {
	if (ev == EVENT_LBUTTONDOWN) {
		char buff[30];
		Mat* img = (Mat*)param;
		Vec3b brg = (*img).at<Vec3b>(Point(x, y));
		sprintf(buff, "%d, %d, %d\n", brg[0], brg[1], brg[2]);
		cout << buff;
	}
}

void mouseCallback2(int ev, int x, int y, int flags, void* param) {
	if (ev == EVENT_LBUTTONDOWN) {
		char buff[30];
		Mat* img = (Mat*)param;
		int count = 0;
		Vec3i sum;
		for (int i = max(0,x-4); i < min((*img).cols,x+5); i++) {
			for (int j = max(0,y-4); j < min((*img).rows,y+5); j++) {
				sum += (*img).at<Vec3b>(Point(i, j));
				count++;
			}
		}
		Vec3b brg = sum / count;
		sprintf(buff, "%d, %d, %d\n", brg[0], brg[1], brg[2]);
		cout << buff;
	}
}

void closeCallback(int ev, int x, int y, int flags, void* param) {
	if (ev == EVENT_LBUTTONDOWN) destroyWindow("Mask");
}

void mouseCallback3(int ev, int x, int y, int flags, void* param) {
	if (ev == EVENT_LBUTTONDOWN) {
		struct pData data = *(struct pData*)param;
		Mat* img = data.img;
		int count = 0;
		Vec3i sum;
		for (int i = max(0, x - 4); i < min((*img).cols, x + 5); i++) {
			for (int j = max(0, y - 4); j < min((*img).rows, y + 5); j++) {
				sum += img->at<Vec3b>(Point(i, j));
				count++;
			}
		}
		sum /= count;
		Scalar bd(data.thsld, data.thsld, data.thsld);
		Mat mask = img->clone();
		inRange((*img), Scalar(sum)-bd, Scalar(sum)+bd, mask);
		namedWindow("Mask", WINDOW_AUTOSIZE);
		setMouseCallback("Mask", closeCallback);
		imshow("Mask", mask);

	}
}

void mouseCallback4(int ev, int x, int y, int flags, void* param) {
	if (ev == EVENT_LBUTTONDOWN) {
		struct pData data = *(struct pData*)param;
		Mat* img = data.img;
		int count = 0;
		Vec3i sum;
		for (int i = max(0, x - 4); i < min((*img).cols, x + 5); i++) {
			for (int j = max(0, y - 4); j < min((*img).rows, y + 5); j++) {
				sum += img->at<Vec3b>(Point(i, j));
				count++;
			}
		}
		sum /= count;
		Scalar bd(data.thsld, data.thsld, data.thsld);
		Mat mask = img->clone();
		Mat dst = Mat(Size(img->cols, img->rows), img->type());
		dst.setTo(data.scl);
		inRange((*img), Scalar(sum) - bd, Scalar(sum) + bd, mask);
		img->copyTo(dst, 255-mask);
		namedWindow("Mask", WINDOW_AUTOSIZE);
		setMouseCallback("Mask", closeCallback);
		imshow("Mask", dst);

	}
}

int main(int argc, char* argv) {
	
	//Task 1

	Mat img = imread("./images/robocup.jpg");
	namedWindow("Image", WINDOW_AUTOSIZE);
	imshow("Image", img);
	
	//Task 2

	setMouseCallback("Image", mouseCallback1, &img);
	waitKey(0);

	//Task 3

	setMouseCallback("Image", mouseCallback2, &img);
	imshow("Image", img);
	waitKey(0);

	//Task 4

	struct pData d;
	d.img = &img;
	d.thsld = 70;
	setMouseCallback("Image", mouseCallback3, &d);
	imshow("Image", img);
	waitKey(0);

	//Task 5

	d.scl = Scalar(92, 37, 201);
	setMouseCallback("Image", mouseCallback4, &d);
	imshow("Image", img);
	waitKey(0);
	

	return 0;
}