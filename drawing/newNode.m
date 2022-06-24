function newNode(x,y)
    global nodes
    plot(x,y,'bo');
    nodes=[nodes;x,y];
end