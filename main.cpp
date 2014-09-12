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
	ofstream myfile;
	myfile.open ("major flow.txt");

	Mat input,redE,greenE,blueE;
	Mat temp,gray,grayE;
	Mat gradX,gradY,Magnitude,Orientation, mag8bit;
	int kernelSize=7;
	
	double minv=0,maxv=0;
	Point minLoc,maxLoc;
	vector<Mat> BGR;
	input=imread("motion4.tiff",1);
	split(input,BGR);
	cv::cvtColor(input,gray,CV_BGR2GRAY);
	Sobel(gray,gradX,CV_32F,1,0,kernelSize);
	Sobel(gray,gradY,CV_32F,0,1,kernelSize);
	Magnitude=Mat::zeros(gray.rows,gray.cols,CV_32F);
	Orientation=Mat::zeros(gray.rows,gray.cols,CV_32F);
	vector<float> aveMag(gray.rows), aveOri(gray.rows);

	Scalar average;
	//double average=0;
	double motionDirection=0;
	minMaxLoc(gradX,&minv,&maxv,&minLoc,&maxLoc,Mat());
	cout<<"min value: "<<minv<<endl<<"max value: "<<maxv<<endl;
	minMaxLoc(gradY,&minv,&maxv,&minLoc,&maxLoc,Mat());
	cout<<"min value: "<<minv<<endl<<"max value: "<<maxv<<endl;

	double angle=0,magnit=0;
	double thresh=4000;
	double valX=0,valY=0;
	for(int row=0;row<gradX.rows;row++)
	{
		double magSum=0,oriSum=0;
		int oriCnt=0;
		for(int col=0;col<gradX.cols;col++)
		{

			valX=gradX.at<float>(row,col);
			valY=gradY.at<float>(row,col);

			if(valX==0)
			{
				angle=valY;
			}
			else
			{
				angle=atan2(valY,valX);				
			}
			while(angle>PI)
			{
				angle-=2*PI;
			}
			while(angle<-PI)
			{
				angle+=2*PI;
			}

							
			if((angle>(-PI/2))&&(angle<PI/2))
			{
				angle+=PI/2;
			}
			else if((angle>PI/2)&&(angle<PI))
			{
				angle-=PI/2;
			}
			else if((angle>-PI)&&(angle<(-PI/2)))
			{
				angle+=(3*PI)/2;
			}

			magnit=sqrt((valY*valY)+(valX*valX));
			Orientation.at<float>(row,col)=angle;
			Magnitude.at<float>(row,col)=magnit;
			//magSum+=magnit;
			//if(angle!=PI/2 && angle!=(-PI/2))
			//{
			//	oriSum+=angle;
			//	oriCnt++;
			//}
			if(magnit>thresh)
			{
				oriSum+=angle;
				oriCnt++;
			}
			
		}
		//average=magSum/gradX.cols;
		//aveMag[row]=average;
		//aveMag[row]=magSum/gradX.cols;

		//Mat OriSorted;
		//cv::sort(Orientation(Range(row,row+1),Range::all()),OriSorted,CV_SORT_EVERY_ROW);
		//// Middle or average of middle values in the sorted array.
		//double dMedian = 0.0;
		//if ((Orientation.cols % 2) == 0) {
		//	dMedian = (OriSorted.at<float>(0,OriSorted.cols/2) + OriSorted.at<float>(0,OriSorted.cols/2-1))/2.0;
		//} else {
		//	dMedian = OriSorted.at<float>(0,OriSorted.rows/2);
		//}
		int mode_cnt;
		//mode_cnt=modeEachRow(Orientation(Range(row,row+1),Range::all()),aveOri[row]);
		//if ((aveOri[row]<PI/2)&&(aveOri[row]>(-PI/2)))
		//	int x=0;//cout<<"low cnt "<<mode_cnt<<endl;
		//else 
		//	cout<<"high cnt "<<mode_cnt<<endl;
		average=oriSum/oriCnt;
		aveOri[row]=average[0];
		//aveOri[row]=dMedian;


		//average=mean(Magnitude(Range(row,row+1),Range::all()));
		//if((average[0]-aveMag[row])!=0)
		//	cout<<average[0]-aveMag[row]<<endl;
		//aveMag[row]=(double)average[0];
		//average=mean(Orientation(Range(row,row+1),Range::all()));
		//if((average[0]-aveOri[row])!=0)
		//	cout<<average[0]-aveOri[row]<<endl;
		//aveOri[row]=(double)average[0];
	}
	Magnitude.convertTo(mag8bit,CV_8U);
	//minMaxLoc(aveMag,&minv,&maxv,&minLoc,&maxLoc,Mat());
	//cout<<"min value: "<<minv<<endl<<"max value: "<<maxv<<endl;
	//minMaxLoc(aveOri,&minv,&maxv,&minLoc,&maxLoc,Mat());
	//cout<<"min value: "<<minv<<endl<<"max value: "<<maxv<<endl;


	

	//drawOptFlowMap(Orientation,Magnitude,input,8, 1.5, CV_RGB(0, 255, 0));
	Point start,end;
	//Point start=Point(input.cols/2,input.rows/2);
	//double ang=4;

	//Point end=Point(start.x+30*cos(ang),start.y+30*sin(ang));
	//line(input,start,end,Scalar(0,0,255),1);
	//circle(input, start, 1, Scalar(0,0,255), 1);


	/*
	for(int i=0;i<input.rows-1;i+=5)
	{
		int x=0,y=0;
		double one=aveOri[i],two=aveMag[i];
		//two=(two/10);
		double cos_one=cos(one);
		//x=cos(aveOri[i])*aveMag[i];
		x=two*cos(one);
		y=two*sin(one);
		//start=Point(input.cols/2,input.rows-i-1);
		end=Point(start.x+x,start.y+y);
		//line(input,start,end,Scalar(255,0,0),1);
		//circle(input, start, 1, Scalar(255,0,0), 1);
		//start=end;
		start=Point(input.cols/2,i+1);
		//imshow("gray",input);
		//waitKey();
	}
	*/
	//drawing a bar chart
	
	for(int i=0;i<input.rows-1;i+=1)
	{
		int x;
		//double mag=40;
		//int y;
		//if(tan(aveOri[i])<0)
		//	cout<<tan(aveOri[i])<<endl;
		//if(i==582)
		//	cout<<"het";
		double ang=aveOri[i];
		//y=mag*sin(ang);
		//x=mag*cos(ang);
		
		double tangent=(1/tan(ang));
		if(tangent>2000 ||tangent==std::numeric_limits<double>::infinity())
			tangent=0;
		if(tangent<-2000)
			tangent=0;
		if(tangent>=0)
		{
			x=(int)(tangent+0.5);
		}
		else
		{
			x=(int)(tangent-0.5);
		}
		
		//x=(int)((1/tan(aveOri[i])));
		//myfile<<x<<" , "<<tangent<<endl;
		myfile<<tangent<<endl;
		//myfile<<x<<endl;
		x*=10;
		
		start=Point(input.cols/2,i);
		end=Point(start.x+x,i+1);
		//start=Point(input.cols/2,i);
		//end=Point(start.x+x,start.y+y);		
		//if(x==0)
		//{
		//	
		//	string text=to_string((long double)(i));
		//	cout<<text<<endl;
		//	putText(input,text,Point(start.x+rand()%340+20,start.y),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,255));
		//	//imshow("gray",input);
		//	//waitKey();
		//}
		rectangle(input,start,end,Scalar(255,0,0),1);
		//line(input,start,end,Scalar(255,0,0),1);
		//imshow("gray",input);
		//waitKey();
	}
	
	imshow("gray",input);
	imwrite("flowmap.tiff",input);
	myfile.close();
	//Canny(BGR[0],BlueE,5,45);
	//Canny(BGR[1],GreenE,5,45);
	//Canny(BGR[2],RedE,5,45);

	//imshow("blue",BlueE);
	//imshow("green",GreenE);
	//imshow("red",RedE);
	//imwrite("red_edge.tiff",RedE);

	waitKey();
	return 0;
}