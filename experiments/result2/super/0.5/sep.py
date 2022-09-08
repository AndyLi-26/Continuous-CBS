import os
if __name__=="__main__":
    a=os.listdir()
    os.mkdir("e0")
    os.mkdir("e1")
    for f in a:
        if "e0" in f:
            os.rename(f,"e0/"+f)
        elif "e1" in f:
            os.rename(f,"e1/"+f)