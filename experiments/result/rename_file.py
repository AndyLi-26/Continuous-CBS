import os
directory = os.fsencode("./")
    
for file in os.listdir(directory):
     filename = os.fsdecode(file)
     if filename.endswith(".txt"): 
        os.rename(filename, filename.strip('.txt'))