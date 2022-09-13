import os
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.image as mpimg
if __name__=="__main__":
    mapName="warehouse-10-20-10-2-1"
    #read map
    with open(mapName+'/'+mapName+".map",'r') as f:
        f.readline()
        h=int(f.readline().split(' ')[1])
        w=int(f.readline().split(' ')[1])
        f.readline()
        mapArray=np.zeros([h,w,1])
        Vx=set(range(w))
        Vy=set(range(h))
        imgArray=np.ones([h,w,3])*255
        for i,l in enumerate(f):
            for j,c in enumerate(l):
                if c=='T':
                    imgArray[i,j,:]=[0,0,0]
                    mapArray[i,j]=1
                    if i in Vy and j!=0 and j!=w-1:
                        Vy.remove(i)
                    if j in Vx and i!=0 and i!=h-1:
                        Vx.remove(j)
    print(Vx,Vy)
    #create potential x and y location
    Vx_ori=list(Vx)
    Vx=list(Vx)
    Vy=list(Vy)
    curNode=[Vx.pop(0)]
    Vx_index=[]
    Vy_index=[]
    while Vx:
        curIndex=Vx.pop(0)
        if abs(curIndex-curNode[-1])>1:
            x=sum(curNode)/len(curNode)
            Vx_index.append(x)
            curNode=[curIndex]
        else:
            curNode.append(curIndex)
            
    if curNode:
        Vx_index.append(sum(curNode)/len(curNode))
        
    prev=Vx_ori[0]
    for i in Vx_ori[1:]:
        if i-prev!=1:
            break
        prev=i
    
    Vx_index.insert(1,prev)
    
    Vx_ori=Vx_ori[::-1]
    prev=Vx_ori[0]
    for i in Vx_ori[1:]:
        if i-prev!=-1:
            break
        prev=i
    
    Vx_index.insert(len(Vx_index)-1,prev)

    curNode=[Vy.pop(0)]        
    while Vy:
        curIndex=Vy.pop(0)
        if abs(curIndex-curNode[-1])>1:
            y=sum(curNode)/len(curNode)
            Vy_index.append(y)
            curNode=[curIndex]
        else:
            curNode.append(curIndex)
            
    if curNode:
        Vy_index.append(sum(curNode)/len(curNode))
    
    
    
