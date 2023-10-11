#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "utils.h"
#include <iostream>

using namespace cv;
using namespace std;

typedef struct {
	int t1;
	int t2;
	int apt;
	vector<Mat> &img;
} tData;

typedef struct {
	const Mat& m;
	vector<pair<int, int>>& seed;
} mData;

void mouseCallback(int e, int x, int y, int flags, void* userdata) {
	if (e == EVENT_LBUTTONDOWN) {
		mData data = *((mData*)userdata);
		data.seed.push_back(pair<int, int>(x, y));
		circle(data.m, Point(x, y), 4, Scalar(0, 0, 255), FILLED);
		imshow("Pick seed", data.m);
	}
}

void trackbarCallback(int pos, void* data) {
	tData x = *((tData*)data);
	Mat img1_canny, img2_canny, img3_canny;
	GaussianBlur(x.img[0], x.img[3], cv::Size(5, 5), 100);
	GaussianBlur(x.img[4], x.img[7], cv::Size(5, 5), 100);
	GaussianBlur(x.img[8], x.img[11], cv::Size(5, 5), 100);
	Canny(x.img[3], img1_canny, x.t1 * 5.0, x.t2 * 5.0, 3 + x.apt * 2);
	Canny(x.img[7], img2_canny, x.t1 * 5.0, x.t2 * 5.0, 3 + x.apt * 2);
	Canny(x.img[11], img3_canny, x.t1 * 5.0, x.t2 * 5.0, 3 + x.apt * 2);
	x.img[3] = img1_canny.clone();
	x.img[7] = img2_canny.clone();
	x.img[11] = img3_canny.clone();
	imshow("Segmentation", lab5utils::makeCanvas(x.img, 1000, 3));
}

void task1() {

	char* names[] = { "./images/Asphalt-1.png",
					  "./images/Asphalt-2.png",
					  "./images/Asphalt-3.png" };
	Mat imgs[3][4];
	imgs[0][0] = imread(names[0]);
	imgs[1][0] = imread(names[1]);
	imgs[2][0] = imread(names[2]);
	lab5utils::regGrowing(imgs[0][0], imgs[0][1], 1, 35, 200, 9, false);
	lab5utils::regGrowing(imgs[1][0], imgs[1][1], 1, 65, 200, 5, false);
	lab5utils::regGrowing(imgs[2][0], imgs[2][1], 1, 30, 200, 6, false);

	Mat img0_grey, img1_grey, img2_grey;
	cvtColor(imgs[0][0], img0_grey, COLOR_BGR2GRAY);
	cvtColor(imgs[1][0], img1_grey, COLOR_BGR2GRAY);
	cvtColor(imgs[2][0], img2_grey, COLOR_BGR2GRAY);

	threshold(img0_grey, imgs[0][2], 25, 255, THRESH_BINARY_INV);
	threshold(img1_grey, imgs[1][2], 30, 255, THRESH_BINARY_INV);
	threshold(img2_grey, imgs[2][2], 25, 255, THRESH_BINARY_INV);

	vector<vector<pair<int, int>>> seed_set(3);
	
	for (int i = 0; i < 3; i++) {
		namedWindow("Pick seed", WINDOW_AUTOSIZE);
		imshow("Pick seed", imgs[i][0]);
		mData data = { imgs[i][0], seed_set[i] };
		setMouseCallback("Pick seed", mouseCallback, &data);
		waitKey(0);
	}
	/*
	for (int i = 0; i < 3; i++) {
		Mat img_grey, img_blur, img_thold, img_seed;
		cvtColor(imgs[0][i], img_grey, COLOR_BGR2GRAY);
		bilateralFilter(img_grey, img_blur, 3, 800, 800);
		//cv::GaussianBlur(img_grey, img_blur, cv::Size(5, 5), 100);
		threshold(img_blur, img_thold, 50, 255, THRESH_BINARY_INV);
		int erosion_size = 2, erosion_type = MORPH_CROSS;
		Mat element = cv::getStructuringElement(erosion_type,
			Size(2 * erosion_size + 1, 2 * erosion_size + 1),
			Point(erosion_size, erosion_size));
		erode(img_thold, img_seed, element, Point(-1, -1), 1);
		vector<Point> seeds;
		findNonZero(img_seed, seeds);
		for (int j = 0; j < seeds.size(); j++) {
			seed_set[j].push_back(pair<int,int>(seeds[j].x, seeds[j].y));
		}
	}
	*/
	lab5utils::regGrowing2(img0_grey, seed_set[0], imgs[0][3]);
	lab5utils::regGrowing2(img1_grey, seed_set[1], imgs[1][3]);
	lab5utils::regGrowing2(img2_grey, seed_set[2], imgs[2][3]);


	/*
	Mat img1_canny, img2_canny, img3_canny;
	Canny(imgs[0][0], img1_canny, 0, 30);
	Canny(imgs[1][0], img2_canny, 0, 30);
	Canny(imgs[2][0], img3_canny, 0, 30);
	imgs[0][3] = img1_canny;
	imgs[1][3] = img2_canny;
	imgs[2][3] = img3_canny;
	*/

	/*
	Mat img1_seg, img2_seg, img3_seg;
	Mat lab1, lab2, lab3;
	lab5utils::clusterSegment(imgs[0][0], img1_seg, 7, 3, lab1);
	lab5utils::clusterSegment(imgs[1][0], img2_seg, 7, 3, lab2);
	lab5utils::clusterSegment(imgs[2][0], img3_seg, 7, 3, lab3);
	Mat k1 = (lab1 == Mat::ones(lab1.size(), CV_32S) * 3 );
	Mat k2 = (lab2 == Mat::ones(lab2.size(), CV_32S) * 4);
	Mat k3 = (lab3 == Mat::ones(lab3.size(), CV_32S) * 3);
	imgs[0][3] = k1;
	imgs[1][3] = k2;
	imgs[2][3] = k3;
	*/

	/*
	imgs[0][3] = img1_seg;
	imgs[1][3] = img2_seg;
	imgs[2][3] = img3_seg;
	*/


	vector<Mat> images = { imgs[0][0], imgs[0][1], imgs[0][2], imgs[0][3],
						   imgs[1][0], imgs[1][1], imgs[1][2], imgs[1][3],
						   imgs[2][0], imgs[2][1], imgs[2][2], imgs[2][3] };
	Mat canvas = lab5utils::makeCanvas(images, 1000, 3);
	namedWindow("Segmentation", WINDOW_AUTOSIZE);
	imshow("Segmentation", canvas);

	/*
	tData d = { 15,40,0, images };
	createTrackbar("Thold 1", "Segmentation", &d.t1, 100, trackbarCallback, &d);
	createTrackbar("Thold 2", "Segmentation", &d.t2, 100, trackbarCallback, &d);
	createTrackbar("Aperture", "Segmentation", &d.apt, 2, trackbarCallback, &d);
	*/


	waitKey(0);
}

void task2() {
	Mat img = imread("./images/street_scene.png");
	Mat clusters, c1, c2, c3, labels;
	lab5utils::clusterSegment(img, clusters, 3, 3, labels);
	
	Mat k1 = (labels == Mat::ones(labels.size(), CV_32S) * 0);
	Mat k2 = (labels == Mat::ones(labels.size(), CV_32S) * 1);
	Mat k3 = (labels == Mat::ones(labels.size(), CV_32S) * 2);
	img.copyTo(c1, k1);
	img.copyTo(c2, k2);
	img.copyTo(c3, k3);

	vector<Mat> images = { img, c1, c2, c3 };
	Mat canvas = lab5utils::makeCanvas(images, 500, 2);

	namedWindow("Clustering", WINDOW_AUTOSIZE);
	imshow("Clustering", canvas);
	waitKey(0);
}

void task3() {
	Mat img = imread("./images/robocup.jpg");
	Mat clusters, labels, orange, tshirt, hsv, mask;
	lab5utils::clusterSegment(img, clusters, 6, 3, labels);

	Mat k1 = (labels == Mat::ones(labels.size(), CV_32S) * 5);
	img.copyTo(orange, k1);
	cvtColor(orange, hsv, COLOR_BGR2HSV);
	inRange(hsv, Scalar(18, 0, 0), Scalar(40, 255, 255), mask);
	img.copyTo(tshirt, mask);

	vector<Mat> images = { img, tshirt };
	Mat canvas = lab5utils::makeCanvas(images, 1000, 2);

	namedWindow("Clustering", WINDOW_AUTOSIZE);
	imshow("Clustering", canvas);
	waitKey(0);
}

int main(int argc, char** argv) {
	task1();
	task2();
	task3();
}