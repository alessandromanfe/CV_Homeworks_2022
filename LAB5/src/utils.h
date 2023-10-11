#ifndef LAB5_UTILS
#define LAB5_UTILS

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace lab5utils{

    //Special thanks to @vinvinod on stack overflow for this function :)

    /**
     * @brief makeCanvas Makes composite image from the given images
     * @param vecMat Vector of Images.
     * @param windowHeight The height of the new composite image to be formed.
     * @param nRows Number of rows of images. (Number of columns will be calculated
     *              depending on the value of total number of images).
     * @return new composite image.
     */
    cv::Mat makeCanvas(std::vector<cv::Mat>& vecMat, int windowHeight, int nRows);

    /**
     *
     * 
     */
    void regGrowing(const cv::Mat& src, cv::Mat& dst, int bin_iter, int bin_thold, 
                    int grow_iter, int grow_thold, bool verbose = false);

    /**
     *
     *
     */
    void regGrowing2(const cv::Mat& src, const std::vector<std::pair<int, int>>& seed,
                     cv::Mat& dst, unsigned char anInValue = 255, float tolerance = 5);


    /**
     *  
     * 
     */
    void clusterSegment(const cv::Mat& src, cv::Mat& dst, int k, int attempts = 3,
                        cv::Mat& indices = cv::Mat() );

    
}

#endif // !LAB5_UTILS
