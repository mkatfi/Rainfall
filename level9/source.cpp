#include <iostream>
#include <cstring>
#include <cstdlib>

class N {
public:
    int number;
    char annotation[100];

    N(int n)
    {
        number = n;
    }

    void setAnnotation(char *str)
    {
        size_t len = strlen(str);
        memcpy(annotation, str, len);   // vulnerable copy (no bounds check)
    }

    virtual int operator+(N &other)
    {
        return number + other.number;
    }

    virtual int operator-(N &other)
    {
        return number - other.number;
    }
};

int main(int argc, char **argv)
{
    if (argc < 2)
        exit(1);

    N *obj1 = new N(5);
    N *obj2 = new N(6);

    obj1->setAnnotation(argv[1]);

    return (*obj2 + *obj1);
}