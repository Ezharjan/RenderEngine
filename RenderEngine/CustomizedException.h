#pragma once

#include<iostream>
#include<exception>

#ifndef __CustomizedException__
#define __CustomizedException__


namespace RenderEngine {

	class BaseException : public std::exception {
	public:
		BaseException() : message("Error happened!") {
			printf("%s",message);
		};
		BaseException(const std::string& msg) : message("Error happened: " + msg) {
			printf("%s",message);
		};

		// һ�������һ��Ҫ�ѱ��̳еĸ������������д���麯����
		virtual ~BaseException() throw() {	};

		virtual const char* detail() const throw() {
			// c_str���ص�ǰ�ַ������ַ���ַ����Ϊ����C���Լ��ݶ�ʹ�������ķ��ط�ʽ
			return message.c_str();
		}

	protected:
		std::string message;
	};

	class NullReferenceException : public BaseException
	{
	public:
		// �����ʼ��ʱ���Ǹ�������Ե�ʱ�����ڴ���������г�ʼ������Ҫͨ������ķ�ʽ��������Ҫ�������ڳ�ʼ��
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

	

	template<typename T>
	void CheckParam(T param) {
		if (param == NULL)
		{
			// ��arrΪNULLʱ�����Զ����쳣 
			throw NullReferenceException();
		}
	}
}

#endif // !__CustomizedException__
