import sys
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

if __name__=="__main__":
    nodes=[]
    edge=[]
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
    
    f_opt=sys.argv[1][:-3]+"xml"
    with open(f_opt,'w') as f:
        print(Sbegin,file=f)
        
        for i in nodes:
            print(Snode.format(*i),file=f)
        
        for i,e in enumerate(edge):
            print(Sedge.format(i,*e),file=f)

        print(Send,file=f)
