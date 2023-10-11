#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

typedef struct{
	int t1;
	int t2;
	int apt;
	Mat* img;
	Mat* canny;
} tData;

typedef struct {
	Point p1;
	Point p2;
	double acc;
	int size;
} lineData;

void saveImg(Mat img, int n) {
	char buff[15];
	sprintf(buff, "./Image%d.jpg", n);
	imwrite(buff, img);
}

void trackbarCallback(int pos, void *data) {
	tData x = *((tData*) data);
	Canny((*x.img), (*x.canny), x.t1*10.0, x.t2*10.0, 3+x.apt*2);
	imshow("Canny", (*x.canny));
}

void equalizeImage(Mat& src, Mat& dst) {
	vector<uchar> map;
	cvtColor(src, dst, COLOR_BGR2GRAY);
	for (int i = 0; i < 256; i++) {
		map.push_back(max(0, static_cast<int>((i - 200) * 255.0 / 55.0)));
	}
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			uchar x = dst.at<uchar>(i, j);
			dst.at<uchar>(i, j) = map[x];
		}
	}
}

void lineDetect(InputArray src, vector<lineData>& dst, int scale,
				int thold = 128, double min_acc = 0.5, int min_len = 20) {
	Mat scaled;
	resize(src, scaled, src.size()/scale,0,0,INTER_AREA);
	imshow("scaled", scaled);
	waitKey(0);
	for (int i = 0; i < scaled.rows; i++) {
		for (int j = 0; j < scaled.cols; j++) {
			if (scaled.at<uchar>(i, j) < thold) continue;
			for (int k = i; k < scaled.rows; k++) {
				for (int l = ((k == i) ? (j + 1) : 0); l < scaled.cols; l++) {
					if (scaled.at<uchar>(k, l) < thold) continue;
					double dx = l - j;
					double dy = k - i;
					if (max(abs(dx), abs(dy)) < min_len) continue;
					int steps = max(abs(dx), abs(dy));
					int count = 0;
					for (int m = 0; m < steps; m++) {
						if (scaled.at<uchar>(
							i + round(m * dy / steps),
							j + round(m * dx / steps))
							> 0) count++;
					}
					double acc = count * 1.0 / steps;
					if (steps >= min_len && acc >= min_acc) {
						Point p1(j, i);
						Point p2(l, k);
						dst.push_back(lineData{
							p1,
							p2,
							acc,
							steps });
					}
				}
			}
		}
	}

}

int main(int argc, char* argv) {

	int count = 0; //Incremental counter for saving images

	//Task 1 

	Mat img = imread("./images/street_scene.png");
	Mat eqImg;
	equalizeImage(img, eqImg);
	Mat canny;
	namedWindow("Image", WINDOW_AUTOSIZE);
	namedWindow("Canny", WINDOW_AUTOSIZE);
	imshow("Image", img);
	tData d = { 50,85,0,&img,&canny };
	createTrackbar("Thold 1","Image",&d.t1,100,trackbarCallback,&d);
	createTrackbar("Thold 2","Image",&d.t2,100,trackbarCallback,&d);
	createTrackbar("Aperture","Image",&d.apt,2,trackbarCallback,&d);
	
	Canny(img, canny, d.t1*10.0, d.t2*10.0, 3+d.apt*2);
	imshow("Canny", canny);
	if (waitKey(0) == 's') saveImg(canny, count++);
	destroyAllWindows();

	//Task 2

	namedWindow("scaled", WINDOW_AUTOSIZE);
	vector<lineData> dst;
	int scale = 3;
	lineDetect(canny, dst, scale, 128, 0.95, 90/scale);
	sort(dst.begin(), dst.end(),
		[](lineData const& a, lineData const& b) {
		return a.acc*a.size > b.acc*b.size;
	});
	Mat imgl = img.clone();
	for (int i = 0; i < min(20,static_cast<int>(dst.size())); i++) {
		line(imgl, dst[i].p1*scale, dst[i].p2*scale, Scalar(0, 0, 255), 2, LINE_AA);
	}
	namedWindow("lines", WINDOW_AUTOSIZE);
	imshow("lines", imgl);
	if (waitKey(0) == 's') saveImg(imgl, count++);
	destroyAllWindows();

	//Task 3

	Mat img2 = img.clone();
	vector<Vec2f> lines;
	int c = 0;
	float m[2], q[2];
	HoughLines(canny, lines, 2, CV_PI/45, 170);
	for (size_t i = 0; i < lines.size(); i++){
		float rho = lines[i][0], theta = lines[i][1];
		//Check if line angle stays in desired range
		if ( ! ((theta > CV_PI / 6 && theta < 2 * CV_PI / 6)
			|| (theta > 4 * CV_PI / 6 && theta < 5 * CV_PI / 6)))
			continue;
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 2000 * (-b));
		pt1.y = cvRound(y0 + 2000 * (a));
		pt2.x = cvRound(x0 - 2000 * (-b));
		pt2.y = cvRound(y0 - 2000 * (a));
		line(img2, pt1, pt2, Scalar(0, 255, 0), 1, LINE_AA);
		if (c < 2) {
			m[c] = (pt1.y-pt2.y)/(1.0*pt1.x-pt2.x);
			q[c] = pt1.y - m[c] * pt1.x;
			c++;
		}
	}
	int xcross = round((q[1] - q[0]) / (m[0] - m[1]));
	int ycross = round(m[0] * xcross + q[0]);
	Point cross(xcross, ycross);
	Point p0((img2.rows - 1 - q[0]) / m[0], img2.rows - 1);
	Point p1((img2.rows - 1 - q[1]) / m[1], img2.rows - 1);
	Point p[] = { p0,p1,cross };
	fillConvexPoly(img2, p, 3, Scalar(0, 0, 255));
	namedWindow("hl", WINDOW_AUTOSIZE);
	imshow("hl", img2);
	if (waitKey(0) == 's') saveImg(img2, count++);
	destroyAllWindows();

	//Task 4

	Mat img3 = img.clone();
	Mat bf;
	vector<Vec3f> circles;
	cvtColor(img3, canny, COLOR_BGR2GRAY);
	bilateralFilter(canny, bf, 5, 100, 100);
	HoughCircles(bf, circles, HOUGH_GRADIENT, 1, 100, 700, 25, 0, 100);
	for (size_t i = 0; i < circles.size(); i++){
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		circle(img3, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		circle(img3, center, radius, Scalar(0, 0, 255), 3, 8, 0);
	}
	namedWindow("circles", WINDOW_AUTOSIZE);
	imshow("circles", img3);
	if (waitKey(0) == 's') saveImg(img3, count++);
	destroyAllWindows();
	return 0;
}