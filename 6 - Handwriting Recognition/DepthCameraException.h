////////////////////////////////////////////////////////////////////////////////
//
// An exception occurring when the depth camera does not work properly
//
// Author: Patrick Lühne (2012)
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __DEPTH_CAMERA_EXCEPTION_H
#define __DEPTH_CAMERA_EXCEPTION_H

#include <string>
#include <exception>

////////////////////////////////////////////////////////////////////////////////
//
// DepthCameraException
//
////////////////////////////////////////////////////////////////////////////////

class DepthCameraException : public std::exception
{
	public:
		DepthCameraException(std::string message) throw();
		virtual ~DepthCameraException() throw();

		virtual const char* what() const throw();

	protected:
		std::string m_message;
};

#endif
