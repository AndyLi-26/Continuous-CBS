#include "config.h"
using namespace  tinyxml2;
Config::Config()
{
    connectdness = CN_CONNECTEDNESS;
    use_cardinal = CN_USE_CARDINAL;
	use_edge_split = CN_USE_EDGE_SPLIT;
    agent_size = CN_AGENT_SIZE;
    timelimit = CN_TIMELIMIT;
    focal_weight = CN_FOCAL_WEIGHT;
    precision = CN_PRECISION;
	agent_num = CN_AGENT_NUM;
	output="";
}


void Config::getConfig(const char *fileName)
{
    stringstream stream;
    XMLDocument doc;
    if (doc.LoadFile(fileName) != tinyxml2::XMLError::XML_SUCCESS)
    {
        cout << "Error opening Config XML file!" << endl;
        return;
    }

    XMLElement *root = doc.FirstChildElement(CNS_TAG_ROOT);
    if (!root)
    {
        cout << "No 'root' element found in XML file."<<endl;
        return;
    }

    XMLElement *algorithm = root->FirstChildElement(CNS_TAG_ALGORITHM);
    if(!algorithm)
    {
        cout << "No 'algorithm' element found in XML file."<<endl;
        return;
    }

    XMLElement *element = algorithm->FirstChildElement("precision");
    if (!element)
    {
        cout << "Error! No 'precision' element found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_PRECISION<<"'."<<endl;
        precision = CN_PRECISION;
    }
    else
    {
        auto value = element->GetText();
        stream<<value;
        stream>>precision;
        if(precision > 1.0 || precision <= 0)
        {
            cout << "Error! Wrong 'precision' value found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_PRECISION<<"'."<<endl;
            precision = CN_PRECISION;
        }
        stream.clear();
        stream.str("");
    }

    element = algorithm->FirstChildElement("use_cardinal");
    if (!element)
    {
        cout << "Error! No 'use_cardinal' element found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_USE_CARDINAL<<"'."<<endl;
        use_cardinal = CN_USE_CARDINAL;
    }
    else
    {
        string value = element->GetText();
        if(value.compare("true") == 0 || value.compare("1") == 0)
        {
            use_cardinal = true;
        }
        else if(value.compare("false") == 0 || value.compare("0") == 0)
        {
            use_cardinal = false;
        }
        else
        {
            cout << "Error! Wrong 'use_cardinal' value found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_USE_CARDINAL<<"'."<<endl;
            use_cardinal = CN_USE_CARDINAL;
        }
    }

    element = algorithm->FirstChildElement("use_disjoint_splitting");
    if (!element)
    {
        cout << "Error! No 'use_disjoint_splitting' element found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_USE_DS<<"'."<<endl;
        use_disjoint_splitting = CN_USE_DS;
    }
    else
    {
        string value = element->GetText();
        if(value.compare("true") == 0 || value.compare("1") == 0)
        {
            use_disjoint_splitting = true;
        }
        else if(value.compare("false") == 0 || value.compare("0") == 0)
        {
            use_disjoint_splitting = false;
        }
        else
        {
            cout << "Error! Wrong 'use_disjoint_splitting' element found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_USE_DS<<"'."<<endl;
            use_disjoint_splitting = CN_USE_DS;
        }
    }

    element = algorithm->FirstChildElement("connectedness");
    if (!element)
    {
        cout << "Error! No 'connectedness' element found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_CONNECTEDNESS<<"'."<<endl;
        connectdness = CN_CONNECTEDNESS;
    }
    else
    {
        auto value = element->GetText();
        stream<<value;
        stream>>connectdness;
        if(connectdness > 5 || connectdness < 2)
        {
            cout << "Error! Wrong 'connectedness' value found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_CONNECTEDNESS<<"'."<<endl;
            connectdness = CN_CONNECTEDNESS;
        }
        stream.clear();
        stream.str("");
    }

    element = algorithm->FirstChildElement("focal_weight");
    if (!element)
    {
        cout << "Error! No 'focal_weight' element found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_FOCAL_WEIGHT<<"'."<<endl;
        focal_weight = CN_FOCAL_WEIGHT;
    }
    else
    {
        auto value = element->GetText();
        stream<<value;
        stream>>focal_weight;
        if(focal_weight < 1.0)
        {
            cout << "Error! Wrong 'focal_weight' value found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_FOCAL_WEIGHT<<"'."<<endl;
            focal_weight = CN_FOCAL_WEIGHT;
        }
        stream.clear();
        stream.str("");
    }

    element = algorithm->FirstChildElement("agent_size");
    if (!element)
    {
        cout << "Error! No 'agent_size' element found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_AGENT_SIZE<<"'."<<endl;
        agent_size = CN_AGENT_SIZE;
    }
    else
    {
        auto value = element->GetText();
        stream<<value;
        stream>>agent_size;
        if(agent_size < 0 || agent_size > 0.5)
        {
            cout << "Error! Wrong 'agent_size' value found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_AGENT_SIZE<<"'."<<endl;
            agent_size = CN_AGENT_SIZE;
        }
        stream.clear();
        stream.str("");
    }

    element = algorithm->FirstChildElement("hlh_type");
    if (!element)
    {
        cout << "Error! No 'hlh_type' element found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_HLH_TYPE<<"'."<<endl;
        hlh_type = CN_HLH_TYPE;
    }
    else
    {
        auto value = element->GetText();
        stream<<value;
        stream>>hlh_type;
        if(hlh_type < 0 || hlh_type > 2)
        {
            cout << "Error! Wrong 'hlh_type' value found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_HLH_TYPE<<"'."<<endl;
            hlh_type = CN_HLH_TYPE;
        }
        stream.clear();
        stream.str("");
    }

    element = algorithm->FirstChildElement("timelimit");
    if (!element)
    {
        cout << "Error! No 'timelimit' element found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_TIMELIMIT<<"'."<<endl;
        timelimit = CN_TIMELIMIT;
    }
    else
    {
        auto value = element->GetText();
        stream<<value;
        stream>>timelimit;
        if(timelimit <= 0)
            timelimit = CN_INFINITY;
        stream.clear();
        stream.str("");
    }
	
	element = algorithm->FirstChildElement("agent_number");
    if (!element)
    {
        cout << "Error! No 'agent_number' element found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_AGENT_NUM<<"'."<<endl;
        agent_num = CN_AGENT_NUM;
    }
    else
    {
        auto value = element->GetText();
        stream<<value;
        stream>>agent_num;
        stream.clear();
        stream.str("");
    }
	
	element = algorithm->FirstChildElement("output_file");
    if (!element)
    {
        cout << "Error! No 'output_file' element found inside, won't write to a txt"<<endl;
        output = "";
    }
    else
    {
        auto value = element->GetText();
        stream<<value;
        stream>>output;
        stream.clear();
        stream.str("");
    }
	
	element = algorithm->FirstChildElement("use_edge_split");
    if (!element)
    {
        cout << "Error! No 'use_edge_split' element found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_USE_EDGE_SPLIT<<"'."<<endl;
        use_edge_split = CN_USE_EDGE_SPLIT;
    }
    else
    {
        string value = element->GetText();
        if(value.compare("true") == 0 || value.compare("1") == 0)
        {
            use_edge_split = true;
        }
        else if(value.compare("false") == 0 || value.compare("0") == 0)
        {
            use_edge_split = false;
        }
        else
        {
            cout << "Error! Wrong 'use_edge_split' value found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_USE_EDGE_SPLIT<<"'."<<endl;
            use_edge_split = CN_USE_EDGE_SPLIT;
        }
    }
	/*
	element = algorithm->FirstChildElement("resolution");
    if (!element)
    {
        cout << "Error! No 'resolution' element found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_RESOLUTION<<"'."<<endl;
        resolution = CN_RESOLUTION;
    }
    else
    {
        auto value = element->GetText();
        stream<<value;
        stream>>resolution;
        if(resolution < 0 || resolution > agent_size)
        {
            cout << "Error! Wrong 'resolution' value found inside '"<<CNS_TAG_ALGORITHM<<"' section. It's compared to '"<<CN_RESOLUTION<<"'."<<endl;
            resolution = CN_RESOLUTION;
        }
        stream.clear();
        stream.str("");
    }
	*/
    return;
}
