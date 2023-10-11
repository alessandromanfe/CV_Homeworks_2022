#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <regex>
#include <iostream>
#include <filesystem>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

void surf() {

}

void orb() {

}

void task1(char** argv) {
	Mat img1 = imread(argv[1]);
	Mat img2 = imread(argv[2]);
	namedWindow("Image 1", WINDOW_AUTOSIZE);
	namedWindow("Image 2", WINDOW_AUTOSIZE);
	imshow("Image 1", img1);
	imshow("Image 2", img2);
	waitKey(0);

	//MSER 

	Ptr<MSER> detector = MSER::create();
	vector<KeyPoint> keypoints1, keypoints2;
	Mat descriptors1, descriptors2;
	detector->detect(img1, keypoints1);
	detector->detect(img2, keypoints2);
	Ptr<MSER> brief = MSER::create();
	try {
		brief->compute(img1, keypoints1, descriptors1);
		brief->compute(img2, keypoints2, descriptors2);
	}
	catch (Exception e) {
		printf("%s", e.what());
	}
	Mat img_keypoints1, img_keypoints2;
	drawKeypoints(img1, keypoints1, img_keypoints1);
	drawKeypoints(img2, keypoints2, img_keypoints2);
	imshow("MSER Keypoints1", img_keypoints1);
	imshow("MSER Keypoints2", img_keypoints2);
	waitKey(0);

	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE_HAMMING);
	std::vector< std::vector<DMatch> > knn_matches;
	matcher->knnMatch(descriptors1, descriptors2, knn_matches, 2);

	const float ratio_thresh = 0.7f;
	std::vector<DMatch> good_matches;
	for (size_t i = 0; i < knn_matches.size(); i++){
		if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance){
			good_matches.push_back(knn_matches[i][0]);
		}
	}
	//-- Draw matches
	Mat img_matches;
	drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches, Scalar::all(-1),
		Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	//-- Show detected matches
	imshow("Good Matches", img_matches);
	waitKey(0);

	
}

int main(int argc, char** argv) {
	if (argc == 3) {
		/*
		regex reg("(([^\\/\\n]*\\/)*)[^\\/\\n]+\\.((jpg)|(jpeg)|(png))");
		bool m1 = regex_match(argv[1], reg);
		bool m2 = regex_match(argv[2], reg);
		*/
		if (filesystem::exists(argv[1]) && filesystem::exists(argv[2]))
			task1(argv);
		else printf("Arguments are not valid paths or files do not exist.");
	}
	else printf("Wrong number of arguments( expected: 2 ).");

	return 0;
}