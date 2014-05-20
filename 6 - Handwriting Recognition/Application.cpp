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

	}

	for (  size_t i = 0; i < contours.size(); i++ ){
		double contourSize = contourArea(contours.at(i));
		if(contourSize > 40){
			cv::RotatedRect rect = cv::fitEllipse(contours.at(i));
			cv::ellipse(m_outputImage, rect, 255, 5);
			cv::circle(m_drawedImage, rect.center, 10, 255, 20);
			touchPoints.push_back(rect.center);
		}
	}

	///m_outputImage = m_outImage;
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
	cv::namedWindow("depth", 1);
	cv::namedWindow("raw", 1);
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

		case 'q':
			m_isFinished = true;
	}

	// Grab new images from the Kinect's cameras
	m_depthCamera->frameFromCamera(m_rgbImage, m_depthImage, CV_16UC1);

	// Process the current frame
	processFrame();

	// Display the images
	cv::imshow("raw", m_rgbImage);
	cv::imshow("depth", m_depthImage);
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
