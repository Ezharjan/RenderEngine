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

		// 一般情况下一定要把被继承的父类的析构函数写成虚函数！
		virtual ~BaseException() throw() {	};

		virtual const char* detail() const throw() {
			// c_str返回当前字符串首字符地址，仅为了与C语言兼容而使用这样的返回方式
			return message.c_str();
		}

	protected:
		std::string message;
	};

	class NullReferenceException : public BaseException
	{
	public:
		// 子类初始化时覆盖父类的属性的时候若在大括号外进行初始化就需要通过下面的方式，否则需要在括号内初始化
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
			// 当arr为NULL时返回自定义异常 
			throw NullReferenceException();
		}
	}
}

#endif // !__CustomizedException__
