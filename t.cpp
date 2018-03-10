#include <iostream>
int main()
{
int x=0,y=0;
if (x++ && y++)
std::cout <<x<<' '<<y;
else std::cout <<"no " << x<< " "<<y<< "\n";
return 0;
}
