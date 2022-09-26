function load_newNode()
    hold on;
    new_nodes=readmatrix("node.txt");
    for i=1:size(new_nodes,1)
        newNode(new_nodes(i,1),new_nodes(i,2));
    end

end
