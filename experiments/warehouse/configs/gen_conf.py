import os
sconf='''<?xml version="1.0" ?>
<root>
	<algorithm>
		<use_cardinal>true</use_cardinal>
		<use_disjoint_splitting>false</use_disjoint_splitting>
		<hlh_type>0</hlh_type>
		<connectedness>2</connectedness>
		<focal_weight>1.0</focal_weight>
		<agent_size>{}</agent_size>
		<timelimit>300</timelimit>
		<precision>0.0001</precision>
		<agent_number>{}</agent_number>
		<use_edge_split>{}</use_edge_split>
		<output_file>{}</output_file>
	</algorithm>
</root>
'''
if __name__=="__main__":

    for a_size in [4.5]:
        folder_name=str(a_size)
        if os.path.exists(folder_name):
            os.rmdir(folder_name)
        os.mkdir(folder_name)
        for e in ["split",'no_split']:
            folder_name="{}\{}".format(a_size,e)
            if os.path.exists(folder_name):
                os.rmdir(folder_name)
            os.mkdir(folder_name)
            for a_num in range(5,26):
                fn=folder_name+"\conf_{}.xml".format(a_num)
                with open(fn,'w') as f:
                    bool_split= "true" if e=="split" else "false"
                    optfn="{}.xml".format(e)
                    print(sconf.format(a_size,a_num,bool_split,optfn),file=f)
