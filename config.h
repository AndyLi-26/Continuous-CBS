#ifndef CONFIG_H
#define CONFIG_H
#include "tinyxml2.h"
#include "const.h"
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>
using namespace std;
class Config
{
public:
    Config();
    void getConfig(const char* fileName);
    double  precision;
    double  focal_weight;
    bool    use_cardinal;
    bool    use_disjoint_splitting;
    bool    use_edge_split;
    int     hlh_type;
    int     connectdness;
    double  agent_size;
    double  timelimit;
	int 	agent_num;
	//double 	resolution;
	string output;
};

#endif // CONFIG_H
