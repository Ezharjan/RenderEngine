#pragma once

#include<exception>

namespace RenderEngine {

	class BaseException : public std::exception {
	public:
		BaseException() : message("Error happened!") {
			printf("%s",message.c_str());
		};
		BaseException(const std::string& msg) : message("Error happened: " + msg) {
			printf("%s",message.c_str());
		};

		virtual ~BaseException() throw() {	};

		virtual const char* detail() const throw() {
			return message.c_str();
		}

	protected:
		std::string message;
	};

	class NullReferenceException : public BaseException
	{
	public:
		NullReferenceException() :
			BaseException("Array is NULL, please check whether the array was potentially set to null!") {
		}
	};

	class FileNotFoundException : public BaseException
	{
	public:
		FileNotFoundException() :
			BaseException("File does not exist, please check the file path or file name!") {}
	};

	class FileFormatNotSupportedException : public BaseException
	{
	public:
		FileFormatNotSupportedException() :
			BaseException("File format not supported, please check your files.") {};
	};

}
