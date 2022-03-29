import subprocess, os,glob, time
processPool=[]
exe = "../debug/CCBS"
map_address ="../Instances/roadmaps/{}/map.xml"
task_address="../Instances/roadmaps/{}/{}_task.xml"
config = "../Instances/roadmaps/configs/{}/config-{}.xml"

for m in ["sparse","dense","super-dense"]:
    for i in range(1,26):
        for a in range(2,36,2):
            cmd=[exe,map_address.format(m),
            task_address.format(m,i),
            config.format(m,a)]
            if (len(processPool)>=20):
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
             
                                            





