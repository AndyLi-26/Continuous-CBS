import sys
from random import choices
Sbegin='''<?xml version="1.0" encoding="UTF-8"?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
<key id="key0" for="node" attr.name="coords" attr.type="string" />
  <key id="key1" for="edge" attr.name="weight" attr.type="double" />
  <graph id="G" edgedefault="directed" parse.nodeids="free" parse.edgeids="canonical" parse.order="nodesfirst">'''
Send='''  </graph>
</graphml>'''
Snode='''    <node id="n{}">
      <data key="key0">{},{}</data>
    </node>'''
Sedge='''    <edge id="e{}" source="n{}" target="n{}">
      <data key="key1">1</data>
    </edge>'''
    
def allow(nodesToCheck,newNode,nodes,agent_size):
	sqrdist=lambda a,b:(a[0]-b[0])**2+(a[1]-b[1])**2
	for n in nodesToCheck:
		if sqrdist(nodes[n],nodes[newNode])<=(2*agent_size)**2:
			return False
	return True
    
if __name__=="__main__":
    nodes=[]
    edge=[]
    agent_size=4.5
    with open(sys.argv[1],'r') as f:
        f.readline()
        for i in range(int(f.readline())):
            info=tuple(map(int,f.readline().split(' ')))
            assert info[0]==i,"wrong index"
            nodes.append(info)
        
        for i in range(int(f.readline())):
            info=tuple(map(int,f.readline().split(' ')[:-1]))
            edge.append((info[0],info[1]))
            edge.append((info[1],info[0]))
    
    for i in range(100):
        tasks=[[],[]]
        timeout=0
        nodesInd=list(range(len(nodes)))
        
        while 1:
            task=choices(nodesInd,k=2)
            if task[0] not in tasks[0] and task[1] not in tasks[1] and allow(tasks[0],task[0],nodes,agent_size) and allow(tasks[1],task[1],nodes,agent_size):
                tasks[0].append(task[0])
                tasks[1].append(task[1])
                timeout=0
            else:
                timeout+=1
                
            if len(tasks[0])>80 or timeout==100000:
                break
                
        with open("task{}.xml".format(i),'w') as f:
            print('<?xml version="1.0" ?>\n<root>',file=f)
            for j in range(len(tasks[0])):
                print('   <agent start_id="{}" goal_id="{}"/>'.format(tasks[0][j],tasks[1][j]), file=f)
            print('</root>',file=f)
            
    f_opt=sys.argv[1][:-3]+"xml"
    with open(f_opt,'w') as f:
        print(Sbegin,file=f)
        
        for i in nodes:
            print(Snode.format(*i),file=f)
            
        for i,e in enumerate(edge):
            print(Sedge.format(i,*e),file=f)

        print(Send,file=f)
        
    
