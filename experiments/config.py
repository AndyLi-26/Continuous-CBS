s='''
<?xml version="1.0" ?>
<root>
	<algorithm>
		<use_cardinal>true</use_cardinal>
		<use_disjoint_splitting>false</use_disjoint_splitting>
		<hlh_type>2</hlh_type>
		<connectedness>2</connectedness>
		<focal_weight>1.0</focal_weight>
		<agent_size>4.5</agent_size>
		<timelimit>{}</timelimit>
		<precision>0.0000001</precision>
		<agent_number>{}</agent_number>
		<use_edge_split>{}</use_edge_split>
	</algorithm>
</root>
'''
if __name__=="__main__":
    tLim=[30,300]
    a_num=range(5,26)
    edgeSplit=['true','false']
    