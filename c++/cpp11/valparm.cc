#include <stdio.h>
#include <iostream>  
using namespace std;  

template <typename T>  
void func(const T& t)  
{  
	cout << "func "<< t << endl;  
}  

template <typename T, typename ...Args> //Args:模板参数包  
void func(const T& t, Args ...args)      //args:函数参数包  
{  
	cout << "t= " << t << endl;  
	cout << "sizeof...(Args) = " << sizeof...(Args) << endl;
	//func(args...);  
}  

template <typename ...Args> //Args:模板参数包  
void func2(Args ...args)      //args:函数参数包  
{
	cout << "sizeof...(Args) = " << sizeof...(Args) << endl;
	//cout << " Args = " << args... << endl;  
}

template <typename... TS>   // typename... TS为模板形参包，TS为模式
static void MyPrint(const char* s, TS... args)  // TS... args为函数形参包，args为模式
{
    printf(s, args...);
}

int main()  
{     
	func(1, 2, 3, 4, 5, "hello world", "x");  
	func2(1, 2, 3, 4, 5, "hello world", "x");  

	MyPrint("test %d \n", 1);

	return 0;  
}  

