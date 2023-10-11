#include <opencv2\highgui.hpp>
#include <opencv2\core\matx.hpp>
#include <opencv2\imgcodecs.hpp>
#include <opencv2\imgproc.hpp>
#include <iostream>

int main(int argc, char** argv){

    // Task 1-2

    /* 
    cv::Mat img = cv::imread(argv[1]);
    cv::namedWindow("Example 1");
    cv::imshow("Example 1", img);
    int nc = img.channels();
    std::cout << nc << std::endl;
    char wk = cv::waitKey(0);
    std::cout << wk << std::endl;
    */

    //Task 3

    /*
    cv::Mat img1(256, 256, CV_8U);
    cv::Mat img2 = img1.clone();
    for (int i=0; i<img1.cols; i++) {
        for (int j=0; j<img1.rows; j++) {
            img1.at<uchar>(i, j) = j;
            img2.at<uchar>(i, j) = i;
        }
    }
    cv::namedWindow("Image 1");
    cv::namedWindow("Image 2");
    cv::imshow("Image 1", img1);
    cv::imshow("Image 2", img2);

    cv::Mat img3(300, 300, CV_8U);
    cv::Mat img4 = img3.clone();
    for (int i = 0; i < img3.cols; i++) {
        for (int j = 0; j < img3.rows; j++) {
            img3.at<uchar>(i, j) = !((i/20)%2) != !((j/20)%2) ? 255 : 0;
            img4.at<uchar>(i, j) = !((i/50)%2) != !((j/50)%2) ? 255 : 0;
        }
    }
    cv::namedWindow("Image 3");
    cv::namedWindow("Image 4");
    cv::imshow("Image 3", img3);
    cv::imshow("Image 4", img4);
    cv::waitKey(0);
    */

    //Task 4
    /*
    cv::Mat img = cv::imread(argv[1]);
    cv::namedWindow("Example 1");
    cv::imshow("Example 1", img);
    int nc = img.channels();
    std::cout << nc << std::endl;
    if (nc == 3) {
        for (int c = 0; c < 3; c++) {
            cv::Mat temp = img.clone();
            for (int i = 0; i < temp.cols; i++) {
                for (int j = 0; j < temp.rows; j++) {
                    temp.at<cv::Vec3b>(i, j)[c] = 0;
                }
            }
            cv::imshow("Example 1", temp);
            cv::waitKey(0);
        }
    }
    */

    //Task 5
    /*
    std::string ws[] = { "Image1","Image2","Image3" };
    cv::Mat img = cv::imread(argv[1]);
    int nc = img.channels();
    std::cout << nc << std::endl;
    if (nc == 3) {
        for (int c = 0; c < 3; c++) {
            cv::Mat temp = img.clone();
            std::cout << "cloned" << std::endl;
            for (int i = 0; i < temp.cols; i++) {
                for (int j = 0; j < temp.rows; j++) {
                    temp.at<cv::Vec3b>(i, j)[(c+1)%3] = 0;
                    temp.at<cv::Vec3b>(i, j)[(c+2)%3] = 0;
                }
            }
            std::cout << c << std::endl;
            cv::namedWindow(ws[c]);
            cv::imshow(ws[c], temp);
        }
    }
    cv::waitKey(0);
    */

    //Task 6
    cv::Mat img = cv::imread(argv[1]);
    cv::Point center(img.cols / 2, img.rows / 2);
    double angle = 90.0;
    double scale = 0.5;
    cv::Mat rotMat = cv::getRotationMatrix2D(center, angle, scale);
    cv::Mat imgRot;
    cv::warpAffine(img, imgRot, rotMat, img.size());
    cv::namedWindow("Rotation");
    cv::imshow("Rotation", imgRot);
    cv::Point2f srcTri[3];
    cv::Point2f dstTri[3];
    srcTri[0] = cv::Point2f(0, 0);
    srcTri[1] = cv::Point2f(img.cols-1, 0);
    srcTri[2] = cv::Point2f(0, img.rows-1);
    dstTri[0] = cv::Point2f(0, img.rows*0.8);
    dstTri[1] = cv::Point2f(img.cols-1, img.rows-1);
    dstTri[2] = cv::Point2f(0, 0);
    cv::Mat warpMat = cv::getAffineTransform(srcTri, dstTri);
    cv::Mat imgAff;
    cv::warpAffine(img, imgAff, warpMat, img.size());
    cv::namedWindow("Affine Transform");
    cv::imshow("Affine Transform", imgAff);
    cv::waitKey(0);
}

