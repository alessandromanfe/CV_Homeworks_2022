#include <iostream>
#include <opencv2/highgui.hpp>
#include "filters.h"

using namespace cv;
using namespace std;

int filters::maxFilter(Mat& src, Mat& dst, int size) {
	if (!(size % 2)) {
		cout << "Invalid kernel size." << endl;
		return -1;
	}
	if (src.channels() != 1) {
		cout << "Image is not grayscale." << endl;
		return -1;
	}
	dst = src.clone();
	uchar maxv;
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			maxv = 0;
			for (int s = max(0, i - size / 2); s <= min(src.rows-1, i + size / 2); s++) {
				for (int t = max(0, j - size / 2); t <= min(src.cols-1, j + size / 2); t++) {
					if (src.at<uchar>(s, t) > maxv) {
						maxv = src.at<uchar>(s, t);
					}
				}
			}
			dst.at<uchar>(i, j) = maxv;
		}
	}
	cout << "generating image" << endl;
	return 1;
}

int filters::minFilter(Mat& src, Mat& dst, int size) {
	if (!(size % 2)) {
		cout << "Invalid kernel size." << endl;
		return -1;
	}
	if (src.channels() != 1) {
		cout << "Image is not grayscale." << endl;
		return -1;
	}
	dst = src.clone();
	uchar minv;
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			minv = UCHAR_MAX;
			for (int s = max(0, i - size / 2); s <= min(src.rows-1, i + size / 2); s++) {
				for (int t = max(0, j - size / 2); t <= min(src.cols-1, j + size / 2); t++) {
					if (src.at<uchar>(s, t) < minv) {
						minv = src.at<uchar>(s, t);
					}
				}
			}
			dst.at<uchar>(i, j) = minv;
		}
	}
	cout << "generating image" << endl;
	return 1;
}