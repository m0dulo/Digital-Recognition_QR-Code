#include <stdio.h>
#include "stdafx.h"
#include <iostream>   
#include <opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <sstream>
using namespace cv;
using namespace std;
int kSize = 5;

struct matchResult
{
	Point2f pt;
	double score;
};

vector<Mat> getImgs()
{
	vector<Mat> output;
	for (int i = 0; i < 5; i++)
	{
		stringstream ss;
		ss << "source"<<(i + 1) << ".jpg";
		string filename;
		ss >> filename;
		output.push_back(imread(filename, 0));
	}
	//cout << output.size() << endl;
	return output;
}

vector<Mat> getTemps()
{
	vector<Mat> output;
	for (int i = 0; i < 5; i++)
	{
		stringstream ss;
		ss << "template" << (i + 1) << ".jpg";
		string filename;
		ss >> filename;
		output.push_back(imread(filename, 0));
	}
	//cout << output.size() << endl;
	return output;
}

void detectText(Mat img, vector<Mat> temps)
{
	imshow("img", img);
	Mat element = getStructuringElement(MORPH_RECT, Size(kSize, kSize));
	Mat src = img.clone();
	threshold(src, src, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	dilate(src, src, element, Point(-1, -1), 2);
	imshow("src", src);

	vector< vector< Point> > contours;                 //连通域点
													   //连通域检测擦除<某范围的块
	findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	vector<vector<Point> >::iterator itc = contours.begin();
	while (itc != contours.end())//擦出小区快
	{
		if (itc->size()<200)
		{
			itc = contours.erase(itc);
		}
		else
		{
			++itc;
		}
	}

	int flags = 0;
	int index = 0;
	double maximum = 0;
	vector<matchResult> results;

	for (int i = 0; i < 5; i++)
	{
		Mat R;
		Mat temp = temps.at(i);
		double maxVal;
		Point maxLoc;
		matchTemplate(src, temp, R, cv::TM_CCOEFF_NORMED);
		minMaxLoc(R, NULL, &maxVal, NULL, &maxLoc);
		cout << maxVal << endl;
		if (maxVal > 0.4)
		{
			flags = flags + 1;
		}
		matchResult result;
		{
			result.pt = maxLoc;
			result.score = maxVal;
		}
		results.push_back(result);
	}

	for (int i = 0; i < 5; i++)
	{
		if (results.at(i).score >= maximum)
		{
			maximum = results.at(i).score;
			index = i;
		}
	}

	string detectedNumber;

	if (results.at(1).score > 0.4 && results.at(2).score > 0.4)
	{
		detectedNumber = "4";
	}
	else
	{
		switch (index)
		{
		case 0: detectedNumber = "0"; break;
		case 1: detectedNumber = "1"; break;
		case 3: detectedNumber = "6"; break;
		case 4: detectedNumber = "8"; break;
		}

	}

	string displayText = "The detected Number is " + detectedNumber;
	cvtColor(img, img, COLOR_GRAY2BGR);
	putText(img, displayText, Point(1, 40), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255));
	imshow(detectedNumber, img);


}

int main()
{
	vector<Mat> images = getImgs();
	vector<Mat> templates = getTemps();
	//detectText(images.at(3), templates);
	for (int i = 0; i < 5; i++)
	{
		detectText(images.at(i), templates);
	}

	waitKey(0);
	return 0;
}
