from random import shuffle

def exportTask_XML(tasks,num):
    for i in range(num):
        shuffle(tasks)
        with open("tasks/task{}.xml".format(i),'w') as f:
            print('<?xml version="1.0" ?>\n<root>',file=f)
            for s,t in tasks:
                print('   <agent start_id="{}" goal_id="{}"/>'.format(s,t), file=f)
            print('</root>',file=f)

if __name__=="__main__":
    tasks=[]
    with open("all_tasks.csv",'r') as f:
        for line in f:
            tasks.append(tuple(map(int,line.strip().split(','))))
    
    exportTask_XML(tasks,30)