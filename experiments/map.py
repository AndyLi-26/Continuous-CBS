import sys
from random import choices
global agent_size
agent_size=4.5
def remove_rubbish(s):
    x=s.index("<node")
    s=s[x:]
    s=s.strip()
    return s
    
def parserNode(info):
    b=info.index('>')
    b1=info[:b+1]
    b2=info[b+1:].strip()
    i=int(b1[11:-2])
    coord=tuple(map(float,b2[17:-7].split(',')))
    return coord

def allow(nodesToCheck,newNode,nodes):
    global agent_size
    sqrdist=lambda a,b:(a[0]-b[0])**2+(a[1]-b[1])**2
    for n in nodesToCheck:
        if sqrdist(nodes[n],nodes[newNode])<=(2*agent_size)**2:
            return False
    return True
    
def exportTaskCSV(tasks):
    with open("all_tasks.csv",'w') as f:
        [print(*i,sep=',',file=f) for i in tasks]

if __name__=="__main__":
    ##gen node list
    with open(sys.argv[1],'r') as f:
        s=f.read().strip()
    s=remove_rubbish(s)
    nodesstr=list(map(lambda x:x.strip(),s.split("</node>")))
    nodesstr=nodesstr[:-1]
    nodes=[]
    for info in nodesstr:
        nodes.append(parserNode(info))
    
    ##gen tasks
    tasks=[[],[]]
    nodesInd=list(range(len(nodes)))
    timeout=0
    while 1:#len(tasks[0])<105:
        task=choices(nodesInd,k=2)
        if task[0] not in tasks[0] and task[1] not in tasks[1] and allow(tasks[0],task[0],nodes) and allow(tasks[1],task[1],nodes):
            tasks[0].append(task[0])
            tasks[1].append(task[1])
            print(len(tasks[0]))
            timeout=0
        else:
            timeout+=1
        if timeout==100000:
            break
    
    tasks=[(tasks[1][i],tasks[0][i]) for i in range(len(tasks[0]))]
    tasks=sorted(tasks, key=lambda x:x[0])
    
    exportTaskCSV(tasks)