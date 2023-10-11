#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <filesystem>
#include <set>
#include "panoramic_utils.h"

using namespace std;
using namespace cv;
using namespace xfeatures2d;



/**
 * Compare function implementation for DMatch instances
 */
bool cmpMatch(const DMatch& a, const DMatch& b) {
	return a.distance < b.distance;
}


/**
 * Function that computes Perspective Transformation matrices from feature matching of given images
 */
vector<Mat> getTransformMatrix(const vector<Mat>& cyl_img,
							   const vector<vector<KeyPoint>>& keypoints,
							   const vector<Mat>& descriptors,
							   int ratio) {
	Ptr<BFMatcher> matcher = BFMatcher::create();
	vector<DMatch> matches;
	vector<Mat> hgpt;
	for (int i = 1; i < cyl_img.size(); i++) {
		matcher->match(descriptors[i], descriptors[i - 1], matches);
		//Find minimum distance among matches instances
		float min_dst = min_element(matches.begin(), matches.end(), cmpMatch)->distance;
		float threshold = ratio * min_dst;
		vector<DMatch> min_matches;
		//Filter matches by upper distance threshold
		copy_if(matches.begin(), matches.end(), back_inserter(min_matches),
			[&threshold](const DMatch& x) {
			return x.distance < threshold;
		});
		//Calculate vectors of keypoints from pair of images 
		vector<Point2f> points1;
		vector<Point2f> points2;
		for (const DMatch& m : min_matches) {
			points1.push_back(keypoints[i - 1][m.trainIdx].pt);
			points2.push_back(keypoints[i][m.queryIdx].pt);
		}
		Mat mask;
		//Add perspective transform matrix to vector
		hgpt.push_back(findHomography(points2, points1, mask, RANSAC, 3.0));
		/*
		Mat img_match;
		drawMatches(cyl_img[i], keypoints[i],
			cyl_img[i - 1], keypoints[i - 1],
			min_matches, img_match);
		imshow("Match", img_match);
		waitKey(0);
		*/
	}
	return hgpt;
}



int main(int argc, char** argv) {
	
	//Command line arguments parsing
	char* path;
	int fov;
	if (argc != 3 && argc != 4) {
		cerr << "Incorrect number of parameters ( path fov [ratio] ).";
		return -1;
	}
	path = argv[1];
	if (!filesystem::exists(path)) {
		cerr << "Invalid path.";
		return -1;
	}
	fov = stoi(argv[2]);

	//User defined ratio for thresholding features
	int ratio;
	if (argc == 4) ratio = stoi(argv[3]);
	else ratio = 4; //default value

	

	//Read all images from given directory
	set<string> extensions = { "png", "bmp", "jpg", "jpeg" };
	vector<Mat> images;
	for (const auto &img : filesystem::directory_iterator(path)) {
		string path = img.path().u8string();
		string ext = path.substr(path.find_last_of('.') + 1);
		if (extensions.find(ext) != extensions.end()) {
			images.push_back(imread(path));
		}
	}

	//Compute cylindrical projections, keypoints and features descriptors
	vector<Mat> cyl_images;
	vector<vector<KeyPoint>> keypoints;
	vector<Mat> descriptors;
	Ptr<SIFT> detector = SIFT::create();
	for (const Mat &img : images) {
		Mat cyl_img = cylindricalProj(img, fov);
		cyl_images.push_back(cyl_img);
		vector<KeyPoint> kp;
		detector->detect(cyl_img, kp);
		keypoints.push_back(kp);
		Mat desc;
		detector->compute(cyl_img, kp, desc);
		descriptors.push_back(desc);
		/*
		Mat kimg;
		drawKeypoints(cyl_img, kp, kimg);
		imshow("Keypoints", kimg);
		waitKey(0);
		*/
	}

	//Compute perspective transformation matrices
	vector<Mat> hgpt = getTransformMatrix(cyl_images, keypoints, descriptors, ratio);

	//Get images dimensions
	int width = cyl_images[0].cols;
	int height = cyl_images[0].rows;
	//Calculate total width of panorama image from translation component 
	//of transformation matrices (0,2)
	int sum = 0;
	for (const Mat& x : hgpt) sum += x.at<double>(0, 2);
	int tot_width = sum + width;

	//Incremental x coordinate for images translation
	int x_coord = 0;
	Mat panorama(height, tot_width, CV_8U);
	//Copy first image
	cyl_images[0].copyTo(panorama(Rect(x_coord, 0, width, height)));

	//Arrange images according to calculated translations
	for (int i = 0; i < hgpt.size(); i++) {
		x_coord += hgpt[i].at<double>(0, 2);
		cyl_images[i + 1].copyTo(panorama(Rect(x_coord, 0, width, height)));
	}

	//Resize panorama image to a more comfortable size 
	resize(panorama, panorama, Size(2000, static_cast<int>((2000.0*height)/tot_width)));
	imshow("Panorama", panorama);
	waitKey(0);

	return 0;
}