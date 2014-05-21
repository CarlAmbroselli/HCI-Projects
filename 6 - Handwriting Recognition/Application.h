////////////////////////////////////////////////////////////////////////////////
//
// Basic class organizing the application
//
// Authors: Stephan Richter (2011) and Patrick Lühne (2012)
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __APPLICATION_H
#define __APPLICATION_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>

// Forward declarations
class DepthCamera;

////////////////////////////////////////////////////////////////////////////////
//
// Application
//
////////////////////////////////////////////////////////////////////////////////

class Application
{
	public:
		struct PointScore
		{
			float score;
			float value;
		};
		Application();
		~Application();

		void loop();

		////////////////////////////////////////////////////////////////////////
		//
		// To do: Insert your code here
		//
		////////////////////////////////////////////////////////////////////////
		void processFrame();

		void makeScreenshots();
		void clearOutputImage();
		float rateLine(cv::Point input[], cv::Mat testData, int rowNumber);
		int compareToTestData(cv::Point input[], cv::Mat testData, cv::Mat labels);
		cv::Point* matToRow(cv::Mat row, int rowNumber, cv::Point* rowArr);

		bool isFinished();
		// Sort Container by name function
		bool sortByScore(const PointScore &lhs, const PointScore &rhs) { return lhs.score < rhs.score; };

	protected:
		DepthCamera *m_depthCamera;

		cv::Mat m_rgbImage;
		cv::Mat m_depthImage;
		cv::Mat m_outputImage;
		cv::Mat m_drawedImage;
		cv::Mat m_initialImage;
		cv::Point m_initialDistance;
		cv::Point last_point;
		bool m_initialIsInitialized;
		int lastNumber;
		int distance;

		std::vector<cv::Point> touchPoints;
		void analyse(std::vector<cv::Point> touchPoints);

		bool m_isFinished;

		
};

#endif // __APPLICATION_H
