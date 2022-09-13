import random
def exportTask_XML(tasks,i):
	with open("tasks{}.xml".format(i),'w') as f:
		print('<?xml version="1.0" ?>\n<root>',file=f)
		for j in range(len(tasks)):
			print('   <agent start_id="{}" goal_id="{}"/>'.format(tasks[j][0],tasks[j][1]), file=f)
		print('</root>',file=f)
        
if __name__=="__main__":
    all=[]
    while len(all)<100:
        new=random.choices(range(30,130),k=30)
        if new not in all:
            all.append(new)
    
    for i,l in enumerate(all):
        tasks=[x for x in enumerate(l)]
        print(tasks)
        exportTask_XML(tasks,i)
    