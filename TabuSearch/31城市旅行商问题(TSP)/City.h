#ifndef CITY_H
#define CITY_H

/*用来存储城市的位置*/ 
class City{
public:
	City():x(0),y(0){}
	City(double X, double Y):x(X),y(Y){}
	double x;
	double y;
};

#endif
