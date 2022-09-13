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
    '''
    scens=os.listdir(mapName+"/scen-even/")
    for fn in scens:
        with open(mapName+'/scen-even/'+fn,'r') as f:
            f.readline()
            for l in f:
                info=list(map(int,l.split('\t')[4:-1]))
                print(info)
                imgArray[info[1],info[0],:]=[0,255,0]
                imgArray[info[3],info[2],:]=[255,0,0]
    '''
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
	
	print("Xs:",Vx_index)
	print("Ys:",Vy_index)
    #display
    print(imgArray.shape)
    imgplot = plt.imshow(imgArray)
    plt.show()
