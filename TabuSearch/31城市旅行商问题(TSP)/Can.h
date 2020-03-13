#ifndef CAN_H
#define CAN_H

#include <vector> 

using std::vector;
/*用来存储候选解*/ 
class Can{
public:
	Can(){
		distance = 0;
	}
	Can(vector<unsigned> &pa, double dis): path(pa), distance(dis){} 
	vector<unsigned> path;//存储移动路径
	double distance;  //存储适值函数值
};

#endif
