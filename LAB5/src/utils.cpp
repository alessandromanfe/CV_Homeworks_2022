#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "utils.h"

cv::Mat lab5utils::makeCanvas(std::vector<cv::Mat>& vecMat, int windowHeight, int nRows) {
    int N = vecMat.size();
    nRows = nRows > N ? N : nRows;
    int edgeThickness = 10;
    int imagesPerRow = ceil(double(N) / nRows);
    int resizeHeight = floor(2.0 * ((floor(double(windowHeight - edgeThickness) / nRows)) / 2.0)) - edgeThickness;
    int maxRowLength = 0;

    std::vector<int> resizeWidth;
    for (int i = 0; i < N;) {
        int thisRowLen = 0;
        for (int k = 0; k < imagesPerRow; k++) {
            double aspectRatio = double(vecMat[i].cols) / vecMat[i].rows;
            int temp = int(ceil(resizeHeight * aspectRatio));
            resizeWidth.push_back(temp);
            thisRowLen += temp;
            if (++i == N) break;
        }
        if ((thisRowLen + edgeThickness * (imagesPerRow + 1)) > maxRowLength) {
            maxRowLength = thisRowLen + edgeThickness * (imagesPerRow + 1);
        }
    }
    int windowWidth = maxRowLength;
    cv::Mat canvasImage(windowHeight, windowWidth, CV_8UC3, cv::Scalar(255, 255, 255));

    for (int k = 0, i = 0; i < nRows; i++) {
        int y = i * resizeHeight + (i + 1) * edgeThickness;
        int x_end = edgeThickness;
        for (int j = 0; j < imagesPerRow && k < N; k++, j++) {
            int x = x_end;
            cv::Rect roi(x, y, resizeWidth[k], resizeHeight);
            cv::Size s = canvasImage(roi).size();
            // change the number of channels to three
            cv::Mat target_ROI(s, CV_8UC3);
            if (vecMat[k].channels() != canvasImage.channels()) {
                if (vecMat[k].channels() == 1) {
                    cv::cvtColor(vecMat[k], target_ROI, cv::COLOR_GRAY2BGR);
                }
            }
            else {
                vecMat[k].copyTo(target_ROI);
            }
            cv::resize(target_ROI, target_ROI, s);
            if (target_ROI.type() != canvasImage.type()) {
                target_ROI.convertTo(target_ROI, canvasImage.type());
            }
            target_ROI.copyTo(canvasImage(roi));
            x_end += resizeWidth[k] + edgeThickness;
        }
    }
    return canvasImage;
}


void lab5utils::regGrowing(const cv::Mat& src, cv::Mat& dst, int bin_iter, int bin_thold,
                           int grow_iter, int grow_thold, bool verbose) {

    if(verbose) cv::namedWindow("regGrow", cv::WINDOW_AUTOSIZE);
    cv::Mat img_grey, img_blur, img_thold, img_seed;
    cv::cvtColor(src, img_grey, cv::COLOR_BGR2GRAY);
    if (verbose)cv::imshow("regGrow", img_grey);
    if (verbose)cv::waitKey(0);
    cv::bilateralFilter(img_grey, img_blur, 3, 800, 800);
    //cv::GaussianBlur(img_grey, img_blur, cv::Size(5, 5), 100);
    if (verbose)cv::imshow("regGrow", img_blur);
    if (verbose)cv::waitKey(0);
    cv::threshold(img_blur, img_thold, bin_thold, 255, cv::THRESH_BINARY_INV);
    if (verbose)cv::imshow("regGrow", img_thold);
    if (verbose)cv::waitKey(0);
    int erosion_size = 2, erosion_type = cv::MORPH_CROSS;
    cv::Mat element = cv::getStructuringElement(erosion_type,
        cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
        cv::Point(erosion_size, erosion_size));
    erode(img_thold, img_seed, element, cv::Point(-1, -1), bin_iter);
    if (verbose)cv::imshow("regGrow", img_seed);
    if (verbose)cv::waitKey(0);
    /*
    dst = img_seed.clone();
    return;
    */
    int col = img_grey.cols;
    int row = img_grey.rows;
    for (int c = 0; c < grow_iter; c++) {
        for (int x = 0; x < col; x++) {
            for (int y = 0; y < row; y++) {
                if (img_seed.at<uchar>(y, x) > 0) {
                    cv::Point neighb[] = {
                        cv::Point(cv::max(cv::min(x + 1, col - 1), 0), cv::max(cv::min(y, row - 1), 0)),
                        cv::Point(cv::max(cv::min(x - 1, col - 1), 0), cv::max(cv::min(y, row - 1), 0)),
                        cv::Point(cv::max(cv::min(x, col - 1), 0), cv::max(cv::min(y + 1, row - 1), 0)),
                        cv::Point(cv::max(cv::min(x, col - 1), 0), cv::max(cv::min(y - 1, row - 1), 0))/*,
                        cv::Point(cv::max(cv::min(x + 1, col - 1), 0), cv::max(cv::min(y + 1, row - 1), 0)),
                        cv::Point(cv::max(cv::min(x + 1, col - 1), 0), cv::max(cv::min(y - 1, row - 1), 0)),
                        cv::Point(cv::max(cv::min(x - 1, col - 1), 0), cv::max(cv::min(y + 1, row - 1), 0)),
                        cv::Point(cv::max(cv::min(x - 1, col - 1), 0), cv::max(cv::min(y - 1, row - 1), 0)),*/
                    };
                    for (cv::Point p : neighb) {
                        if (img_seed.at<uchar>(p) > 0) continue;
                        if (abs(img_blur.at<uchar>(y, x) - img_blur.at<uchar>(p)) < grow_thold) {
                            img_seed.at<uchar>(p) = 255;
                        }
                    }
                }
            }
        }
    }
    if (verbose)cv::imshow("regGrow", img_seed);
    if (verbose)cv::waitKey(0);
    dst = img_seed.clone();
}


void lab5utils::regGrowing2(const cv::Mat& src, const std::vector<std::pair<int, int>>& seed,
                            cv:: Mat& dst, unsigned char anInValue, float tolerance) {

    cv::Mat visited_matrix = cv::Mat::zeros(cv::Size(src.cols, src.rows), CV_8UC1);
    std::vector<std::pair<int, int>> point_list = seed;

    while (!point_list.empty())
    {
        std::pair<int, int> this_point = point_list.back();
        point_list.pop_back();

        int x = this_point.first;
        int y = this_point.second;
        unsigned char pixel_value = src.at<unsigned char>(cv::Point(x, y));
        visited_matrix.at<unsigned char>(cv::Point(x, y)) = anInValue;
        for (int j = y - 1; j <= y + 1; ++j){
            if (0 <= j && j < src.rows){
                for (int i = x - 1; i <= x + 1; ++i){
                    if (0 <= i && i < src.cols){
                        unsigned char neighbour_value = src.at<unsigned char>(cv::Point(i, j));
                        unsigned char neighbour_visited = visited_matrix.at<unsigned char>(cv::Point(i, j));
                        if (!neighbour_visited && fabs(neighbour_value - pixel_value) <= (tolerance / 100.0 * 255.0)){
                            point_list.push_back(std::pair<int, int>(i, j));
                        }
                    }
                }
            }
        }
    }
    dst = visited_matrix.clone();
}


void lab5utils::clusterSegment(const cv::Mat& src, cv::Mat& dst, int k, int attempts, cv::Mat& indices) {
    cv::Mat img_blur, data, labels, centers, clusters;
    cv::GaussianBlur(src, img_blur, cv::Size(3, 3), 50);
    img_blur.convertTo(data, CV_32F);
    data = data.reshape(1, data.total());
    cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0);
    cv::kmeans(data, k, labels, criteria, attempts, cv::KMEANS_PP_CENTERS, centers);
    centers = centers.reshape(3, centers.rows);
    data = data.reshape(3, data.rows);
    cv::Vec3f* p = data.ptr<cv::Vec3f>();
    for (int i = 0; i < data.rows; i++) {
        int c_index = labels.at<int>(i);
        p[i] = centers.at<cv::Vec3f>(c_index);
    }
    clusters = data.reshape(3, src.rows);
    clusters.convertTo(clusters, CV_8U);
    dst = clusters.clone();
    labels = labels.reshape(0, src.rows);
    indices = labels.clone();
}