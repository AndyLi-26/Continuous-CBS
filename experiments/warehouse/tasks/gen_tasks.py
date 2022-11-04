import random
def exportTask_XML(tasks,i):
	with open("tasks{}.xml".format(i),'w') as f:
		print('<?xml version="1.0" ?>\n<root>',file=f)
		for j in range(len(tasks)):
			print('   <agent start_id="{}" goal_id="{}"/>'.format(tasks[j][0],tasks[j][1]), file=f)
		print('</root>',file=f)
        
if __name__=="__main__":
    all=[]
    for i in range(100):
        task_list=[]
        temp=list(range(100))
        new=random.choices(temp,k=30)
        j=0
        while j<30:
            target=random.choice(temp)
            if new[j]!=target:
                temp.remove(target)
                task_list.append((new[j],target))
                j+=1
        all.append(task_list)
        print(*task_list,sep='\n')
        print('-----------------------')
    for i,l in enumerate(all):
        print(l)
        exportTask_XML(l,i)
