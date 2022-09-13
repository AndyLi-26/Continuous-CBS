import os
if __name__=="__main__":
    
    for m in ["super","sparse","dense"]:
        os.mkdir(m)
        dirM=os.listdir()
        for f in dirM:
            if m in f and ".csv" in f:
                os.rename(f,m+"/"+f)
        
    for m in ["super","sparse","dense"]:
        for a_size in [str(i-0.5) for i in range(1,6)]:
            os.mkdir(m+"/"+a_size)
            dirM=os.listdir("./"+m+'/')
            for f in dirM:
                if a_size in f  and ".csv" in f:
                    os.rename(m+'/'+f,m+'/'+a_size+"/"+f)
        
    for m in ["super","sparse","dense"]:
        for a_size in [str(i-0.5) for i in range(1,6)]:
            for e in ["e0","e1"]:
                dirM=os.mkdir("./"+m+'/'+a_size+'/'+e)
                dirA=os.listdir('./'+m+'/'+a_size+'/')
                for f in dirA :
                    if e in f and ".csv" in f:
                        os.rename(m+'/'+a_size+"/"+f,m+'/'+a_size+"/"+e+'/'+f)
