#include "map.h"
Map::Map(Map *m) {
	grid=m->grid;
	nodes=m->nodes;
	height=m->height;
	width=m->width;
	size=m->size;
	init_node_num=m->init_node_num;
	for (int i=0;i<m->valid_moves.size();++i){
		std::vector<Node> neighbors;
		for (int j=0;j<m->valid_moves[i].size();++j){
			Node new_node;
			new_node.id=m->valid_moves[i][j].id;
			new_node.i=m->valid_moves[i][j].i;
			new_node.j=m->valid_moves[i][j].j;
			neighbors.push_back(new_node);
		}
		valid_moves.push_back(neighbors);
	}
	
	for (gNode original:m->nodes){
		gNode temp;
		temp.i=original.i;
		temp.j=original.j;
		nodes.push_back(temp);
	}
	
}

bool Map::equal(Map *m){
	if (grid!=m->grid)
		return false;
	if (height!=m->height)
		return false;
	if (width!=m->width)
		return false;
	if (init_node_num!=m->init_node_num)
		return false;
	for (int i=0;i<m->valid_moves.size();++i){
		for (int j=0;j<m->valid_moves[i].size();++j){
			if (valid_moves[i][j].id!=m->valid_moves[i][j].id ||
			valid_moves[i][j].i!=m->valid_moves[i][j].i ||
			valid_moves[i][j].j!=m->valid_moves[i][j].j ){
				std::cout<<"this:"<<std::endl;
				prt_validmoves();
				std::cout<<"ori"<<std::endl;
				m->prt_validmoves();
				return false;
			}
		}
	}
	return true;
}

bool Map::get_map(const char* FileName)
{
    tinyxml2::XMLElement *root = nullptr;
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS)
    {
        std::cout << "Error opening MAP XML file!" << std::endl;
        return false;
    }
    root = doc.FirstChildElement(CNS_TAG_ROOT);
    if (root)
    {
        map_is_roadmap = false;
        return get_grid(FileName);
    }
    else
    {
        map_is_roadmap = true;
        return get_roadmap(FileName);
    }
}

double Map::get_i(int id) const
{
    if(!map_is_roadmap)
        return int(id/width);
    else
        return nodes[id].i;
}

double Map::get_j(int id) const
{
    if(!map_is_roadmap)
        return int(id%width);
    else
        return nodes[id].j;
}

bool Map::get_grid(const char* FileName)
{

    tinyxml2::XMLElement *root = nullptr, *map = nullptr, *element = nullptr, *mapnode = nullptr;

    std::string value;
    std::stringstream stream;
    bool hasGridMem(false), hasGrid(false), hasHeight(false), hasWidth(false);

    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS)
    {
        std::cout << "Error opening XML file!" << std::endl;
        return false;
    }
    root = doc.FirstChildElement(CNS_TAG_ROOT);
    if (!root)
    {
        std::cout << "Error! No '" << CNS_TAG_ROOT << "' tag found in XML file!" << std::endl;
        return false;
    }
    map = root->FirstChildElement(CNS_TAG_MAP);
    if (!map)
    {
        std::cout << "Error! No '" << CNS_TAG_MAP << "' tag found in XML file!" << std::endl;
        return false;
    }

    for (mapnode = map->FirstChildElement(); mapnode; mapnode = mapnode->NextSiblingElement())
    {
        element = mapnode->ToElement();
        value = mapnode->Value();
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);

        stream.str("");
        stream.clear();
        stream << element->GetText();

        if (!hasGridMem && hasHeight && hasWidth)
        {
            grid.resize(height);
            for (int i = 0; i < height; ++i)
                grid[i].resize(width);
            hasGridMem = true;
        }

        if (value == CNS_TAG_HEIGHT)
        {
            if (hasHeight)
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_HEIGHT << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_HEIGHT << "' =" << height << "will be used."
                          << std::endl;
            }
            else
            {
                if (!((stream >> height) && (height > 0)))
                {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_HEIGHT
                              << "' tag encountered (or could not convert to integer)." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_HEIGHT << "' tag should be an integer >=0" << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_HEIGHT
                              << "' tag will be encountered later..." << std::endl;
                }
                else
                    hasHeight = true;
            }
        }
        else if (value == CNS_TAG_WIDTH)
        {
            if (hasWidth)
            {
                std::cout << "Warning! Duplicate '" << CNS_TAG_WIDTH << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_WIDTH << "' =" << width << "will be used." << std::endl;
            }
            else
            {
                if (!((stream >> width) && (width > 0)))
                {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_WIDTH
                              << "' tag encountered (or could not convert to integer)." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_WIDTH << "' tag should be an integer AND >0" << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_WIDTH
                              << "' tag will be encountered later..." << std::endl;

                }
                else
                    hasWidth = true;
            }
        }
        else if (value == CNS_TAG_GRID)
        {
            int grid_i(0), grid_j(0);
            hasGrid = true;
            if (!(hasHeight && hasWidth))
            {
                std::cout << "Error! No '" << CNS_TAG_WIDTH << "' tag or '" << CNS_TAG_HEIGHT << "' tag before '"
                          << CNS_TAG_GRID << "'tag encountered!" << std::endl;
                return false;
            }
            element = mapnode->FirstChildElement();
            while (grid_i < height)
            {
                if (!element)
                {
                    std::cout << "Error! Not enough '" << CNS_TAG_ROW << "' tags inside '" << CNS_TAG_GRID << "' tag."
                              << std::endl;
                    std::cout << "Number of '" << CNS_TAG_ROW
                              << "' tags should be equal (or greater) than the value of '" << CNS_TAG_HEIGHT
                              << "' tag which is " << height << std::endl;
                    return false;
                }
                std::string str = element->GetText();
                std::vector<std::string> elems;
                std::stringstream ss(str);
                std::string item;
                while (std::getline(ss, item, ' '))
                    elems.push_back(item);
                grid_j = 0;
                int val;
                if (elems.size() > 0)
                    for (grid_j = 0; grid_j < width; ++grid_j)
                    {
                        if (grid_j == int(elems.size()))
                            break;
                        stream.str("");
                        stream.clear();
                        stream << elems[grid_j];
                        stream >> val;
                        grid[grid_i][grid_j] = val;
                    }

                if (grid_j != width)
                {
                    std::cout << "Invalid value on " << CNS_TAG_GRID << " in the " << grid_i + 1 << " " << CNS_TAG_ROW
                              << std::endl;
                    return false;
                }
                ++grid_i;
                element = element->NextSiblingElement();
            }
        }
    }
    if (!hasGrid) {
        std::cout << "Error! There is no tag 'grid' in xml-file!\n";
        return false;
    }
    size = width*height;
    std::vector<Step> moves;
    valid_moves.resize(height*width);
    if(connectedness == 2)
        moves = {{0,1}, {1,0}, {-1,0},  {0,-1}};
    else if(connectedness == 3)
        moves = {{0,1}, {1,1}, {1,0},  {1,-1},  {0,-1},  {-1,-1}, {-1,0}, {-1,1}};
    else if(connectedness == 4)
        moves = {{0,1}, {1,1}, {1,0},  {1,-1},  {0,-1},  {-1,-1}, {-1,0}, {-1,1},
                 {1,2}, {2,1}, {2,-1}, {1,-2}, {-1,-2}, {-2,-1}, {-2,1},  {-1,2}};
    else
        moves = {{0,1},   {1,1},   {1,0},   {1,-1},  {0,-1},  {-1,-1}, {-1,0}, {-1,1},
                 {1,2},   {2,1},   {2,-1},  {1,-2},  {-1,-2}, {-2,-1}, {-2,1}, {-1,2},
                 {1,3},   {2,3},   {3,2},   {3,1},   {3,-1},  {3,-2},  {2,-3}, {1,-3},
                 {-1,-3}, {-2,-3}, {-3,-2}, {-3,-1}, {-3,1},  {-3,2},  {-2,3}, {-1,3}};
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
        {
            std::vector<bool> valid(moves.size(), true);
            for(unsigned int k = 0; k < moves.size(); k++)
                if((i + moves[k].i) < 0 || (i + moves[k].i) >= height || (j + moves[k].j) < 0 || (j + moves[k].j) >= width
                        || cell_is_obstacle(i + moves[k].i, j + moves[k].j)
                        || !check_line(i, j, i + moves[k].i, j + moves[k].j))
                    valid[k] = false;
            std::vector<Node> v_moves = {};
            for(unsigned int k = 0; k < valid.size(); k++)
                if(valid[k])
                    v_moves.push_back(Node((i + moves[k].i)*width + moves[k].j + j, 0, 0, i + moves[k].i, j + moves[k].j));
            valid_moves[i*width+j] = v_moves;
        }
    return true;
}

bool Map::get_roadmap(const char *FileName)
{
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS)
    {
        std::cout << "Error opening XML file!" << std::endl;
        return false;
    }
    tinyxml2::XMLElement *root = 0, *element = 0, *data;
    std::string value;
    std::stringstream stream;
    root = doc.FirstChildElement("graphml")->FirstChildElement("graph");
    for(element = root->FirstChildElement("node"); element; element = element->NextSiblingElement("node"))
    {
        data = element->FirstChildElement();

        stream.str("");
        stream.clear();
        stream << data->GetText();
        stream >> value;
        auto it = value.find_first_of(",");
        stream.str("");
        stream.clear();
        stream << value.substr(0, it);
        double i;
        stream >> i;
		i=fit2grid(i);
        stream.str("");
        stream.clear();
        value.erase(0, ++it);
        stream << value;
        double j;
        stream >> j;
		j=fit2grid(j);
        gNode node(i,j);
		node.agent.insert(-1);
        nodes.push_back(node);
		//nodes_table.insert(n);
		node_index ind(std::make_pair(i,j));
		nodes_table[ind]=nodes.size()-1;
    }
	//prt_nodes();
	nodes_num=nodes.size();
	init_node_num=nodes_num;
	
    for(element = root->FirstChildElement("edge"); element; element = element->NextSiblingElement("edge"))
    {
        std::string source = std::string(element->Attribute("source"));
        std::string target = std::string(element->Attribute("target"));
        source.erase(source.begin(),++source.begin());
        target.erase(target.begin(),++target.begin());
        int id1, id2;
        stream.str("");
        stream.clear();
        stream << source;
        stream >> id1;
        stream.str("");
        stream.clear();
        stream << target;
        stream >> id2;
        nodes[id1].neighbors.push_back(id2);
    }
    for(gNode cur:nodes)
    {
        Node node;
        std::vector<Node> neighbors;
        neighbors.clear();
        for(unsigned int i = 0; i < cur.neighbors.size(); i++)
        {
            node.i = nodes[cur.neighbors[i]].i;
            node.j = nodes[cur.neighbors[i]].j;
            node.id = cur.neighbors[i];
            neighbors.push_back(node);
        }
        valid_moves.push_back(neighbors);
    }
    size = int(nodes.size());
    return true;
}

int Map::add_node(double i, double j, int node1, int node2,int agent)
{
  //std::cout<<"new agent:"<<agent<<std::endl;
  hast_table::iterator it;
  node_index ind(std::make_pair(i,j));
  //check if exist
  it = nodes_table.find(ind);
  //std::cout<<std::endl<<std::endl<<"info"<<std::endl;
  //prt_ind(ind);
  //std::cout<<"("<<ind.first.first<<","<<ind.first.second<<") a:"<<ind.second<<std::endl;
  //prt_nodes();
  int node_id;
  if (it != nodes_table.end())
  {
    node_id=it->second;
    if (node_id== node1 || node_id==node2)
      return -1;
    if (nodes[node_id].agent.find(-1)!=nodes[node_id].agent.end() || nodes[node_id].agent.find(agent)!=nodes[node_id].agent.end())
      return -1;
    else{
      nodes[node_id].agent.insert(agent);

      bool n1=false,n2=false;
      for (Node n:valid_moves[node_id]){
        if(n1 && n2) break;
        if (n.id==node1) n1=true;
        if (n.id==node2) n2=true;
      }
      if (!n1){
        Node valid1;
        valid1.i = nodes[node1].i;
        valid1.j = nodes[node1].j;
        valid1.id = node1;
        valid1.agent.insert(-1);
        valid_moves[node_id].push_back(valid1);
      }
      if (!n2){
        Node valid2;
        valid2.i = nodes[node2].i;
        valid2.j = nodes[node2].j;
        valid2.id = node2;
        valid2.agent.insert(-1);
        valid_moves[node_id].push_back(valid2);
      }

    }
  }
  else{
    node_id=nodes.size();
    gNode tempnode(i,j);
    nodes.push_back(tempnode);
    this->size+=1;
    nodes[node_id].agent.insert(agent);
    nodes_table[ind]=node_id;

    std::vector<Node> neighbors;
    Node valid1;
    valid1.i = nodes[node1].i;
    valid1.j = nodes[node1].j;
    valid1.id = node1;
    valid1.agent.insert(-1);
    neighbors.push_back(valid1);
    Node valid2;
    valid2.i = nodes[node2].i;
    valid2.j = nodes[node2].j;
    valid2.agent.insert(-1);
    valid2.id = node2;
    neighbors.push_back(valid2);
    valid_moves.push_back(neighbors);
  }
  //add nodes to node list
  nodes[node_id].neighbors.push_back(node1);
  nodes[node_id].neighbors.push_back(node2);
  nodes[node1].neighbors.push_back(node_id);
  nodes[node2].neighbors.push_back(node_id);

  //add node to hash_table


  //add edges
  //Node node;
  //node.i=i;
  //node.j=j;
  //node.id=nodeid;

  //break up the original edge
  /*
     for (int n=0;n<valid_moves[node1].size();++n){
     if (valid_moves[node1][n].id==node2){
     valid_moves[node1][n].negtive_list.push_back(agent);
     }
     }

     for (int n=0;n<valid_moves[node2].size();++n){
     if (valid_moves[node2][n].id==node1){
     valid_moves[node2][n].negtive_list.push_back(agent);
     }
     }

	
	for (int n=0;n<valid_moves[node2].size();++n){
		if (valid_moves[node2][n].id==node1){
			valid_moves[node2].erase(valid_moves[node2].begin()+n);
			break;
		}
	}
	*/
	//valid_moves[node1].push_back(node);
	//valid_moves[node2].push_back(node);

	
	return node_id;
}

void Map::print_map()
{
    std::cout<<height<<"x"<<width<<std::endl;
    for(int i = 0; i < height; i++)
    {
        std::cout<<"<row>";
        for(int j = 0; j < width; j++)
            std::cout<<grid[i][j]<<" ";
        std::cout<<"</row>"<<std::endl;
    }
}

void Map::printPPM()
{
    std::cout<<"P3\n"<<width<<" "<<height<<"\n255\n";
    for(int i = 0; i < height; i++)
        for(int j = 0; j < width; j++)
        {
            if(grid[i][j]==1)
                std::cout<<"0 0 0\n";
            else
                std::cout<<"255 255 255\n";
        }
}

bool Map::cell_is_obstacle(int i, int j) const
{
    return (grid[i][j] == CN_OBSTL);
}

std::vector<Node> Map::get_valid_moves(int id,int agent) const
{
    std::vector<Node> retval=valid_moves[id];
	if (agent==-1)
		return retval;
	
	for (int i=0;i<retval.size();++i){
		if (retval[i].agent.find(-1)==retval[i].agent.end() && retval[i].agent.find(agent)==retval[i].agent.end()){
		//if (retval[i].agent!=-1 && retval[i].agent!=agent)
			/*if (valid_moves[retval[i].id].size()!=2){
				std::cout<<"id: "<<id<<", agent: "<<agent<<", i:"<<i<<std::endl;
				prt_validmoves();
				assert(false);
			}*/
			if (valid_moves[retval[i].id][0].id==id)
				retval[i]=valid_moves[retval[i].id][1];
			else
				retval[i]=valid_moves[retval[i].id][0];
		}
	}
	return retval;
}

bool Map::check_line(int x1, int y1, int x2, int y2)
{
    int delta_x(std::abs(x1 - x2));
    int delta_y(std::abs(y1 - y2));
    if((delta_x > delta_y && x1 > x2) || (delta_y >= delta_x && y1 > y2))
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    int step_x(x1 < x2 ? 1 : -1);
    int step_y(y1 < y2 ? 1 : -1);
    int error(0), x(x1), y(y1);
    int gap = int(agent_size*sqrt(pow(delta_x, 2) + pow(delta_y, 2)) + double(delta_x + delta_y)/2 - CN_EPSILON);
    int k, num;

    if(delta_x > delta_y)
    {
        int extraCheck = int(agent_size*delta_y/sqrt(pow(delta_x, 2) + pow(delta_y, 2)) + 0.5 - CN_EPSILON);
        for(int n = 1; n <= extraCheck; n++)
        {
            error += delta_y;
            num = (gap - error)/delta_x;
            for(k = 1; k <= num; k++)
                if(cell_is_obstacle(x1 - n*step_x, y1 + k*step_y))
                    return false;
            for(k = 1; k <= num; k++)
                if(cell_is_obstacle(x2 + n*step_x, y2 - k*step_y))
                    return false;
        }
        error = 0;
        for(x = x1; x != x2 + step_x; x++)
        {
            if(cell_is_obstacle(x, y))
                return false;
            if(x < x2 - extraCheck)
            {
                num = (gap + error)/delta_x;
                for(k = 1; k <= num; k++)
                    if(cell_is_obstacle(x, y + k*step_y))
                        return false;
            }
            if(x > x1 + extraCheck)
            {
                num = (gap - error)/delta_x;
                for(k = 1; k <= num; k++)
                    if(cell_is_obstacle(x, y - k*step_y))
                        return false;
            }
            error += delta_y;
            if((error<<1) > delta_x)
            {
                y += step_y;
                error -= delta_x;
            }
        }
    }
    else
    {
        int extraCheck = int(agent_size*delta_x/sqrt(pow(delta_x, 2) + pow(delta_y, 2)) + 0.5 - CN_EPSILON);
        for(int n = 1; n <= extraCheck; n++)
        {
            error += delta_x;
            num = (gap - error)/delta_y;
            for(k = 1; k <= num; k++)
                if(cell_is_obstacle(x1 + k*step_x, y1 - n*step_y))
                    return false;
            for(k = 1; k <= num; k++)
                if(cell_is_obstacle(x2 - k*step_x, y2 + n*step_y))
                    return false;
        }
        error = 0;
        for(y = y1; y != y2 + step_y; y += step_y)
        {
            if(cell_is_obstacle(x, y))
                return false;
            if(y < y2 - extraCheck)
            {
                num = (gap + error)/delta_y;
                for(k = 1; k <= num; k++)
                    if(cell_is_obstacle(x + k*step_x, y))
                        return false;
            }
            if(y > y1 + extraCheck)
            {
                num = (gap - error)/delta_y;
                for(k = 1; k <= num; k++)
                    if(cell_is_obstacle(x - k*step_x, y))
                        return false;
            }
            error += delta_x;
            if((error<<1) > delta_y)
            {
                x += step_x;
                error -= delta_y;
            }
        }
    }
    return true;
}

void Map::prt_ind(node_index n){
	std::cout<<" ("<<n.first<<","<<n.second<<")"<<std::endl;
}

void Map::prt_nodes(){
	hast_table::iterator it;
	std::cout<<"full_table"<<std::endl;
	for(auto it=nodes_table.begin(); it!=nodes_table.end(); ++it){
		std::cout<<"id:"<<it->second;
		prt_ind(it->first);
	}
}

void Map::prt_validmoves() const{
	//std::cout<<"Map:"<<std::endl;
	for (int i=0;i<valid_moves.size();++i){
		auto vecNode=valid_moves[i];
		std::cout<<"|"<<i<<
		" ("<<nodes[i].i<<","<<nodes[i].j<<") : ";
		for (Node n:vecNode){
			std::cout<<n.id<<"(";
			prt_set(n.agent);
			std::cout<<"), ";
		}
		std::cout<<std::endl;
	}
	std::cout<<"&&&&&&&"<<std::endl<<std::endl;
}

void Map::prt_set(std::set<int> s) const{
	for (int i:s)
		std::cout<<i<<",";
}

void Map::alter(Map_deltas map_deltas){
    for (Map_delta delta:map_deltas){
        int node_id(delta.add_node);
        
        if (node_id==-1)
            return;
        
        int v1(delta.del_edge.first),v2(delta.del_edge.second);
        
        for (int i=0;i<valid_moves[v1].size();++i){
            if (valid_moves[v1][i].id==v2){
                valid_moves[v1][i].i=get_i(node_id);
                valid_moves[v1][i].j=get_j(node_id);
                valid_moves[v1][i].id=node_id;
                valid_moves[v1][i].agent=nodes[node_id].agent;
                break;
            }
        }
        
        for (int i=0;i<valid_moves[v2].size();++i){
            if (valid_moves[v2][i].id==v1){
                valid_moves[v2][i].i=get_i(node_id);
                valid_moves[v2][i].j=get_j(node_id);
                valid_moves[v2][i].id=node_id;
                valid_moves[v2][i].agent=nodes[node_id].agent;
                break;
            }
        }
    }
}

void Map::alter_back(Map_deltas map_deltas){
    for (Map_delta delta: map_deltas){
        int node_id(delta.add_node);
        
        if (node_id==-1)
            return;
        
        int v1(delta.del_edge.first),v2(delta.del_edge.second);
        
        for (int i=0;i<valid_moves[v1].size();++i){
            if (valid_moves[v1][i].id==node_id){
                valid_moves[v1][i].i=get_i(v2);
                valid_moves[v1][i].j=get_j(v2);
                valid_moves[v1][i].id=v2;
                valid_moves[v1][i].agent=nodes[i].agent;
            }
        }
        
        for (int i=0;i<valid_moves[v2].size();++i){
            if (valid_moves[v2][i].id==node_id){
                valid_moves[v2][i].i=get_i(v1);
                valid_moves[v2][i].j=get_j(v1);
                valid_moves[v2][i].id=v1;
                valid_moves[v2][i].agent=nodes[i].agent;
            }
        }
    }
}
