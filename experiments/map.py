import sys,shutil,os
from random import choices
sconf='''<?xml version="1.0" ?>
<root>
	<algorithm>
		<use_cardinal>true</use_cardinal>
		<use_disjoint_splitting>false</use_disjoint_splitting>
		<hlh_type>0</hlh_type>
		<connectedness>2</connectedness>
		<focal_weight>1.0</focal_weight>
		<agent_size>{}</agent_size>
		<timelimit>180</timelimit>
		<precision>0.0000001</precision>
		<agent_number>{}</agent_number>
		<use_edge_split>{}</use_edge_split>
		<output_file>{}</output_file>
	</algorithm>
</root>
'''

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

def allow(nodesToCheck,newNode,nodes,agent_size):
	sqrdist=lambda a,b:(a[0]-b[0])**2+(a[1]-b[1])**2
	for n in nodesToCheck:
		if sqrdist(nodes[n],nodes[newNode])<=(2*agent_size)**2:
			return False
	return True
	
def exportTask_XML(agent_size,tasks,i,fn):
	with open(str(agent_size)+'\\'+fn+'\\'+"tasks\\task{}.xml".format(i),'w') as f:
		print('<?xml version="1.0" ?>\n<root>',file=f)
		for j in range(len(tasks[0])):
			print('   <agent start_id="{}" goal_id="{}"/>'.format(tasks[0][j],tasks[1][j]), file=f)
		print('</root>',file=f)
			
def exportTaskCSV(tasks):
	with open("all_tasks.csv",'w') as f:
		[print(*i,sep=',',file=f) for i in tasks]

if __name__=="__main__":
	for agent_size in [0.5,1.0,1.5,2.0,2.5,3.0,3.5,4.0,4.5]:
		shutil.copytree("./backup","./{}".format(agent_size))
		print(agent_size)
		
		##gen node list
		for fn in ["sparse","dense","super-dense"]:
			print(fn)
			##gen configs
			if os.path.exists("{}\{}\configs".format(agent_size,fn)):
				os.rmdir("{}\{}\configs".format(agent_size,fn))
			os.mkdir("{}\{}\configs".format(agent_size,fn))
			
			if os.path.exists("{}\{}\\tasks".format(agent_size,fn)):
				os.rmdir("{}\{}\\tasks".format(agent_size,fn))
			os.mkdir("{}\{}\\tasks".format(agent_size,fn))
			for config in ["split","no_split"]:
				if os.path.exists("{}\{}\configs\{}".format(agent_size,fn,config)):
					os.rmdir("{}\{}\configs\{}".format(agent_size,fn,config))
				os.mkdir("{}\{}\configs\{}".format(agent_size,fn,config))
			
				a_num=range(5,26)
				edgeSplit="true" if config=="split" else "false"
				for a in a_num:
					with open("{}\{}\configs\{}\conf_{}.xml".format(agent_size,fn,config,a),"w") as f:
						optName="{}-a{}-num{}-e{}-{}.csv".format(fn,agent_size,a,int(config=="split"),fn)
						print(sconf.format(round(agent_size,1),a,edgeSplit,optName),file=f)
			with open(str(agent_size)+'\\'+fn+'\\'+"map.xml",'r') as f:
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
			
			for i in range(50):
				print(i,end=',')
				timeout=0
				tasks=[[],[]]
				while 1:#len(tasks[0])<105:
					task=choices(nodesInd,k=2)
					if task[0] not in tasks[0] and task[1] not in tasks[1] and allow(tasks[0],task[0],nodes,agent_size) and allow(tasks[1],task[1],nodes,agent_size):
						tasks[0].append(task[0])
						tasks[1].append(task[1])
						
						timeout=0
					else:
						timeout+=1
					if len(tasks[0])>80 or timeout==100000:
						break
				exportTask_XML(agent_size,tasks,i,fn)
		
