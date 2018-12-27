//
// Created by colin on 18-5-25.
//
#include <stdio.h>
#include <string>

class Shape
{
public:
    Shape()
    : tag("shape")
    {

    };

    virtual void Print() {
        printf("%s\n", tag.c_str());
    };

private:
    std::string tag;
};


class Circle: public Shape
{
public:
    Circle()
    : tag("Circle")
    {

    };

    void Print() {
        printf("%s\n", tag.c_str());
    };
private:
    std::string tag;
};


std::string Str()
{
    std::string xxx = "xxxxx";
    return xxx;
}

const char* GetStr() {
    return Str().c_str();
}

int main()
{
    Shape *shape1 = new Circle();
    shape1->Print();
    delete shape1;

    printf("%s \n", GetStr());
    return 0;
}

