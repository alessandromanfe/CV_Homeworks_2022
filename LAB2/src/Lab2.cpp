#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "filters.h"

using namespace cv;
using namespace std;


/**
* Generates the image plot of the histogram.
* 
* Takes in input the histogram ( Mat ) and returns a new image ( Mat ) 
* plotting the graph and some reference values along the axes.
* 
* @param hist Histogram matrix
* @param size (Optional) Base size of the plot image
* @param scaleX (Optional) Width scale factor 
* @param scaleY (Optional) Height scale factor 
* @returns Image plot of the histogram
*/
Mat histToImg(Mat hist, Size size = Size(512,256), float scaleX = 1, float scaleY = 1) {
	double maxVal = 0;
	minMaxLoc(hist, 0, &maxVal, 0, 0);
	int width = size.width;
	int height = size.height;
	int bins = hist.rows;
	Mat histImg = Mat::ones(height * scaleY, width * scaleX, CV_8U) * 255;
	int pxPerBin = width / bins;
	for (int i = 0; i < bins; i++) {
		float val = hist.at<float>(i, 0);
		Point pt1((i*pxPerBin +1) * scaleX, height*(1-val/maxVal) * scaleY);
		Point pt2(((i+1) * pxPerBin -1) * scaleX, height*(1-val/maxVal) * scaleY);
		Point pt3(((i + 1) * pxPerBin -1) * scaleX, height-1);
		Point pt4((i * pxPerBin + 1) * scaleX, height-1);
		Point pts[] = { pt1, pt2, pt3, pt4 };
		fillConvexPoly(histImg, pts, 4, Scalar(0, 0, 0));
	}
	Mat img = Mat::ones(histImg.rows + 100, histImg.cols + 100, CV_8U) * 255;
	int ox = 90;
	int oy = histImg.rows * 1.1f;
	Mat insetImage(img, Rect(ox, oy-histImg.rows, histImg.cols, histImg.rows));
	histImg.copyTo(insetImage);
	line(img, Point(ox-2, oy+2), Point(ox-2,5), Scalar(0), 2);
	line(img, Point(ox-2, oy+2), Point(img.cols-5, oy+2), Scalar(0), 2);
	line(img, Point(ox - 2 - 5, 5 + 5), Point(ox - 2, 5), Scalar(0), 2);
	line(img, Point(ox - 2 + 5, 5 + 5), Point(ox - 2, 5), Scalar(0), 2);
	line(img, Point(img.cols - 5 - 5, oy + 2 + 5), Point(img.cols - 5, oy + 2), Scalar(0), 2);
	line(img, Point(img.cols - 5 - 5, oy + 2 - 5), Point(img.cols - 5, oy + 2), Scalar(0), 2);
	char buff[10];
	float tot = sum(hist)[0];
	for (int i = 0; i < 10; i++) {
		float val = i * maxVal / tot / 9 * 100;
		sprintf(buff, "%.2f%%", val);
		line(img,
			Point(ox-5, oy-histImg.rows * (1.0/9 * i)),
			Point(ox, oy-histImg.rows * (1.0/9 * i)),
			Scalar(0), 2);
		putText(img, buff,
			Point(ox-65, oy + 5 - histImg.rows * (1.0/9 * i)),
			FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0), 1);
		int bin = i * bins / 10;
		sprintf(buff, "%d", bin);
		line(img,
			Point(ox + histImg.cols * 0.1 * i, oy),
			Point(ox + histImg.cols * 0.1 * i, oy+5),
			Scalar(0), 2);
		putText(img, buff, 
			Point(ox - 5 + histImg.cols * 0.1 * i, oy + 25),
			FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0), 1);
	}
	putText(img, "%/Bin", Point(img.cols / 2 - 20, img.rows - 20),
		FONT_HERSHEY_DUPLEX, 0.6, Scalar(0), 1);
	return img;
}

void saveImg(Mat img, int n) {
	char buff[15];
	sprintf(buff, "./Image%d.jpg", n);
	imwrite(buff, img);
}

int main(int argc, char** argv) {

	int count = 0; //Incremental counter for saving images

	//Task 1

	Mat img = imread("./images/image.jpg");
	Mat gray_img;
	//this line is not required, used for conveninece during developement
	//resize(img, img, Size(img.cols / 2, img.rows / 2));
	namedWindow("Image", WINDOW_AUTOSIZE);
	imshow("Image", img);
	cvtColor(img, gray_img, COLOR_BGR2GRAY);
	imwrite("./images/image_grayscale.jpg", gray_img);
	if(waitKey(0)=='s') saveImg(img,count++);
	destroyAllWindows();


	//N.B. Following tasks are inside while loops, which are more practical
	//to test all parameter values.


	//Task2

	int n = 0;
	do {
		cout << "Insert maxFilter kernel size (0 to skip): ";
		cin >> n;
		if (n == 0) break;
		Mat temp;
		if (filters::maxFilter(gray_img, temp, n)<0) continue;
		namedWindow("MaxFilter", WINDOW_AUTOSIZE);
		imshow("MaxFilter", temp);
		if (waitKey(0) == 's') saveImg(temp, count++);
		destroyAllWindows();
	} while (n != 0);

	do {
		cout << "Insert minFilter kernel size (0 to skip): ";
		cin >> n;
		if (n == 0) break;
		Mat temp;
		if (filters::minFilter(gray_img, temp, n)<0) continue;
		namedWindow("MinFilter", WINDOW_AUTOSIZE);
		imshow("MinFilter", temp);
		if (waitKey(0) == 's') saveImg(temp, count++);
		destroyAllWindows();
	} while (n != 0);

	//Task 3

	do {
		cout << "Insert medianBlur kernel size (0 to skip): ";
		cin >> n;
		if (n == 0) break;
		if (!(n % 2)) {
			cout << "Invalid kernel size." << endl;
			continue;
		}
		Mat temp;
		medianBlur(gray_img, temp, n);
		namedWindow("MedianBlur", WINDOW_AUTOSIZE);
		imshow("MedianBlur", temp);
		if (waitKey(0) == 's') saveImg(temp, count++);
		destroyAllWindows();
	} while (n != 0);

	do {
		cout << "Insert GaussianBlur kernel size (0 to skip): ";
		cin >> n;
		if (n == 0) break;
		if (!(n % 2)) {
			cout << "Invalid kernel size." << endl;
			continue;
		}
		Mat temp;
		GaussianBlur(gray_img, temp, Size(n,n), 0);
		namedWindow("GaussianBlur", WINDOW_AUTOSIZE);
		imshow("GaussianBlur", temp);
		if (waitKey(0) == 's') saveImg(temp, count++);
		destroyAllWindows();
	} while (n != 0);

	//Task 4

	Mat hist;
	int bins = 0;
	int channels[] = { 0 };
	float histRanges[] = { 0,255 };
	const float* ranges[] = {histRanges};
	do {
		cout << "Insert number of bins (0 to skip): ";
		cin >> n;
		if (n == 0) break;
		int histSize[] = { n };
		calcHist(&gray_img, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);
		Mat histImg = histToImg(hist);
		namedWindow("Histogram", WINDOW_AUTOSIZE);
		imshow("Histogram", histImg);
		if (waitKey(0) == 's') saveImg(histImg, count++);
		destroyAllWindows();
	} while (n != 0);

	//Task 5

	Mat eqImg;
	Mat eqHist;
	equalizeHist(gray_img, eqImg);
	calcHist(&gray_img, 1, channels, Mat(), hist, 1, new int[] { 255 }, ranges, true, false);
	calcHist(&eqImg, 1, channels, Mat(), eqHist, 1, new int[] { 255 }, ranges, true, false);
	Mat histImg = histToImg(hist);
	Mat histEqImg = histToImg(eqHist);
	imshow("Image", gray_img);
	namedWindow("Histogram", WINDOW_AUTOSIZE);
	imshow("Histogram", histImg);
	namedWindow("Equalized Image", WINDOW_AUTOSIZE);
	imshow("Equalized Image", eqImg);
	namedWindow("Equalized Histogram", WINDOW_AUTOSIZE);
	imshow("Equalized Histogram", histEqImg);
	if (waitKey(0) == 's') {
		saveImg(gray_img, count++);
		saveImg(histImg, count++);
		saveImg(eqImg, count++);
		saveImg(histEqImg, count++);
	}
	return 0;
}

	