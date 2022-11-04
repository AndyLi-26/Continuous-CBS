import sys
def exportNode(f,nodes):
    s='''    <node id="n{}">
      <data key="key0">{},{}</data>
    </node>'''
    for i,(x,y) in enumerate(nodes):
        print(s.format(i,x,y),file=f)

def exportEdge(f,edges):
    s='''    <edge id="e{}" source="n{}" target="n{}">
      <data key="key1">1</data>
    </edge>'''
    
    for i,(n1,n2) in enumerate(edges):
        print(s.format(i,n1,n2),file=f)

def export(f,nodes,edges):
    header='''<?xml version="1.0" encoding="UTF-8"?>
<graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
  <key id="key0" for="node" attr.name="coords" attr.type="string" />
  <key id="key1" for="edge" attr.name="weight" attr.type="double" />
  <graph id="G" edgedefault="directed" parse.nodeids="free" parse.edgeids="canonical" parse.order="nodesfirst">'''
    end='''  </graph>
</graphml>'''
    print(header,file=f)
    exportNode(f,nodes)
    exportEdge(f,edges)
    print(end,file=f)
  
if __name__=="__main__":
    a_size=5
    #if len(sys.argv)==2:
    #    warehouse_size=(10,20)
    #else:
    #    warehouse_size=(int(sys.argv[2]),int(sys.argv[3]))
    warehouse_size=(20,10)
    node=[]
    edge=[]
    x_offset=y_offset=2*a_size
    '''
    #create node for leftdocking
    for x in range(3):
        for y in range(10):
            x_pos=round(x_offset+(x+1)*a_size,2)
            y_pos=round(y_offset+(y+1)*a_size,2)
            node.append((x_pos,y_pos))
            if y!=0:
                edge.append((len(node)-1,len(node)-2))
                edge.append((len(node)-2,len(node)-1))
            if x!=0:
                edge.append((len(node)-1,len(node)-11))
                edge.append((len(node)-11,len(node)-1))
    #create left intersection point
    x_pos=x_offset+3*a_size+5*a_size
    y_pos=y_offset+5.5*a_size
    node.append((x_pos,y_pos))
    for i in range(20,30):
        edge.append((i,30))
        edge.append((30,i))
        
    x_offset=x_pos+5*a_size
    intersection=30
    '''
    #x_offset=x_offset+3*a_size+5*a_size
    #create warehouse
    for x in range(warehouse_size[1]):
        for y in range(warehouse_size[0]):
            x_pos=round(x_offset+6*(x+1)*a_size,2)
            y_pos=round(y_offset+6*(y+1)*a_size,2)
            node.append((x_pos,y_pos))
            if y!=0:
                edge.append((len(node)-1,len(node)-2))
                edge.append((len(node)-2,len(node)-1))
            if x!=0:
                edge.append((len(node)-1,len(node)-1-warehouse_size[0]))
                edge.append((len(node)-1-warehouse_size[0],len(node)-1))
            #else:
            #    for i in range(20,30):
            #        edge.append((i,len(node)-1))
            #        edge.append((len(node)-1,i))
    
    #display+export
    print("agent size:",a_size)
    print("nodes:")
    [print(i,n) for i,n in enumerate(node)]
    print("edge:")
    [print(i,n) for i,n in enumerate(edge)]
    fn="warehouse_4.5_20_10.xml"
    with open(fn,'w') as f:
        export(f,node,edge)    
