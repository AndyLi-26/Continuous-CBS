#include "cbs.h"

bool CBS::init_root(Map &map, const Task &task)
{
  CBS_Node root;
  tree.set_focal_weight(config.focal_weight);
  sPath path;
  for(int i = 0; i < int(task.get_agents_size()); i++)
  {
    Agent agent = task.get_agent(i);
    path = planner.find_path(agent, map, {}, h_values);
    if(path.cost < 0)
      return false;
    root.paths.push_back(path);
    root.cost += path.cost;
  }
  root.low_level_expanded = 0;
  root.parent = nullptr;
  root.id = 1;
  root.id_str = "1";
  auto conflicts = get_all_conflicts(root.paths, -1);
  root.conflicts_num = conflicts.size();
  cout<<"init_path"<<endl;
  prt_paths(root.paths);
  for(auto conflict: conflicts)
    if(!config.use_cardinal)
      root.conflicts.push_back(conflict);
    else
    {
      auto pathA = planner.find_path(task.get_agent(conflict.agent1), map, {get_constraint(conflict.agent1, conflict.move1, conflict.move2)}, h_values);
      auto pathB = planner.find_path(task.get_agent(conflict.agent2), map, {get_constraint(conflict.agent2, conflict.move2, conflict.move1)}, h_values);
      //conflict.path1 = pathA;
      //conflict.path2 = pathB;
      if(pathA.cost > root.paths[conflict.agent1].cost && pathB.cost > root.paths[conflict.agent2].cost)
      {
        conflict.overcost = std::min(pathA.cost - root.paths[conflict.agent1].cost, pathB.cost - root.paths[conflict.agent2].cost);
        root.cardinal_conflicts.push_back(conflict);
      }
      else if(pathA.cost > root.paths[conflict.agent1].cost || pathB.cost > root.paths[conflict.agent2].cost)
        root.semicard_conflicts.push_back(conflict);
      else
        root.conflicts.push_back(conflict);
    }
  solution.init_cost = root.cost;
  tree.add_node(root);
  CBS_Node_aux*  r_aux= new CBS_Node_aux(root);
  tree_info[1]=r_aux;
  map.prt_validmoves();
  return true;
}

bool CBS::check_conflict(Move move1, Move move2)
{

  double startTimeA(move1.t1), endTimeA(move1.t2), startTimeB(move2.t1), endTimeB(move2.t2);
  double m1i1(map->get_i(move1.id1)), m1i2(map->get_i(move1.id2)), m1j1(map->get_j(move1.id1)), m1j2(map->get_j(move1.id2));
  double m2i1(map->get_i(move2.id1)), m2i2(map->get_i(move2.id2)), m2j1(map->get_j(move2.id1)), m2j2(map->get_j(move2.id2));
  Vector2D A(m1i1, m1j1);
  Vector2D B(m2i1, m2j1);
  Vector2D VA((m1i2 - m1i1)/(move1.t2 - move1.t1), (m1j2 - m1j1)/(move1.t2 - move1.t1));
  Vector2D VB((m2i2 - m2i1)/(move2.t2 - move2.t1), (m2j2 - m2j1)/(move2.t2 - move2.t1));
  if(startTimeB > startTimeA)
  {
    A += VA*(startTimeB-startTimeA);
    startTimeA = startTimeB;
  }
  else if(startTimeB < startTimeA)
  {
    B += VB*(startTimeA - startTimeB);
    startTimeB = startTimeA;
  }
  double r(2*config.agent_size);
  Vector2D w(B - A);
  double c(w*w - r*r);
  if(c < 0)
    return true;

  Vector2D v(VA - VB);
  double a(v*v);
  double b(w*v);
  double dscr(b*b - a*c);
  if(dscr - config.precision < 0)
    return false;
  double ctime = (b - sqrt(dscr))/a;
  if(ctime > - config.precision  && ctime < std::min(endTimeB,endTimeA) - startTimeA +  config.precision )
    return true;
  return false;
}
/*
// my version
Constraint CBS::get_wait_constraint(int agent, Move move1, Move move2)
{
double radius = 2*config.agent_size;
double i0(map->get_i(move2.id1)), j0(map->get_j(move2.id1)), i1(map->get_i(move2.id2)), j1(map->get_j(move2.id2)), i2(map->get_i(move1.id1)), j2(map->get_j(move1.id1));
std::pair<double,double> interval;
Point point(i2,j2), p0(i0,j0), p1(i1,j1);
int cls = point.classify(p0, p1);
double dist = fabs((i0 - i1)*j2 + (j1 - j0)*i2 + (j0*i1 - i0*j1))/sqrt(pow(i0 - i1, 2) + pow(j0 - j1, 2));
double da = (i0 - i2)*(i0 - i2) + (j0 - j2)*(j0 - j2);
double db = (i1 - i2)*(i1 - i2) + (j1 - j2)*(j1 - j2);
double ha = sqrt(da - dist*dist+CN_EPSILON);
double size = sqrt(radius*radius - dist*dist);

if(da < radius*radius)
{
if(db < radius*radius)
{
interval.first = move2.t1;
interval.second = move2.t2;
}
else
{
interval.first = move2.t1;
interval.second = move2.t2 - ha + size;
}
}
else
{
if(db < radius*radius)
{
interval.first = move2.t2 - size + sqrt(db - dist*dist+CN_EPSILON);
interval.second = move2.t2;
}
else
{
interval.first = move1.t1 + ha - size;
interval.second = move1.t1 + ha + size;
}
}

return Constraint(agent, interval.first, interval.second, move1.id1, -1,move1.id2); // exit index -1 means wait
}
*/

//original version
Constraint CBS::get_wait_constraint(int agent, Move move1, Move move2)
{
  double radius = 2*config.agent_size;
  double i0(map->get_i(move2.id1)), j0(map->get_j(move2.id1)), i1(map->get_i(move2.id2)), j1(map->get_j(move2.id2)), i2(map->get_i(move1.id1)), j2(map->get_j(move1.id1));
  std::pair<double,double> interval;
  Point point(i2,j2), p0(i0,j0), p1(i1,j1);
  int cls = point.classify(p0, p1);
  double dist = fabs((i0 - i1)*j2 + (j1 - j0)*i2 + (j0*i1 - i0*j1))/sqrt(pow(i0 - i1, 2) + pow(j0 - j1, 2));
  double da = (i0 - i2)*(i0 - i2) + (j0 - j2)*(j0 - j2);
  double db = (i1 - i2)*(i1 - i2) + (j1 - j2)*(j1 - j2);
  double ha = sqrt(da - dist*dist);
  double size = sqrt(radius*radius - dist*dist);
  if(cls == 3)
  {
    interval.first = move2.t1;
    interval.second = move2.t1 + (sqrt(radius*radius - dist*dist) - ha);
  }
  else if(cls == 4)
  {
    interval.first = move2.t2 - sqrt(radius*radius - dist*dist) + sqrt(db - dist*dist);
    interval.second = move2.t2;
  }
  else if(da < radius*radius)
  {
    if(db < radius*radius)
    {
      interval.first = move2.t1;
      interval.second = move2.t2;
    }
    else
    {
      double hb = sqrt(db - dist*dist);
      interval.first = move2.t1;
      interval.second = move2.t2 - hb + size;
    }
  }
  else
  {
    if(db < radius*radius)
    {
      interval.first = move2.t1 + ha - size;
      interval.second = move2.t2;
    }
    else
    {
      interval.first = move2.t1 + ha - size;
      interval.second = move2.t1 + ha + size;
    }
  }
  return Constraint(agent, interval.first, interval.second, move1.id1, -1,move1.id2); // exit index -1 means wait
}

double CBS::get_hl_heuristic(const std::list<Conflict> &conflicts)
{
  if(conflicts.empty() || config.hlh_type == 0)
    return 0;
  else if (config.hlh_type == 1)
  {
    optimization::Simplex simplex("simplex");
    std::map<int, int> colliding_agents;
    for(auto c: conflicts)
    {
      colliding_agents.insert({c.agent1, colliding_agents.size()});
      colliding_agents.insert({c.agent2, colliding_agents.size()});
    }

    pilal::Matrix coefficients(conflicts.size(), colliding_agents.size(), 0);
    std::vector<double> overcosts(conflicts.size());
    int i(0);
    for(auto c:conflicts)
    {
      coefficients.at(i, colliding_agents.at(c.agent1)) = 1;
      coefficients.at(i, colliding_agents.at(c.agent2)) = 1;
      overcosts[i] = c.overcost;
      i++;
    }
    simplex.set_problem(coefficients, overcosts);
    simplex.solve();
    return simplex.get_solution();
  }
  else
  {
    double h_value(0);
    std::vector<std::tuple<double, int, int>> values;
    values.reserve(conflicts.size());
    std::set<int> used;
    for(auto c:conflicts)
      values.push_back(std::make_tuple(c.overcost, c.agent1, c.agent2));
    std::sort(values.begin(), values.end(), std::greater<std::tuple<double, int, int>>());
    for(auto v: values)
    {
      if(used.find(get<1>(v)) != used.end() || used.find(get<2>(v)) != used.end())
        continue;
      h_value += get<0>(v);
      used.insert(get<1>(v));
      used.insert(get<2>(v));
    }
    return h_value;
  }
}

Constraint CBS::get_constraint(int agent, Move move1, Move move2)
{
  if(move1.id1 == move1.id2)
    return get_wait_constraint(agent, move1, move2);
  double startTimeA(move1.t1), endTimeA(move1.t2);
  Vector2D A(map->get_i(move1.id1), map->get_j(move1.id1)), A2(map->get_i(move1.id2), map->get_j(move1.id2)),
           B(map->get_i(move2.id1), map->get_j(move2.id1)), B2(map->get_i(move2.id2), map->get_j(move2.id2));
  if(move2.t2 == CN_INFINITY){
    int exit_index=id2ind(move1.id1, move1.id2,agent);
    return Constraint(agent, move1.t1, CN_INFINITY, move1.id1, exit_index, move1.id2);
  }
  double delta = move2.t2 - move1.t1;
  while(delta > config.precision/2.0)
  {
    if(check_conflict(move1, move2))
    {
      move1.t1 += delta;
      move1.t2 += delta;
    }
    else
    {
      move1.t1 -= delta;
      move1.t2 -= delta;
    }
    if(move1.t1 > move2.t2 + CN_EPSILON)
    {
      move1.t1 = move2.t2;
      move1.t2 = move1.t1 + endTimeA - startTimeA;
      break;
    }
    delta /= 2.0;
  }
  if(delta < config.precision/2.0 + CN_EPSILON && check_conflict(move1, move2))
  {
    move1.t1 = fmin(move1.t1 + delta*2, move2.t2);
    move1.t2 = move1.t1 + endTimeA - startTimeA;
  }
  int exit_index=id2ind(move1.id1, move1.id2,agent);
  return Constraint(agent, startTimeA, move1.t1, move1.id1, exit_index, move1.id2);
}

int CBS::id2ind(int v1, int v2,int agent)
{
  std::vector<Node> temp_moves=map->get_valid_moves(v1,agent);
  for (int i=0;i<temp_moves.size();++i){
    if (temp_moves[i].id==v2)
      return i;
  }
  //cout<<"trying get from: "<<v1<<"->"<<v2<<"for a:"<<agent<<endl;
  //map->prt_validmoves();
  return -2;
}

Conflict CBS::get_conflict(std::list<Conflict> &conflicts)
{
  auto best_it = conflicts.begin();
  for(auto it = conflicts.begin(); it != conflicts.end(); it++)
  {
    if(it->overcost > 0)
    {
      if(best_it->overcost < it->overcost || (fabs(best_it->overcost - it->overcost) < CN_EPSILON && best_it->t < it->t))
        best_it = it;
    }
    else if(best_it->t < it->t)
      best_it = it;
  }

  Conflict conflict = *best_it;
  conflicts.erase(best_it);
  return conflict;
}

Solution CBS::find_solution(Map &map, const Task &task, const Config &cfg)
{
  config = cfg;

  this->map = &map;
  Map temp(map);
  this->original = &temp;
  h_values.init(map.get_size(), task.get_agents_size());
  for(int i = 0; i < int(task.get_agents_size()); i++)
  {
    Agent agent = task.get_agent(i);
    h_values.count(map, agent);
  }
  auto t = std::chrono::high_resolution_clock::now();
  int cardinal_solved = 0, semicardinal_solved = 0;
  if(!this->init_root(map, task))
    return solution;
  solution.init_time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t);
  solution.found = true;
  CBS_Node node;
  std::chrono::duration<double> time_spent;
  int expanded(1);
  double time(0);
  std::list<Conflict> conflicts;
  Conflict conflict;
  std::vector<int> conflicting_agents;
  std::vector<std::pair<int, int>> conflicting_pairs;
  int low_level_searches(0);
  int low_level_expanded(0);
  int id = 2;
  do
  {
    auto parent = tree.get_front();
    node = *parent;
    node.cost -= node.h;
    parent->conflicts.clear();
    parent->cardinal_conflicts.clear();
    parent->semicard_conflicts.clear();
    bool p=false;
    if (node.id==4)
      p=false;

    //cout<<"###"<<endl;
    //cout<<node.id<<endl;
    auto paths = get_paths(&node, task.get_agents_size());
    if (p){
    cout<<"###   "<<node.id<<"   #####################################"<<endl;
      cout<<"before conflict"<<endl;
      prt_paths(paths);
    cout<<"ori map"<<endl;
    map.prt_validmoves();
    }
    //cout<<flush;
    //prt_conflicts(parent->conflicts);
    //cout<<"------------------"<<endl;
    auto time_now = std::chrono::high_resolution_clock::now();
    conflicts = node.conflicts;
    auto cardinal_conflicts = node.cardinal_conflicts;
    auto semicard_conflicts = node.semicard_conflicts;
    if(conflicts.empty() && semicard_conflicts.empty() && cardinal_conflicts.empty())
    {
      printBT_aux();
      //string file="CT_tree.dot";
      //saveCT(file,&node,task.get_agents_size());
      //prt_paths(paths);
      //if (config.use_edge_split)
      //	gen_new_map(&node);
      //map.prt_validmoves();
      break; //i.e. no conflicts => solution found
    }
    if(!cardinal_conflicts.empty())
    {
      conflict = get_conflict(cardinal_conflicts);
      cardinal_solved++;
    }
    else if(!semicard_conflicts.empty())
    {
      conflict = get_conflict(semicard_conflicts);
      semicardinal_solved++;
    }
    else
      conflict = get_conflict(conflicts);

    parent->cur_conflict=conflict; //del when experiment
                                   //Map_delta_pair info;
    Map_deltas deltasL,deltasR;
    deltasL.clear(); deltasR.clear();
    if (p)
      prt_conflict(conflict);
    if(config.use_edge_split)
      split_edge(conflict, paths,deltasR,deltasL);
    if (p)
      prt_map_deltas(deltasR,deltasL);

    time_spent = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - time_now);
    time += time_spent.count();
    expanded++;
    if (expanded==51)
      1;
    std::list<Constraint> constraintsA = get_constraints(&node, conflict.agent1);
    //std::list<Constraint> constraintsA_New;
    Constraint constraintA;
    if (p)
    {prt_constraints(constraintsA);
      cout<<"+"<<endl;
    }
    if (config.use_edge_split)
      gen_new_map(&node);

    if (config.use_edge_split && !deltasR.empty() && conflict.move1.id1!=conflict.move1.id2){
      //cout<<"move1: ";
      //prt_move(conflict.move1);
      Move temp_move=modify_move(conflict.move1,(deltasR.begin())->add_node);
      //cout<<"new_m: ";
      //prt_move(temp_move);
      constraintA = get_constraint(conflict.agent1, temp_move, conflict.move2);
      constraintsA.push_back(constraintA);
    }
    else{
      constraintA = get_constraint(conflict.agent1, conflict.move1, conflict.move2);
      constraintsA.push_back(constraintA);
    }
    if (p)
    {
    cout<<"new constraintA:  ";
    prt_constraint(constraintA);
    cout<<"||"<<endl;
    prt_constraints(constraintsA);
    cout<<"----"<<endl;
    }
    //cout<<"original"<<endl;
    //map.prt_validmoves();



    //cout<<"curNode"<<endl;
    //map.prt_validmoves();
    sPath pathA;
    if (config.use_edge_split && !deltasR.empty()){
      map.alter(deltasR);
      //cout<<"modA"<<endl;
      //map.prt_validmoves();
      pathA = planner.find_path(task.get_agent(conflict.agent1), map, constraintsA, h_values);
      map.alter_back(deltasR);
      //cout<<"prev: node"<<endl;
      //map.prt_validmoves();
    }
    else{
      pathA = planner.find_path(task.get_agent(conflict.agent1), map, constraintsA, h_values);
    }
    if (p){
      prt_path(pathA);
    }
    low_level_searches++;
    low_level_expanded += pathA.expanded;
    //----------------------------------------------------------------------------------------------------------------
    if (p)
      cout<<"-----------------------------------------"<<endl;
    std::list<Constraint> constraintsB = get_constraints(&node, conflict.agent2);
    Constraint constraintB;
    if (p){
     prt_constraints(constraintsB);
      cout<<"+"<<endl;
    }
    if (config.use_edge_split && !deltasL.empty() && conflict.move2.id1!=conflict.move2.id2){
      //cout<<"move2: ";
      //prt_move(conflict.move2);
      Move temp_move=modify_move(conflict.move2,(deltasL.begin())->add_node);
      //cout<<"new_m: ";
      //prt_move(temp_move);
      constraintB= get_constraint(conflict.agent2, temp_move, conflict.move1);
      constraintsB.push_back(constraintB);
    }
    else{
      constraintB = get_constraint(conflict.agent2, conflict.move2, conflict.move1);
      //cout<<"moveB1:";
      //prt_move(conflict.move2);
      //cout<<", moveB2: ";
      //prt_move(conflict.move1);
      constraintsB.push_back(constraintB);
    }
    if (p){
    cout<<"new constraintB:  ";
    prt_constraint(constraintB);
    cout<<"||"<<endl;
    prt_constraints(constraintsB);
    cout<<"----"<<endl;
    cout<<endl;
    cout<<flush;
    }
    sPath pathB;
    
    if (config.use_edge_split && !deltasL.empty()){
      map.alter(deltasL);
      //cout<<"modB"<<endl;
      //map.prt_validmoves();
      pathB = planner.find_path(task.get_agent(conflict.agent2), map, constraintsB, h_values,p);
      map.alter_back(deltasL);
      //cout<<"prev: node"<<endl;
      //map.prt_validmoves();
    }
    else{
      pathB = planner.find_path(task.get_agent(conflict.agent2), map, constraintsB, h_values,p);
    }
    if (p){
    prt_path(pathB);
    //assert(false);
    }
    /*
       if (config.use_edge_split){
       gen_original_map(&node);
    //cout<<"original"<<endl;
    //map.prt_validmoves();
    assert(map.equal(original));
    }
    */
    low_level_searches++;
    low_level_expanded += pathB.expanded;

    CBS_Node right({pathA}, parent, constraintA,deltasR, node.cost + pathA.cost - get_cost(node, conflict.agent1), 0, node.total_cons + 1);
    CBS_Node left ({pathB}, parent, constraintB,deltasL, node.cost + pathB.cost - get_cost(node, conflict.agent2), 0, node.total_cons + 1);

    Constraint positive;
    bool inserted = false;
    bool left_ok = true, right_ok = true;
    if(config.use_disjoint_splitting)
    {
      int agent1positives(0), agent2positives(0);
      for(auto c: constraintsA)
        if(c.positive)
          agent1positives++;
      for(auto c: constraintsB)
        if(c.positive)
          agent2positives++;

      /*
         Constraint cons2changeA,cons2changeB;
         if (config.use_edge_split && info.first.add_node!=-1)
         cons2changeA=tempA;
         else
         cons2changeA=constraintA;

         if (config.use_edge_split && info.second.add_node!=-1)
         cons2changeB=tempB;
         else
         cons2changeB=constraintB;
         */

      if(conflict.move1.id1 != conflict.move1.id2 && agent2positives > agent1positives && pathA.cost > 0)
      {
        int exit_index=id2ind(conflict.move1.id1, conflict.move1.id2,conflict.agent1);
        positive = Constraint(constraintA.agent, constraintA.t1, constraintA.t2, constraintA.id1, constraintA.id2, constraintA.to_id, true);
        //positive = Constraint(conflict.agent1, constraintA.t1, constraintA.t2, conflict.move1.id1, exit_index, conflict.move1.id2, true);
        if(check_positive_constraints(constraintsA, positive))
        {
          left.positive_constraint = positive;
          left.total_cons++;
          constraintsB.push_back(left.positive_constraint);
          inserted = true;
        }
      }
      if(conflict.move2.id1 != conflict.move2.id2 && !inserted && pathB.cost > 0)
      {
        int exit_index=id2ind(conflict.move2.id1, conflict.move2.id2,conflict.agent2);
        positive = Constraint(conflict.agent2, constraintB.t1, constraintB.t2, conflict.move2.id1, exit_index, conflict.move2.id2, true);
        if(check_positive_constraints(constraintsB, positive))
        {
          right.positive_constraint = positive;
          right.total_cons++;
          constraintsA.push_back(right.positive_constraint);
          inserted = true;
        }
      }
      if(conflict.move1.id1 != conflict.move1.id2 && !inserted && pathA.cost > 0)
      {
        int exit_index=id2ind(conflict.move1.id1, conflict.move1.id2,conflict.agent1);
        positive = Constraint(conflict.agent1, constraintA.t1, constraintA.t2, conflict.move1.id1, exit_index, conflict.move1.id2, true);
        if(check_positive_constraints(constraintsA, positive))
        {
          inserted = true;
          left.positive_constraint = positive;
          left.total_cons++;
          constraintsB.push_back(left.positive_constraint);
        }
      }
    }
    right.id_str = node.id_str + "0";
    left.id_str = node.id_str + "1";
    right.id = id++;
    left.id = id++;
    //tree_aux::iterator it;
    //it = tree_info.find(parent->id);
    //CBS_Node_aux* rap(*it);
    tree_info[node.id]->id_left=left.id;
    tree_info[node.id]->id_right=right.id;
    CBS_Node_aux* l_aux= new CBS_Node_aux(left);
    CBS_Node_aux* r_aux=new CBS_Node_aux(right);
    tree_info[left.id]=l_aux;
    tree_info[right.id]=r_aux;
    //cout<<node.id<<"->"<<right.id<<endl;
    //cout<<node.id<<"->"<<left.id<<endl;
    /*
       if (left.id>=50){
       string file="CT_tree_no_sol.dot";
       saveCT(file,&node,task.get_agents_size());
       printBT_aux(&node);
    //printBT("", dummy_start, false);
    assert(false);
    }
    */

    if(right_ok && pathA.cost > 0 && validate_constraints(constraintsA, pathA.agentID))
    {
      time_now = std::chrono::high_resolution_clock::now();

      if (config.use_edge_split)
        gen_new_map(&right);
      //cout<<"modA"<<endl;
      //map.prt_validmoves();
      find_new_conflicts(map, task, right, paths, pathA, conflicts, semicard_conflicts, cardinal_conflicts, low_level_searches, low_level_expanded);
      if (config.use_edge_split)
        gen_original_map(&right);
      //cout<<"original"<<endl;
      //map.prt_validmoves();
      time_spent = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - time_now);
      time += time_spent.count();
      if(right.cost > 0)
      {
        right.h = get_hl_heuristic(right.cardinal_conflicts);
        right.cost += right.h;
        tree.add_node(right);
        /*
           if (node.cost+node.h-0.1>right.cost+config.precision){
           cout<<"g+h="<<node.cost<<"+"<<node.h<<">"<<right.cost<<endl;
           cout<<flush;
           string file="CT_tree_no_sol.dot";
           saveCT(file,&right,task.get_agents_size());
           assert(false);
           }
           */
      }
    }
    if(left_ok && pathB.cost > 0 && validate_constraints(constraintsB, pathB.agentID))
    {
      time_now = std::chrono::high_resolution_clock::now();
      if (config.use_edge_split)
        gen_new_map(&left);
      //cout<<"modB"<<endl;
      //map.prt_validmoves();
      find_new_conflicts(map, task, left, paths, pathB, conflicts, semicard_conflicts, cardinal_conflicts, low_level_searches, low_level_expanded);
      if (config.use_edge_split)
        gen_original_map(&left);
      //cout<<"original"<<endl;
      //map.prt_validmoves();
      time_spent = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - time_now);
      time += time_spent.count();           
      if(left.cost > 0)
      {
        left.h = get_hl_heuristic(left.cardinal_conflicts);
        left.cost += left.h;
      tree.add_node(left);
        /*
           if (node.cost+node.h-0.1>left.cost+config.precision){
           cout<<endl<<endl<<endl;
           cout<<"left id:"<<left.id<<endl;
           cout<<"map:"<<endl;
           map.prt_validmoves();
           cout<<"conflict";
           prt_conflict(conflict);
           cout<<"costraints4:"<<endl;
           std::list<Constraint> temp = get_constraints(&left, 4);
           prt_constraints(temp);
           cout<<"parent"<<endl;
           auto paths = get_paths(&node, task.get_agents_size());
           prt_paths(paths);
           cout<<"new left:"<<endl;
           paths = get_paths(&left, task.get_agents_size());
           prt_paths(paths);

           cout<<"g+h="<<node.cost<<"+"<<node.h<<">"<<left.cost<<endl;
           cout<<flush;
           string file="CT_tree_no_sol.dot";
           saveCT(file,&left,task.get_agents_size());
           assert(false);
           }
           */

      }
    }
    time_spent = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t);
    if(time_spent.count() > config.timelimit )
    {
      solution.found = false;
      printBT_aux();
      map.prt_validmoves();
      break;
    }
  }
  while(tree.get_open_size() > 0);
  solution.paths = get_paths(&node, task.get_agents_size());
  solution.flowtime = node.cost;
  solution.low_level_expansions = low_level_searches;
  solution.low_level_expanded = double(low_level_expanded)/std::max(low_level_searches, 1);
  solution.high_level_expanded = expanded;
  solution.high_level_generated = int(tree.get_size());
  for(auto path:solution.paths)
    solution.makespan = (solution.makespan > path.cost) ? solution.makespan : path.cost;
  solution.time = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - t);
  solution.check_time = time;
  solution.cardinal_solved = cardinal_solved;
  solution.semicardinal_solved = semicardinal_solved;

  return solution;
}

bool CBS::check_positive_constraints(std::list<Constraint> constraints, Constraint constraint)
{
  std::list<Constraint> positives;
  for(auto c: constraints)
    if(c.positive && c.agent == constraint.agent)
      positives.push_back(c);

  for(auto p: positives)
  {
    if(p.id1 == constraint.id1 && p.id2 == constraint.id2 && p.t1 - CN_EPSILON < constraint.t1 && p.t2 + CN_EPSILON > constraint.t2) // agent needs to perform two equal actions simultaneously => it's impossible
      return false;
    if(p.id1 == constraint.id1 && p.id2 == constraint.id2 && constraint.t1 - CN_EPSILON < p.t1 && constraint.t2 + CN_EPSILON > p.t2)
      return false;
  }
  return true;
}

bool CBS::validate_constraints(std::list<Constraint> constraints, int agent)
{
  std::list<Constraint> positives;
  for(auto c: constraints)
    if(c.positive && c.agent == agent)
      positives.push_back(c);
  for(auto p: positives)
    for(auto c: constraints)
    {
      if(c.positive)
        continue;
      if(p.agent == c.agent && p.id1 == c.id1 && p.id2 == c.id2) //if the same action
        if(p.t1 > c.t1 - CN_EPSILON && p.t2 < c.t2 + CN_EPSILON) //if the whole positive interval is inside collision interval
          return false;
    }
  return true;
}

void CBS::find_new_conflicts(Map &map, const Task &task, CBS_Node &node, std::vector<sPath> &paths, const sPath &path,
    const std::list<Conflict> &conflicts, const std::list<Conflict> &semicard_conflicts, const std::list<Conflict> &cardinal_conflicts,
    int &low_level_searches, int &low_level_expanded)
{
  auto oldpath = paths[path.agentID];
  paths[path.agentID] = path;
  //cout<<"checking conf###############"<<endl;
  auto new_conflicts = get_all_conflicts(paths, path.agentID);
  paths[path.agentID] = oldpath;
  std::list<Conflict> conflictsA({}), semicard_conflictsA({}), cardinal_conflictsA({});
  for(auto c: conflicts)
    if(c.agent1 != path.agentID && c.agent2 != path.agentID)
      conflictsA.push_back(c);
  for(auto c: semicard_conflicts)
    if(c.agent1 != path.agentID && c.agent2 != path.agentID)
      semicard_conflictsA.push_back(c);
  for(auto c: cardinal_conflicts)
    if(c.agent1 != path.agentID && c.agent2 != path.agentID)
      cardinal_conflictsA.push_back(c);
  if(!config.use_cardinal)
  {
    node.conflicts = conflictsA;
    for(auto n:new_conflicts)
      node.conflicts.push_back(n);
    node.cardinal_conflicts.clear();
    node.semicard_conflicts.clear();
    node.conflicts_num = node.conflicts.size();
    return;
  }
  for(auto c: new_conflicts)
  {
    std::list<Constraint> constraintsA, constraintsB;
    if(path.agentID == c.agent1)
    {
      constraintsA = get_constraints(&node, c.agent1);

      constraintsA.push_back(get_constraint(c.agent1, c.move1, c.move2));
      //cout<<"temp new cons 1";
      //prt_constraint(get_constraint(c.agent1, c.move1, c.move2));
      auto new_pathA = planner.find_path(task.get_agent(c.agent1), map, constraintsA, h_values);
      constraintsB = get_constraints(&node, c.agent2);
      constraintsB.push_back(get_constraint(c.agent2, c.move2, c.move1));
      //cout<<"temp new cons 2";
      //prt_constraint(get_constraint(c.agent2, c.move2, c.move1));
      auto new_pathB = planner.find_path(task.get_agent(c.agent2), map, constraintsB, h_values);
      double old_cost = get_cost(node, c.agent2);
      //c.path1 = new_pathA;
      //c.path2 = new_pathB;
      if(new_pathA.cost < 0 && new_pathB.cost < 0)
      {
        node.cost = -1;
        return;
      }
      else if (new_pathA.cost < 0)
      {
        c.overcost = new_pathB.cost - old_cost;
        cardinal_conflictsA.push_back(c);
      }
      else if (new_pathB.cost < 0)
      {
        c.overcost = new_pathA.cost - path.cost;
        cardinal_conflictsA.push_back(c);
      }
      else if(new_pathA.cost > path.cost && new_pathB.cost > old_cost)
      {
        c.overcost = std::min(new_pathA.cost - path.cost, new_pathB.cost - old_cost);
        cardinal_conflictsA.push_back(c);
      }
      else if(new_pathA.cost > path.cost || new_pathB.cost > old_cost)
        semicard_conflictsA.push_back(c);
      else
        conflictsA.push_back(c);
      low_level_searches += 2;
      low_level_expanded += (new_pathA.expanded + new_pathB.expanded);
    }
    else
    {
      constraintsA = get_constraints(&node, c.agent2);
      constraintsA.push_back(get_constraint(c.agent2, c.move2, c.move1));
      //cout<<"temp new cons 3: ";
      //prt_constraint(get_constraint(c.agent2, c.move2, c.move1));
      //cout<<"from move";
      //prt_move(c.move2);
      //prt_move(c.move1);
      //if (c.move2.id1==c.move2.id2 && c.move.id1==2)
      auto new_pathA = planner.find_path(task.get_agent(c.agent2), map, constraintsA, h_values);
      constraintsB = get_constraints(&node, c.agent1);
      constraintsB.push_back(get_constraint(c.agent1, c.move1, c.move2));
      //cout<<"temp new cons 4";
      //prt_constraint(get_constraint(c.agent1, c.move1, c.move2));
      auto new_pathB = planner.find_path(task.get_agent(c.agent1), map, constraintsB, h_values);
      double old_cost = get_cost(node, c.agent1);
      //c.path1 = new_pathB;
      //c.path2 = new_pathA;
      if(new_pathA.cost < 0 && new_pathB.cost < 0)
      {
        node.cost = -1;
        return;
      }
      else if (new_pathA.cost < 0)
      {
        c.overcost = new_pathB.cost - old_cost;
        cardinal_conflictsA.push_back(c);
      }
      else if (new_pathB.cost < 0)
      {
        c.overcost = new_pathA.cost - path.cost;
        cardinal_conflictsA.push_back(c);
      }
      else if(new_pathA.cost > path.cost && new_pathB.cost > old_cost)
      {
        c.overcost = std::min(new_pathA.cost - path.cost, new_pathB.cost - old_cost);
        cardinal_conflictsA.push_back(c);
      }
      else if(new_pathA.cost > path.cost || new_pathB.cost > old_cost)
        semicard_conflictsA.push_back(c);
      else
        conflictsA.push_back(c);
      low_level_searches += 2;
      low_level_expanded += (new_pathA.expanded + new_pathB.expanded);
    }
  }

  node.conflicts = conflictsA;
  node.semicard_conflicts = semicard_conflictsA;
  node.cardinal_conflicts = cardinal_conflictsA;
  node.conflicts_num = conflictsA.size() + semicard_conflictsA.size() + cardinal_conflictsA.size();
  return;
}

std::list<Constraint> CBS::get_constraints(CBS_Node *node, int agent_id)
{
  CBS_Node* curNode = node;
  std::list<Constraint> constraints(0);
  while(curNode->parent != nullptr)
  {

    //for (Constraint c:curNode->constraint){
    //	if(agent_id < 0 || c.agent == agent_id)
    //		constraints.push_back(c);
    //}
    if(agent_id < 0 || curNode->constraint.agent == agent_id)
      constraints.push_back(curNode->constraint);

    if(curNode->positive_constraint.agent == agent_id)
      constraints.push_back(curNode->positive_constraint);
    curNode = curNode->parent;
  }
  return constraints;
}

Conflict CBS::check_paths(const sPath &pathA, const sPath &pathB)
{
  unsigned int a(0), b(0);
  auto nodesA = pathA.nodes;
  auto nodesB = pathB.nodes;
  while(a < nodesA.size() - 1 || b < nodesB.size() - 1)
  {
    double dist = sqrt(pow(map->get_i(nodesA[a].id) - map->get_i(nodesB[b].id), 2) + pow(map->get_j(nodesA[a].id) - map->get_j(nodesB[b].id), 2)) - CN_EPSILON;
    if(a < nodesA.size() - 1 && b < nodesB.size() - 1) // if both agents have not reached their goals yet
    {
      if(dist < (nodesA[a+1].g - nodesA[a].g) + (nodesB[b+1].g - nodesB[b].g) + config.agent_size*2)
        if(check_conflict(Move(nodesA[a], nodesA[a+1]), Move(nodesB[b], nodesB[b+1]))){
          return Conflict(pathA.agentID, pathB.agentID, Move(nodesA[a], nodesA[a+1]), Move(nodesB[b], nodesB[b+1]), std::min(nodesA[a].g, nodesB[b].g));
        }
    }
    else if(a == nodesA.size() - 1) // if agent A has already reached the goal
    {
      if(dist < (nodesB[b+1].g - nodesB[b].g) + config.agent_size*2)
        if(check_conflict(Move(nodesA[a].g, CN_INFINITY, nodesA[a].id, nodesA[a].id), Move(nodesB[b], nodesB[b+1]))){
          return Conflict(pathA.agentID, pathB.agentID, Move(nodesA[a].g, CN_INFINITY, nodesA[a].id, nodesA[a].id), Move(nodesB[b], nodesB[b+1]), std::min(nodesA[a].g, nodesB[b].g));
        }
    }
    else if(b == nodesB.size() - 1) // if agent B has already reached the goal
    {
      if(dist < (nodesA[a+1].g - nodesA[a].g) + config.agent_size*2)
        if(check_conflict(Move(nodesA[a], nodesA[a+1]), Move(nodesB[b].g, CN_INFINITY, nodesB[b].id, nodesB[b].id))){
          return Conflict(pathA.agentID, pathB.agentID, Move(nodesA[a], nodesA[a+1]), Move(nodesB[b].g, CN_INFINITY, nodesB[b].id, nodesB[b].id), std::min(nodesA[a].g, nodesB[b].g));
        }
    }
    if(a == nodesA.size() - 1)
      b++;
    else if(b == nodesB.size() - 1)
      a++;
    else if(fabs(nodesA[a+1].g - nodesB[b+1].g) < CN_EPSILON)
    {
      a++;
      b++;
    }
    else if(nodesA[a+1].g < nodesB[b+1].g)
      a++;
    else if(nodesB[b+1].g - CN_EPSILON < nodesA[a+1].g)
      b++;
  }
  return Conflict();
}

std::vector<Conflict> CBS::get_all_conflicts(const std::vector<sPath> &paths, int id)
{
  std::vector<Conflict> conflicts;
  //check all agents
  if(id < 0)
    for(unsigned int i = 0; i < paths.size(); i++)
      for(unsigned int j = i + 1; j < paths.size(); j++)
      {
        Conflict conflict = check_paths(paths[i], paths[j]);
        if(conflict.agent1 >= 0)
          conflicts.push_back(conflict);
      }
  else
  {
    for(unsigned int i = 0; i < paths.size(); i++)
    {
      if(int(i) == id)
        continue;
      Conflict conflict = check_paths(paths[i], paths[id]);
      if(conflict.agent1 >= 0)
        conflicts.push_back(conflict);
    }
  }
  return conflicts;
}

double CBS::get_cost(CBS_Node node, int agent_id)
{
  while(node.parent != nullptr)
  {
    if(node.paths.begin()->agentID == agent_id)
      return node.paths.begin()->cost;
    node = *node.parent;
  }
  return node.paths.at(agent_id).cost;
}

std::vector<sPath> CBS::get_paths(CBS_Node *node, unsigned int agents_size)
{
  CBS_Node* curNode = node;
  std::vector<sPath> paths(agents_size);
  while(curNode->parent != nullptr)
  {
    if(paths.at(curNode->paths.begin()->agentID).cost < 0)
      paths.at(curNode->paths.begin()->agentID) = *curNode->paths.begin();
    curNode = curNode->parent;
  }
  for(unsigned int i = 0; i < agents_size; i++)
    if(paths.at(i).cost < 0)
      paths.at(i) = curNode->paths.at(i);
  return paths;
}

bool CBS::validNewNode(Vector2D node1,Vector2D node2,Vector2D New)
{
  if (New.i>node1.i && New.i>node2.i)		return false;
  if (New.i<node1.i && New.i<node2.i)		return false;
  if (New.j>node1.j && New.j>node2.j)		return false;
  if (New.j<node1.j && New.j<node2.j)		return false;
  return true;
}

void CBS::split_edge(Conflict conflict, std::vector<sPath> paths, Map_deltas &deltasR, Map_deltas &deltasL)
{
  int node11=conflict.move1.id1;
  int node12=conflict.move1.id2;
  int node21=conflict.move2.id1;
  int node22=conflict.move2.id2;

  if ((node11==node22) && (node12==node21)) //cross each other
    return;
  //prt_conflict(conflict);
  double r(config.agent_size);
  Vector2D New;
  int new_id;
  //cout<<"new node location:";
  std::ofstream output;
  output.open(config.node_file, std::ios_base::app);
  if (map->get_new_node_num()==11)
    1;
  if (node11==node12){
    double i0(map->get_i(node21)),j0(map->get_j(node21));
    double i1(map->get_i(node22)),j1(map->get_j(node22));
    double i2(map->get_i(node11)),j2(map->get_j(node11));
    // take care non waiting edge
    Vector2D P0(i0,j0),P1(i1,j1),P2(i2,j2);
    Vector2D temp(P1-P0);
    Vector2D v(temp/sqrt(temp*temp));
    double A(v*v),B(2*(i0*v.i-i2*v.i+j0*v.j-j2*v.j)),C((i2-i0)*(i2-i0)+(j2-j0)*(j2-j0)-4.41*r*r);
    double t=(-B-sqrt(B*B-4*A*C))/(2*A);
    if (t>-config.precision){
      Vector2D new_node(i0+v.i*t,j0+v.j*t);
      if (validNewNode(P0,P1,new_node)){
        New=Vector2D(map->fit2grid(new_node.i),map->fit2grid(new_node.j)); 
        new_id=map->add_node(New.i,New.j, node21, node22,conflict.agent2);
        if (new_id!=-1){
          output<<New.i<<" "<<New.j<<endl;
          deltasL.push_back(Map_delta(new_id,{node21,node22})); 
          h_values.add_node(new_id,conflict.agent2,node22);
          //cout<<" &&&:"<<new_id<<"@("<<New.i<<","<<New.j<<") a:"<<conflict.agent2;
        }
      }
    }

    //take care waiting node
    std::vector<Node> succ=map->get_valid_moves(node11,conflict.agent1);
    for (Node n: succ){
      int prev_node=n.id;
      assert(n.i!=-1 && n.j!=-1);
      Vector2D prevNode(n.i,n.j);
      temp=P2-prevNode;
      v=temp/sqrt(temp*temp);
      Vector2D d(v*2.1*r+config.precision);
      Vector2D new_node(P2-d);
      if (validNewNode(ind2Vec(prev_node),ind2Vec(node11),new_node)){
        New=Vector2D(map->fit2grid(new_node.i),map->fit2grid(new_node.j));
        new_id=map->add_node(New.i,New.j, prev_node, node11,conflict.agent1);
        if (new_id!=-1) {
          output<<New.i<<" "<<New.j<<endl;
          deltasR.push_back(Map_delta(new_id,{prev_node,node11})); 
          h_values.add_node(new_id,conflict.agent1,node11);
          //cout<<" &&&:"<<new_id<<"@("<<New.i<<","<<New.j<<") a:"<<conflict.agent1;
        }
      }
    }
    /*
       int prev_node=node11;
       for (sNode n: paths[conflict.agent1].nodes){
       if (n.g>=conflict.move1.t1-config.precision)
       break;
       prev_node=n.id;
       }
       if (prev_node==node11)
       return retval;
       double prevNodei0(map->get_i(prev_node)),prevNodej0(map->get_j(prev_node));
       Vector2D prevNode(prevNodei0,prevNodej0);
       temp=P2-prevNode;
       v=temp/sqrt(temp*temp);
       Vector2D d(v*2.1*r+config.precision);
       Vector2D new_node(P2-d);
       if (validNewNode(ind2Vec(prev_node),ind2Vec(node11),new_node)){
       New=Vector2D(map->fit2grid(new_node.i),map->fit2grid(new_node.j));
       int new_idA(map->add_node(New.i,New.j, prev_node, node11,conflict.agent1));
       if (new_idA!=-1) {
       retval.first=Map_delta(new_idA,{prev_node,node11}); 
       h_values.add_node(new_idA,conflict.agent1,node11);
    //cout<<" &&&:"<<new_idA<<"@("<<New.i<<","<<New.j<<") a:"<<conflict.agent1;
    }
    }
    */
  }
  else if (node21==node22){
    double i0(map->get_i(node11)),j0(map->get_j(node11));
    double i1(map->get_i(node12)),j1(map->get_j(node12));
    double i2(map->get_i(node21)),j2(map->get_j(node21));
    // take care non waiting edge
    Vector2D P0(i0,j0),P1(i1,j1),P2(i2,j2);
    Vector2D temp(P1-P0);
    Vector2D v(temp/sqrt(temp*temp));
    double A(v*v),B(2*(i0*v.i-i2*v.i+j0*v.j-j2*v.j)),C((i2-i0)*(i2-i0)+(j2-j0)*(j2-j0)-4.41*r*r);
    double t=(-B-sqrt(B*B-4*A*C))/(2*A);
    if (t>-config.precision){
      Vector2D new_node(i0+v.i*t,j0+v.j*t);
      if (validNewNode(ind2Vec(node11),ind2Vec(node12),new_node)){
        New=Vector2D(map->fit2grid(new_node.i),map->fit2grid(new_node.j)); 
        new_id=map->add_node(New.i,New.j, node11, node12,conflict.agent1);
        if (new_id!=-1) {
          output<<New.i<<" "<<New.j<<endl;
          deltasR.push_back(Map_delta(new_id,{node11,node12})); 
          h_values.add_node(new_id,conflict.agent1,node12);
          //cout<<" &&&:"<<new_id<<"@("<<New.i<<","<<New.j<<") a:"<<conflict.agent1;
        }
      }
    }

    //take care waiting node
    std::vector<Node> succ=map->get_valid_moves(node21,conflict.agent2);
    for (Node n: succ){
      int prev_node=n.id;
      assert(n.i!=-1 && n.j!=-1);
      Vector2D prevNode(n.i,n.j);
      temp=P2-prevNode;
      v=temp/sqrt(temp*temp);
      Vector2D d(v*2.1*r+config.precision);
      Vector2D new_node(P2-d);
      if (validNewNode(ind2Vec(prev_node),ind2Vec(node21),new_node)){
        New=Vector2D(map->fit2grid(new_node.i),map->fit2grid(new_node.j));
        new_id=map->add_node(New.i,New.j, prev_node, node21,conflict.agent2);
        if (new_id!=-1) {
          output<<New.i<<" "<<New.j<<endl;
          deltasL.push_back(Map_delta(new_id,{prev_node,node21})); 
          h_values.add_node(new_id,conflict.agent2,node21);
          //cout<<" &&&:"<<new_id<<"@("<<New.i<<","<<New.j<<") a:"<<conflict.agent1;
        }
      }
    }
    /*
       int prev_node=node21;
       for (sNode n: paths[conflict.agent2].nodes){
       if (n.g>=conflict.move2.t1-config.precision)
       break;
       prev_node=n.id;
       }
       if (prev_node==node21)
       return retval;
       double prevNodei0(map->get_i(prev_node)),prevNodej0(map->get_j(prev_node));
       Vector2D prevNode(prevNodei0,prevNodej0);
       temp=P2-prevNode;
       v=temp/sqrt(temp*temp);
       Vector2D d(v*2.1*r+config.precision);
       Vector2D new_node(P2-d);
       if (validNewNode(ind2Vec(prev_node),ind2Vec(node21),new_node)){
       New=Vector2D(map->fit2grid(new_node.i),map->fit2grid(new_node.j));
       int new_idB(map->add_node(New.i,New.j, prev_node, node21,conflict.agent2));
       if (new_idB!=-1) {
       retval.second=Map_delta(new_idB,{prev_node,node21}); 
       h_values.add_node(new_idB,conflict.agent2,node21);
    //cout<<" &&&:"<<new_idB<<"@("<<New.i<<","<<New.j<<") a:"<<conflict.agent2;
    }
    }
    */
  }
  else{
    double i0(map->get_i(node11)),j0(map->get_j(node11));
    double i1(map->get_i(node12)),j1(map->get_j(node12));
    double i2(map->get_i(node21)),j2(map->get_j(node21));
    double i3(map->get_i(node22)),j3(map->get_j(node22));

    double a0(j0-j1), b0(i1-i0), c0(i0*j1-i1*j0); //define the line
    double a1(j2-j3), b1(i3-i2), c1(i2*j3-i3*j2);
    if (b0==0 && b1==0){ //two vertical line
      output.close();
      return;
    }
    Vector2D Q((b0*c1-b1*c0)/(a0*b1-a1*b0) , (c0*a1-c1*a0)/(a0*b1-a1*b0)); //define intersection point
    double m0(-1*a0/b0),m1(-1*a1/b1);
    double theta;
    if (abs((m0)-(m1))<config.precision){
      output.close();
      return;
    }

    if (b0==0){
      theta=M_PI/2-atan(abs(m1));
    }
    else if (b1==0){
      theta=M_PI/2-atan(abs(m0));
    }
    else{
      theta=atan(abs((m1-m0)/(1+m0*m1)));		
    }

    Vector2D P0(i0,j0),P1(i1,j1),P2(i2,j2),P3(i3,j3);
    Vector2D temp(P1-P0);
    Vector2D V0(temp/sqrt(temp*temp)); //define unit dir vec
    temp= P3-P2;
    Vector2D V1(temp/sqrt(temp*temp));
    double d(2.1*r/sin(theta)+config.precision);//define waiting point
    Vector2D new_node1(Q-V0*d);
    Vector2D new_node2(Q-V1*d);


    if (validNewNode(ind2Vec(node11),ind2Vec(node12),new_node1)){
      New=Vector2D(map->fit2grid(new_node1.i),map->fit2grid(new_node1.j));
      new_id=map->add_node(New.i,New.j, node11, node12,conflict.agent1);
      if (new_id!=-1) {
        output<<New.i<<" "<<New.j<<endl;
        deltasR.push_back(Map_delta(new_id,{node11,node12})); 
        h_values.add_node(new_id,conflict.agent1,node12);
        //cout<<new_id<<"@("<<New.i<<","<<New.j<<") a:"<<conflict.agent1;
      }
    }

    if (validNewNode(ind2Vec(node21),ind2Vec(node22),new_node2)){
      New=Vector2D(map->fit2grid(new_node2.i),map->fit2grid(new_node2.j));
      new_id=map->add_node(New.i,New.j, node21, node22,conflict.agent2);
      if (new_id!=-1) {
        output<<New.i<<" "<<New.j<<endl;
        deltasL.push_back(Map_delta(new_id,{node21,node22})); 
        h_values.add_node(new_id,conflict.agent2,node22);
        //cout<<" &&&:"<<new_id<<"@("<<New.i<<","<<New.j<<") a:"<<conflict.agent2;
      }
    }
  }
  //cout<<endl;
  output.close();
}

Move CBS::modify_move(Move move,int new_id)
{
  if (move.id1==move.id2)
    return move;

  Move new_move(move);
  new_move.id1=new_id;
  Vector2D dis(map->get_i(new_id)-map->get_i(move.id1),map->get_j(new_id)-map->get_j(move.id1));

  new_move.t1+=sqrt(dis*dis);
  return new_move;
}

void CBS::prt_move(Move m)
{
  cout<<"{["<<m.id1<<"("<<map->get_i(m.id1)<<","<<map->get_j(m.id1)<<")->"
    <<m.id2<<"("<<map->get_i(m.id2)<<","<<map->get_j(m.id2)<<"] @:["
    <<m.t1<<"~"<<m.t2<<"]}"<<endl;
}

void CBS::prt_constraint(Constraint c)
{
  cout<<"Constraint  a:"<<c.agent<<" from:"<<c.id1<<"to:"<<c.to_id<<"[t:"<<c.t1<<"~"<<c.t2<<"]"<<endl;
}

void CBS::prt_constraints(std::list<Constraint> constraints)
{
  cout<<endl<<"full constraint table:"<<endl;
  for(Constraint c:constraints)
    prt_constraint(c);
}

void CBS::prt_path(sPath p)
{
  cout<<p.agentID<<":";
  for (sNode n:p.nodes){
    cout<<"("<<n.id<<","<<n.g<<")->";
  }
  cout<<endl;
}

void CBS::prt_paths(std::vector<sPath> paths)
{
  for(sPath p:paths){
    prt_path(p);
    //cout<<"--"<<endl;
  }
}

Constraint CBS::get_split_constraint(int agent, Move move1, Move move2)
{
  /*
     if(move2.t2 == CN_INFINITY) // no idea what's happening here
     return Constraint(agent, move1.t1, CN_INFINITY, move1.id1, move1.id2);

     if (move1.id2 == move2.id2){ //node conflict
                                  //Vector2D square_dis(map->get_i(move1.id1)-map->get_i(move1.id2),map->get_j(move1.id1)-map->get_j(move1.id2));
                                  //double travelTime(sqrt(square_dis*square_dis));
                                  Constraint(agent, move1.t1, move2.t2, move1.id1, move1.id2);
                                  }
                                  else{//edge conflict
                                  Constraint(agent, move1.t1, move2.t2, move1.id1, move1.id2);
                                  }
                                  */
  return Constraint(agent, move1.t1, move2.t2, move1.id1, move1.id2);

}

void CBS::prt_conflict(Conflict conflict)
{
  int node11=conflict.move1.id1;
  int node12=conflict.move1.id2;
  int node21=conflict.move2.id1;
  int node22=conflict.move2.id2;
  //cout<<"------------------"<<endl;
  cout<<"conflict: [id] (coord1->coord2) @[t]"<<endl;
  cout<<"[a"<<conflict.agent1<<":"<<node11<<"->"<<node12<<"] ("<<map->get_i(node11)<<","<<map->get_j(node11)<<") -> ("
    <<map->get_i(node12)<<","<<map->get_j(node12)<<") @["<<conflict.move1.t1<<"~"<<conflict.move1.t2<<"]"<<endl;
  cout<<"[a"<<conflict.agent2<<":"<<node21<<"->"<<node22<<"] ("<<map->get_i(node21)<<","<<map->get_j(node21)<<") -> ("
    <<map->get_i(node22)<<","<<map->get_j(node22)<<") @["<<conflict.move2.t1<<"~"<<conflict.move2.t2<<"]"<<endl;
  //"[move1_t:"<<conflict.move1.t1<<"~"<<conflict.move1.t2<<"] [move2_t:"<<conflict.move2.t1<<"~"<<conflict.move2.t2<<"]"<<endl;
}

void CBS::prt_conflicts(list<Conflict> conflicts) {
  for (Conflict con:conflicts){
    prt_conflict(con);
  }
}

void CBS::saveCT(const string &fileName, CBS_Node *goal_node, unsigned int agent_num) 
{ // write the CT to a file

  std::ofstream output;
  output.open(fileName, std::ios::out);
  output << "digraph G {" << endl;
  output << "size = \"5,5\";" << endl;
  output << "center = true;" << endl;
  for (CBS_Node node : tree.tree)
  {
    output << node.id << " [label=\"#" << node.id 
      << "\ng+h="<< node.cost-node.h<< "+" << node.h 
      << "\n"<<node.cur_conflict
      << node.constraint<<node.positive_constraint
      << "new_Node:";
    for (auto d:node.deltas)
      output<<d.add_node<<",";
    output<<"\n";
    //std::vector<sPath> allp = get_paths((&node), agent_num);
    //for (const sPath p : allp)
    //    output << p;
    output<<"\"]" << endl;
    if (node.parent==nullptr)
      continue;
    output << node.parent->id << " -> " << node.id<<endl;// << " [label=\"";
                                                         //for (const auto &constraint : node.constraint)
                                                         //output << node.constraint;
                                                         //output<<node.positive_constraint;
                                                         //output << "new_Node:"<<node.delta.add_node<<"\n";

                                                         //output << "\"]" << endl;
  }
  auto node = goal_node;
  while (node != nullptr)
  {
    output << node->id << " [color=red]" << endl;
    node = node->parent;
  }
  output << "}" << endl;
  output.close();

}

void CBS::gen_new_map(CBS_Node *node)
{
  stack<CBS_Node> infos;
  CBS_Node* curNode = node;
  while(curNode->parent != nullptr){
    infos.push(*curNode);
    curNode = curNode->parent;
  }
  CBS_Node tempNode;
  while (!infos.empty()){
    map->alter(infos.top().deltas);
    infos.pop();
  }

}

void CBS::gen_original_map(CBS_Node *node)
{
  CBS_Node* curNode = node;
  while (curNode->parent != nullptr){
    map->alter_back(curNode->deltas);
    curNode = curNode->parent;
  }
}

void CBS::prt_map_deltas(Map_deltas R, Map_deltas L)
{
  cout<<"R:"<<endl;
  for (Map_delta map_d: R)
    cout<<"("<<map_d.del_edge.first<<") -- ("<<map_d.add_node<<") -- ("<<map_d.del_edge.second<<")"<<endl;
  cout<<"L:"<<endl;
  for (Map_delta map_d: L)
    cout<<"("<<map_d.del_edge.first<<") -- ("<<map_d.add_node<<") -- ("<<map_d.del_edge.second<<")"<<endl;

}

Vector2D CBS::ind2Vec(int nodeId)
{
  return Vector2D(map->get_i(nodeId),map->get_j(nodeId));
}

void CBS::printBT(const std::string& prefix, const int node_id, bool isLeft)
{
  if (node_id==-1) return;
  tree_aux::iterator it;
  it=tree_info.find(node_id);
  CBS_Node_aux* node=(it->second);
    cout << prefix;
    cout << (isLeft ? "├──" : "└──");
    cout<<node->id<<" ";
    cout<<"g: "<<node->cost<<" ";
    cout<<node->constraint;
    /*
    if (node-> constraint!= nullptr) {
      cout<<"g: "<<node->cost<<" ";
      cout<<node->constraint;
    }
    else {
      std::cout << "no choosen conflict "<<" "<< node->cost << std::endl;
    }
*/
    printBT(prefix + (isLeft ? "│   " : "    "), node->id_left, true);
    printBT(prefix + (isLeft ? "│   " : "    "), node->id_right, false);
}

void CBS::printBT_aux() 
{
  printBT("",1,false);
}
