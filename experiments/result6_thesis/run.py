import subprocess, os,glob, time
processPool=[]
exe = "../../debug/CCBS"
map_address ="../warehouse/warehouse_4.5_20_10.xml"
task_address="../warehouse/tasks/tasks{}.xml"
config = "../warehouse/configs/4.5/{}/conf_{}.xml"
#for m in ["sparse","dense","super-dense"]:
for e_sp in ["split","no_split"]:
    for a in range(5,25):
        for i in range(100):
            cmd=[exe,map_address,
            task_address.format(i),
            config.format(e_sp,a)]
            ''' 
            if (len(processPool)>=32):
                finish = False
                while not finish:
                    time.sleep(1)
                    for p in range(0,len(processPool)):
                        if p >= len(processPool):
                            break
                        if processPool[p].poll() is not None:
                            processPool.pop(p)
                            finish = True
                            p-=1
            else:
                for p in range(0,len(processPool)):
                        if p >= len(processPool):
                            break
                        if processPool[p].poll() is not None:
                            processPool.pop(p)
                            finish = True
                            p-=1
            '''
            print(subprocess.list2cmdline(cmd))
            '''
            try:
                processPool.append(subprocess.Popen(cmd))
            except:
                print(len(processPool))
                print(processPool)
            '''        






