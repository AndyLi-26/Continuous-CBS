import sys
if __name__=="__main__":
    i=int(sys.argv[1]) if len(sys.argv)>1 else 0
    s='''    <edge id="e{}" source="n{}" target="n{}">
      <data key="key1">1</data>	
    </edge>'''
    with open("temp.xml",'w') as f:
        while 1:
            e1,e2=map(int,input("e0 e1:").split())
            print(s.format(i,e1,e2),file=f)
            print(s.format(i,e1,e2))
            i+=1
            print(s.format(i,e2,e1),file=f)
            print(s.format(i,e2,e1))
            i+=1
