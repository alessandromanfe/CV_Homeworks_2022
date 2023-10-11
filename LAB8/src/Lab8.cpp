#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include <set>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {

	char* path = ".\\images\\checkerboard_images\\";
	char* test_path = ".\\images\\test_image.png";

	if (!filesystem::exists(path)) {
		cerr << "Invalid path.";
		return -1;
	}
	if (!filesystem::exists(test_path)) {
		cerr << "Invalid test path.";
		return -1;
	}
	set<string> extensions = { "png", "bmp", "jpg", "jpeg" };
	vector<Mat> images;
	for (const auto& img : filesystem::directory_iterator(path)) {
		string path = img.path().u8string();
		string ext = path.substr(path.find_last_of('.') + 1);
		if (extensions.find(ext) != extensions.end()) {
			images.push_back(imread(path));
		}
	}

	int board[]{ 6,5 };

	vector<vector<Point2f>> corners;
	vector<vector<Point2f>> imgpoints;
	vector<vector<Point3f>> objpoints;
	vector<Point3f> objp;
	for (int i{ 0 }; i < board[1]; i++){
		for (int j{ 0 }; j < board[0]; j++)
			objp.push_back(cv::Point3f(j, i, 0));
	}

	for (int i = 0; i < images.size(); i++) {
		vector<Point2f> corner_pts;
		bool success = findChessboardCorners(images[i], Size(6, 5), corner_pts, 
			CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
		//cornerSubPix(images[i], corner_pts, Size(10, 10), Size(-1,-1), TermCriteria(TermCriteria::MAX_ITER,100,1e-3));
		if (success) {
			drawChessboardCorners(images[i], Size(6, 5), corner_pts, success);
			objpoints.push_back(objp);
			imgpoints.push_back(corner_pts);
			corners.push_back(corner_pts);
		}
		else cout << "pattern not found img n�: " << i << endl;
		//imshow("Image", images[i]);
		//waitKey(0);
	}
	destroyAllWindows();
	Mat cameraMatrix, distCoeffs, R, T;

	double reprjerr = calibrateCamera(objpoints, imgpoints, cv::Size(images[0].rows, images[0].cols), cameraMatrix, distCoeffs, R, T);

	cout << "cameraMatrix : " << cameraMatrix << endl;
	cout << "distCoeffs : " << distCoeffs << endl;
	cout << "Rotation vector : " << R << endl;
	cout << "Translation vector : " << T << endl;
	cout << "Reprojection Error : " << reprjerr << endl;

	Mat test_img = imread(test_path);
	Mat map1, map2, remapped_img;
	try {

		initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(), cameraMatrix, Size(test_img.rows, test_img.cols),
			CV_32FC1, map1, map2);
	}
	catch (Exception e) {
		cout << e.what() << endl;
	}
	remap(test_img, remapped_img, map1, map2, INTER_CUBIC);
	imshow("Image", remapped_img);
	waitKey(0);

	return 0;
}