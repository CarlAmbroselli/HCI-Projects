////////////////////////////////////////////////////////////////////////////////
//
// Basic class organizing the application
//
// Authors: Stephan Richter (2011) and Patrick Lühne (2012)
//
////////////////////////////////////////////////////////////////////////////////

#include "Application.h"

#include "DepthCamera.h"
#include "DepthCameraException.h"
#include <cmath>
#include "DataSet.h"
#include <algorithm>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
//
// Application
//
////////////////////////////////////////////////////////////////////////////////

void Application::processFrame()
{
	///////////////////////////////////////////////////////////////////////////
	//
	// To do:
	//
	// This method will be called every frame of the camera. Insert code here in
	// order to fulfill the assignment. These images will help you doing so:
	//
	// * m_rgbImage: The image of the Kinect's RGB camera
	// * m_depthImage: The image of the Kinects's depth sensor
	// * m_outputImage: The image in which you can draw the touch circles.
	//
	///////////////////////////////////////////////////////////////////////////

	// Sample code brightening up the depth image to make it visible
	m_depthImage *= 32;

	cv::Mat m_outImage = cv::Mat(480, 640, CV_8UC1);
	cv::Mat m_conturImage = cv::Mat(480, 640, CV_8UC1);
	m_depthImage.convertTo(m_outImage, CV_8UC1, 0.00390625);

	if(!m_initialIsInitialized){
		m_initialImage = m_outImage;
		m_initialIsInitialized = true;

		cv::rectangle(m_drawedImage, cv::Point(0, 0), cv::Point(640, 480),
				  cv::Scalar::all(0), CV_FILLED);
	}

	for( size_t i = 0; i < 480; i++ ) {
		for( size_t j = 0; j < 640; j++ ) {
			int distance = (int) m_initialImage.at<uchar>(i,j);
			if(m_outImage.at<uchar>(i,j) < distance-5 && m_outImage.at<uchar>(i,j) > (distance-8)){
			   m_outputImage.at<uchar>(i,j) = 255;
			} else {
			   m_outputImage.at<uchar>(i,j) = 0;
			}
		 }
	}

	std::vector<std::vector<cv::Point>> contours;
	m_conturImage = m_outputImage.clone();
	cv::findContours(m_outputImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point());

	for( size_t i = 0; i < 480; i++ ) {
		for( size_t j = 0; j < 640; j++ ) {
			   m_outputImage.at<uchar>(i,j) = 0;
		 }
	}

	if(contours.size() == 0){
		//Clear drawed image
		cv::rectangle(m_drawedImage, cv::Point(0, 0), cv::Point(640, 480),
				  cv::Scalar::all(0), CV_FILLED);

		//Clear touchPoints
		if(touchPoints.size() > 0){
			Application::analyse(touchPoints);
			touchPoints.clear();
		}

	}

	for (  size_t i = 0; i < contours.size(); i++ ){
		double contourSize = contourArea(contours.at(i));
		if(contourSize > 40){
			cv::RotatedRect rect = cv::fitEllipse(contours.at(i));
			cv::ellipse(m_outputImage, rect, 255, 5);
			cv::circle(m_drawedImage, rect.center, 5, 255, 10);
			touchPoints.push_back(rect.center);
		}
	}

	///m_outputImage = m_outImage;
}

////////////////////////////////////////////////////////////////////////////////


int Application::compareToTestData(cv::Point input[], cv::Mat testData, cv::Mat labels){
	std::vector<float> scores;
	cv::Point* pointRow;

	cv::Point rowArr[8];

	for(int i=0; i<testData.rows; i++){
		scores.push_back(rateLine(input, testData, i));
		if(i % 500 == 0)  printf("Score: %f\n", scores.at(scores.size()-1));
	}
	float minIndex = 300;
	float minValue = scores.at(300);
	std::vector<PointScore> scoreCon(7494);
	for(int j=0; j<scores.size(); j++){
		if(scores.at(j) < minValue){
			minIndex = j;
			minValue = scores.at(j);
		}
		scoreCon[j].score = scores.at(j);
		scoreCon[j].value = labels.at<float>(j);
	}
	//std::sort(scoreCon.begin(), scoreCon.end(), sortByScore);
	for (int j=0; j<scoreCon.size(); j++){
		printf("Score: %f, VAlue: %f", scoreCon.at(j).score, scoreCon.at(j).value);
	}

	return minIndex;
}

cv::Point* Application::matToRow(cv::Mat row, int rowNumber, cv::Point* rowArr){
	int j = 0;
	for(int i=0; i<8; i++){
		rowArr[i].x = (int) 100 * row.at<float>(rowNumber, j);
		j++;
		rowArr[i].y = (int) 100 * row.at<float>(rowNumber, j);
		j++;
	}
	return rowArr;
}

float Application::rateLine(cv::Point input[], cv::Mat testData, int rowNumber){
	float score = 0;
	int j = 0;
	float distance = 0;
	for(int i=0; i<8; i++){
		int distanceX = abs(input[i].x - testData.at<float>(rowNumber, j));
		j++;
		int distanceY = abs(input[i].y - testData.at<float>(rowNumber, j));
		j++;
		distance += (float) ((distanceX*distanceX)+(distanceY*distanceY));
	}

	score = sqrt((float) distance);
	return score;
}

void Application::analyse(std::vector<cv::Point> touchPoints)
{
	const int numberOfTouchPoints = 8;

	//Extract 8 touch points
	cv::Point touches[numberOfTouchPoints];
	float timeframe = touchPoints.size() / numberOfTouchPoints;
	cv::Point max = touchPoints.at(0);
	cv::Point min = touchPoints.at(0);
	for (int i = 0; i <numberOfTouchPoints; i++){
		touches[i] = touchPoints.at((int)i*timeframe);
		if(touches[i].x < min.x) min.x = touches[i].x;
		if(touches[i].y < min.y) min.y = touches[i].y;
		if(touches[i].x > max.x) max.x = touches[i].x;
		if(touches[i].y > max.y) max.y = touches[i].y;
		printf("Initial Touch %d : %d@%d\n", i, touches[i].x, touches[i].y);
	}

	printf("Max: %d@%d | Min: %d@%d\n", max.x, max.y, min.x, min.y);

	for (int i = 0; i < numberOfTouchPoints; i++){

		if(max.x != min.x) {
			float result = ((((touches[i].x - min.x)/((float)(max.x-min.x))) * 100));
			touches[i].x = (int) result; 
		}
		else touches[i].x = 0;

		if(max.y != min.y)
			touches[i].y = ((((touches[i].y - min.y)/((float)(max.y-min.y))) * 100));
		else touches[i].y = 0;

		printf("Touch %d : %d@%d\n", i, touches[i].x, touches[i].y);
	}
	printf("\n\n");

	cv::Mat data;
	cv::Mat labels;
	readDataSet("pendigits.tra", 7494, data, labels);

	int rowNumber = Application::compareToTestData(touches, data, labels);

	float number = labels.at<float>(rowNumber);
	printf("Recognized Number: %d", number);
}

////////////////////////////////////////////////////////////////////////////////

Application::Application()
{
	m_isFinished = false;

	try
	{
		m_depthCamera = new DepthCamera;
	}
	catch (DepthCameraException)
	{
		m_isFinished = true;
		return;
	}

    // open windows
	cv::namedWindow("output", 1);
	//cv::namedWindow("depth", 1);
	//cv::namedWindow("raw", 1);
	cv::namedWindow("draw", 1);

    // create work buffer
	m_rgbImage = cv::Mat(480, 640, CV_8UC3);
	m_depthImage = cv::Mat(480, 640, CV_16UC1);
	m_initialImage = cv::Mat(480, 640, CV_8UC1);
	m_outputImage = cv::Mat(480, 640, CV_8UC1);
	m_drawedImage = cv::Mat(480, 640, CV_8UC1);
	m_initialIsInitialized = false;
}

////////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
	if (m_depthCamera)
		delete m_depthCamera;
}

////////////////////////////////////////////////////////////////////////////////

void Application::loop()
{
	// Check for key input
	int key = cv::waitKey(20);

	switch (key)
	{
		case 's':
			makeScreenshots();
			break;

		case 'c':
			clearOutputImage();
			break;

		case 'r':
			m_initialIsInitialized = false;
			break;

		case 'q':
			m_isFinished = true;
	}

	// Grab new images from the Kinect's cameras
	m_depthCamera->frameFromCamera(m_rgbImage, m_depthImage, CV_16UC1);

	// Process the current frame
	processFrame();

	// Display the images
	//cv::imshow("raw", m_rgbImage);
	//cv::imshow("depth", m_depthImage);
	cv::imshow("output", m_outputImage);
	cv::imshow("draw", m_drawedImage);
}

////////////////////////////////////////////////////////////////////////////////

void Application::makeScreenshots()
{
	cv::imwrite("raw.png", m_rgbImage);
	cv::imwrite("depth.png", m_depthImage);
	cv::imwrite("output.png", m_outputImage);
}

////////////////////////////////////////////////////////////////////////////////

void Application::clearOutputImage()
{
	cv::rectangle(m_drawedImage, cv::Point(0, 0), cv::Point(640, 480),
				  cv::Scalar::all(0), CV_FILLED);
}

////////////////////////////////////////////////////////////////////////////////

bool Application::isFinished()
{
	return m_isFinished;
}
