/*算法tabuSearch实现说明*/ 

/* 
//编码方式（解的形式）：31城市用unsigned 0到30表示，一条路径用一个vector<unsigned>存储 
//适值函数：目标函数为根据路径计算的距离和，这里采用目标函数作为适值函数  
//初始解：这里选择了按顺序从编号为0走到编号为30的城市的路径为初始解 （没有尝试采用利用其他启发式算法得到初始解 ） 
//移动与邻域移动 ：每次移动为交换2个城市的编号顺序；由此产生的所有移动构成邻域 
//禁忌表：这里选择了适值函数作为禁忌对象；长度设定为22；禁忌表用vector<double>存储禁忌对象  
//特赦准则： 1，如果某个候选解优于历史最优解，可被解禁；2若是所有候选解都被禁忌，则选择候选集里的最优解解禁  
//选择策略：每次选择，首先，采用特赦准则1；其次，选择未被禁的最优解（也就是适值函数最小的候选解）；最后采用特赦准则2  
//终止准则： 这里采用最大迭代次数来决定是否终止算法，最大迭代次数设为1000 

*/ 

#include <iostream>
#include <vector>
#include <algorithm>
#include "City.h" 
#include "Can.h"


using namespace std;

using CU = const unsigned;

CU N = 31;// 城市个数
long double D[N][N];// 距离矩阵，用来记录两两城市之间的距离 
CU Ca_Max = 200; //最大候选集数目 200
CU Ca = 100; // 候选集数目 100 
CU TabuL = 22; //禁忌长度
CU G = 1000;//最大迭代次数 

vector<City> citys; //用来存储所有城市的位置  

//坐标数据  
const double postions[N*2] = {1304, 2312, 3639 ,1315, 4177 ,2244,
3712, 1399, 3488, 1535, 3326, 1556, 3238, 1229, 4196, 1004, 4312, 
790, 4386, 570, 3007, 1970, 2562, 1756, 2788, 1491, 2381, 1676, 1332,
695, 3715, 1678, 3918, 2179, 4061 ,2370, 3780 ,2212, 3676, 2578, 
4029, 2838, 4263, 2931, 3429, 1908, 3507, 2367, 3394 ,2643, 3439 ,
3201, 2935, 3240, 3140, 3550, 2545, 2357, 2778, 2826, 2370 ,2975};

Can bestSoFar; //最优解 
Can bestOfCans;// 某一候选集中最优解，也就是用于产生下一候选集的候选值  
vector<Can> cans; //候选集 
vector<double> tabuList;// 禁忌对象，这里选取目标函数（也就是路径距离值）作为禁忌对象 



void initializeCitys();//初始化城市的位置  

double computeDis(const City &c1, const City &c2);//根据位置计算两个城市之间的距离 

void initializeD();// 利用城市的位置来初始化距离平方矩阵D 

double computePath(vector<unsigned> &path); //适值函数，根据路径计算城市距离的和  

void initializeBest();// 初始化最优解 

void produceCans();//产生候选集 （也就是可移动的邻域） 

void produceBest();//选择策略，从候选集中选择最优解  

/*紧急搜索算法*/ 
void tabuSearch();//声明  


 
//用于比较两个候选集对象  
bool compare(Can &c1, Can &c2){
	return c1.distance < c2.distance;
}

/*初始化城市的位置*/
void initializeCitys(){
	for(unsigned i = 0; i < (N*2 - 1); i += 2){
		City tmp(postions[i],postions[i+1]);
		citys.push_back(tmp);
	}
}
 
/*利用城市的位置来初始化距离矩阵D*/
void initializeD(){ 
	for(unsigned i = 0; i< (N-1); ++i){
		for(unsigned j = i + 1; j < N ; ++j){
			D[i][j] = computeDis(citys[i], citys[j]);
			D[j][i] = D[i][j];
		}
	}
}

/*根据位置计算两个城市之间的距离*/ 
double computeDis(const City &c1, const City &c2){ 
	double res = 0;
	double X = c1.x - c2.x, Y = c1.y - c2.y;
//	res = X*X + Y*Y;
	res = sqrt(X*X + Y*Y); 
	return res;
}

/*根据路径计算城市之间的总距离*/
double computePath(vector<unsigned> &path) {
	double res = 0;
	unsigned inx1,inx2 = 0;  
	for(unsigned i = 0; i < (N-1) ; ++i){ 
		inx1 = path[i];
		inx2 = path[i+1]; 
		res += D[inx1][inx2]; 
	}
	inx1 = path[N-1];
	inx2 = path[0];
	res += D[inx1][inx2]; 
	return res;
	
}

/*初始化最优解*/ 
void initializeBest(){
	for(unsigned i = 0; i < N ; ++i)
		bestSoFar.path.push_back(i);
	bestSoFar.distance = computePath(bestSoFar.path);
	tabuList.push_back(bestSoFar.distance); 
	bestOfCans = bestSoFar;
}
 
/*产生候选集 */
void produceCans(){ 
	cans.clear();//清除上一轮候选集 
	 
	unsigned pos1 = 0, pos2 = 0;
	vector<unsigned> path(bestOfCans.path);
	double distance = 0; 
	
	vector<Can> cans0, cans1;
	//产生所有可能的2-opt候选集  
	for(pos1 = 0; pos1 < (N-1); ++pos1){
		for(pos2 = pos1+1; pos2 < N; ++pos2){
			path = bestOfCans.path;
			swap(path[pos1],path[pos2]);
			distance = computePath(path); 
			cans0.push_back(Can(path, distance));  
		} 
	} 
////	随机选择200个候选集 
//	for(unsigned i = 0; i < Ca_Max; ++i){ 
//		cans1.push_back(cans0[2*i]);
//	} 	
////	根据路径距离排序，产生100个候选集 
//	sort(cans1.begin(),cans1.end(),compare);
//	for(unsigned i = 0; i< Ca; ++i){
//		cans.push_back(cans1[i]);
//	} 

	//从所有邻域中产生200个候选集  
	sort(cans0.begin(),cans0.end(),compare);
	for(unsigned i = 0; i< Ca; ++i){
		cans.push_back(cans0[i]);
	} 
	
}

/*从候选集中选择最优解 */
void produceBest(){ 
	Can iniBest = cans[0]; //存储100个候选集中的最优解  
	
	// 根据禁忌对象选择剩余候选集的最优解  
	unsigned countTabu = 0; //用于计算被禁忌对象的个数   
	for(auto canObj: cans){ 
		//查找候选对象是否被禁忌  
 		auto res = find(tabuList.begin(), tabuList.end(), canObj.distance);  
		if(res == tabuList.end()){// 没有被禁忌  
			if(canObj.distance < bestSoFar.distance)  { 
				bestSoFar = bestOfCans;//更新历史最优解  
			} 
			bestOfCans = canObj; // 当前候选对象记为此次候选集的最优解 
 			tabuList.push_back(bestOfCans.distance);//更新禁忌对象表 
			if(tabuList.size() > TabuL){//禁忌表长度大于禁忌长度 
				tabuList.erase(tabuList.begin());//更新，删除第一个禁忌对象 
			} 
			break;
		}
		else{//被禁忌  
			++countTabu;
			if(canObj.distance < bestSoFar.distance ){//如果禁忌对象的目标值优于bestSoFar的目标值  
				//采用特赦原则，放出该禁忌对象  
				bestSoFar = canObj; 
				bestOfCans = canObj;
				//更新禁忌对象表 
				tabuList.erase(res);
 				tabuList.push_back(bestOfCans.distance);   
				break;
			}			
		}
	}  
	if(countTabu == cans.size() ){//候选集全部被禁忌  
		//采用特赦原则，放出最小的禁忌对象 
		if(iniBest.distance < bestSoFar.distance)  { 
			bestSoFar = iniBest;//更新历史最优解   
		}  
		bestOfCans = iniBest;
		//更新禁忌对象表 
 		auto res = find(tabuList.begin(), tabuList.end(), bestOfCans.distance); 
		tabuList.erase(res);
 		tabuList.push_back(bestOfCans.distance);  
	}
 
}

void tabuSearch(){
	/*初始化*/  
	initializeCitys();// 初始化城市位置  
	initializeD();//初始化距离矩阵  
	initializeBest();//初始化最优解  

	for(unsigned i = 0; i< G ; ++i){//在最大迭代次数范围内  
		produceCans();//产生候选集 
		produceBest();//从候选集中选择当前最优解  
	}
}

 


int main(int argc, char** argv) {
 	tabuSearch();
	cout<<"path: ";
	for(auto val : bestSoFar.path){
		cout<< val<<" ";
	}
	cout<<endl;
	
	cout<<"distance: ";
	double dis = bestSoFar.distance;
	cout<< dis <<endl; 
    
	return 0;
}
