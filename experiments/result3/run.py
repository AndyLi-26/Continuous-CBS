import subprocess, os,glob, time
processPool=[]
exe = "../../debug/CCBS"
map_address ="../{}/{}/map.xml"
task_address="../{}/{}/tasks/task{}.xml"
config = "../{}/{}/configs/{}/conf_{}.xml"
for a_size in [0.5,1.5,2.5,3.5,4.5]:
    for m in ["sparse","dense","super-dense"]:
        for e_sp in ["split","no_split"]:
            for a in range(5,15):
                for i in range(50):
                    cmd=[exe,map_address.format(a_size,m),
                    task_address.format(a_size,m,i),
                    config.format(a_size,m,e_sp,a)]
                    
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
                    
                    print(subprocess.list2cmdline(cmd))
                    try:
                        processPool.append(subprocess.Popen(cmd))
                    except:
                        print(len(processPool))
                        print(processPool)
                    






