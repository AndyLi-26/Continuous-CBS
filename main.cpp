#include <iostream>
#include <fstream>
#include "map.h"
#include "task.h"
#include "cbs.h"
#include "xml_logger.h"
using namespace std;
int main(int argc, const char *argv[])
{
    if(argc > 2)
    {
        Config config;
        if(argc > 3){
            config.getConfig(argv[3]);
			cout<<"read config success"<<endl;
		}
        Map map = Map(config.agent_size, config.connectdness);
        map.get_map(argv[1]);
		cout<<"read map success"<<endl;
        Task task(config.agent_num);
        task.get_task(argv[2]);
		cout<<"read task success"<<endl;
        if(map.is_roadmap())
            task.make_ij(map);
        else
            task.make_ids(map.get_width());
		cout<<"agents_num="<<task.get_agent_num()<<endl;
		task.prt_agents();
		//task.print_task();
        CBS cbs;
        Solution solution = cbs.find_solution(map, task, config);
        XML_logger logger;
        auto found = solution.found?"true":"false";
        auto Use_edge = config.use_edge_split?"true":"false";
		
        std::cout<< "Soulution found: " << found <<"\nUse Edge Splitting: "<< Use_edge <<
		"\nRuntime: "<<solution.time.count() << "\nMakespan: " << solution.makespan << "\nFlowtime: " << solution.flowtime<< "\nInitial Cost: "<<solution.init_cost<< "\nCollision Checking Time: " << solution.check_time
             << "\nHL expanded: " << solution.high_level_expanded << "\nLL searches: " << solution.low_level_expansions << "\nLL expanded(avg): " << solution.low_level_expanded << std::endl;
		std::cout<<"agent_size: "<<config.agent_size<<std::endl;
		std::cout<<"introduce new node: "<<map.get_new_node_num()<<std::endl;
        logger.get_log(argv[2]);
        logger.write_to_log_summary(solution);
		logger.txt_writer(solution,config,map.get_new_node_num(), argv[2]);
        logger.write_to_log_path(solution, map);
        logger.save_log();
    }
    else
    {
        std::cout<<"Error! Not enough input parameters are specified!\n";
    }
    return 0;
}
