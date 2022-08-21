#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <vector>
#include "tinyxml2.h"
#include "const.h"
#include "structs.h"
#include <boost/unordered_map.hpp>

class Map
{
private:
    std::vector<std::vector<int>> grid;
    std::vector<gNode> nodes;
    std::vector<std::vector<Node>> valid_moves;
    int  height, width, size;
    int  connectedness;
    double agent_size;
    bool map_is_roadmap;
    bool check_line(int x1, int y1, int x2, int y2);
    bool get_grid(const char* FileName);
    bool get_roadmap(const char* FileName);
	int nodes_num; //public node limit
	int init_node_num;
	void prt_nodes();
	typedef std::pair<std::pair<double,double>,int> node_index;
	typedef boost::unordered_map<node_index,int> hast_table;
	hast_table nodes_table;
	
public:
    Map(double size, int k){ agent_size = size; connectedness = k; }
	Map(Map *m);
    ~Map(){}
    int  get_size() const { return size; }
    bool get_map(const char* FileName);
    bool is_roadmap() const {return map_is_roadmap;}
    bool cell_is_obstacle(int i, int j) const;
    int  get_width() const {return width;}
    gNode get_gNode(int id) const {if(id < int(nodes.size())) return nodes[id]; return gNode();}
    int  get_id(int i, int j) const;
    double get_i (int id) const;
    double get_j (int id) const;
	int add_node(double i, double j, int node1, int node2,int agent);
    std::vector<Node> get_valid_moves(int id) const;
	double fit2grid(double val){return round(val/CN_RESOLUTION)*CN_RESOLUTION;}
    void print_map();
    void printPPM();
	void prt_ind(node_index n);
	void prt_validmoves();
	void alter(Map_delta map_delta);
	void alter_back(Map_delta map_delta);
	bool equal(Map *m);
};

#endif // MAP_H
