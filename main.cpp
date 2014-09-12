#include <string>   
#include <cv.h>
#include <highgui\highgui.hpp>
#include <opencv2\opencv.hpp>  
#include <opencv2\imgproc\imgproc.hpp>
#include <features2d\features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <queue>
#include <iostream>
#include <fstream>
#include <math.h>
#include <cmath>
#include <limits>
using namespace cv;
using namespace std;

#define N_FRAMES 50
//#define GPU
# define PI           3.14159265358979323846  /* pi */

static void drawOptFlowMap(const Mat& orientation,const Mat& magnitude, Mat& cflowmap, int step,
	double, const Scalar& color)
{
 	for(int y = 0; y < cflowmap.rows; y += step)
		for(int x = 0; x < cflowmap.cols; x += step)
		{
			const float& angle = orientation.at<float>(y, x);
			//const float& mag = magnitude.at<float>(y, x);
			const float& mag = 5;
			line(cflowmap, Point(x,y), Point(cvRound(x+mag*cos(angle)), cvRound(y+mag*sin(angle))),color);
			circle(cflowmap, Point(x,y), 1, color, -1);
		}
}

int modeEachRow(const Mat& input,float& mode)
{
	//Mat bins=Mat::zeros(input.rows,input.cols,CV_8U);
	int* bins=new int[input.cols];
	memset(bins,0,sizeof(int)*input.cols);
	int no_mode=0,mode_cnt=0,cnt=0,index=0;
	float num;
	for(int col=0;col<input.cols;col++)
	{
		cnt=0;
		num=input.at<float>(0,col);
		for(int i=0;i<input.cols;i++)
		{
			if(num==input.at<float>(0,i))
				cnt++;
		}
		bins[col]=cnt;
		if(mode_cnt<cnt)
		{
			mode_cnt=cnt;
			index=col;
		}
	}
	if( mode_cnt<=2)
		mode=0;
	mode=input.at<float>(0,index);

	return mode_cnt;

}

int main()   
{
	//image decelerations
	Mat showImg,curFrame,lastFrame,lastGrey,curGrey;

	VideoCapture video =VideoCapture("onionskin-1.MP4");
	VideoWriter videoOut;
	videoOut.open("major_flow.avi", CV_FOURCC('X','V','I','D'), video.get(CV_CAP_PROP_FPS), Size(video.get(CV_CAP_PROP_FRAME_WIDTH),video.get(CV_CAP_PROP_FRAME_HEIGHT)), true);
	if (!videoOut.isOpened())
	{
		cout  << "Could not open the output video for write! "  << endl;
		return -1;
	}
	video>>showImg;
	curFrame=showImg.clone();
	cv::cvtColor(curFrame,lastGrey,CV_BGR2GRAY);

	double displacementX=0, displacementY=0;
	int frameCounter=0;
	char key=0;
	while(key!=27)
	{
		video>>showImg;
		if (showImg.empty())
		{
			break;
		}
		curFrame=showImg.clone();
		cv::cvtColor(curFrame,curGrey,CV_BGR2GRAY);

	}

	waitKey();
	return 0;
}