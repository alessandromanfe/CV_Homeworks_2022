#pragma once
#include <opencv2/highgui.hpp>

namespace filters {
	/**
	* Generates a filtered version of an image using a maxFilter of given size.
	* 
	* Takes a source image and applies for each pixel a max filtering with a square kernel of
	* dimension nxn where n is the integer passed as size parameter, and it saves the resulting 
	* image in the destination matrix. The filtering process updates the value of each pixel with
	* the maximum value of its neighbors inside the kernel centered in its position.
	* 
	* @param src Source image
	* @param dst Destination image ( Filtered image )
	* @param size Size of the kernel
	*/
	int maxFilter(cv::Mat& src, cv::Mat& dst, int size);

	/**
	* Generates a filtered version of an image using a minFilter of given size.
	*
	* Takes a source image and applies for each pixel a min filtering with a square kernel of
	* dimension nxn where n is the integer passed as size parameter, and it saves the resulting
	* image in the destination matrix. The filtering process updates the value of each pixel with
	* the minimum value of its neighbors inside the kernel centered in its position.
	*
	* @param src Source image
	* @param dst Destination image ( Filtered image )
	* @param size Size of the kernel
	*/
	int minFilter(cv::Mat& src, cv::Mat& dst, int size);
}
