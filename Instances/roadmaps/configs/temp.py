if __name__=="__main__":
    txt=r'''<?xml version="1.0" ?>
    <root>
	<algorithm>
		<use_cardinal>true</use_cardinal>
		<use_disjoint_splitting>true</use_disjoint_splitting>
		<hlh_type>2</hlh_type>
		<connectedness>3</connectedness>
		<focal_weight>1.0</focal_weight>
		<agent_size>0.353553</agent_size>
		<timelimit>300</timelimit>
		<precision>0.0000001</precision>
		<agent_number>{}</agent_number>
		<output_file>opt{}.txt</output_file>
	</algorithm>
    </root>
    '''
    for s in ["sparse","dense","super-dense"]:
        for i in range(2,36,2):
            with open(".\{}\config-{}.xml".format(s,i),"w") as f:
                print(txt.format(i,s),file=f)
        
