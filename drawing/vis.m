clc; clear all; close all;
global nodes
nodes=readmatrix('nodes.csv');
edges=readmatrix('edges.csv');
figure; hold on; grid on;
plot(nodes(:,1),nodes(:,2),'ro')
dcm = datacursormode;
dcm.Enable = 'on';
dcm.UpdateFcn = @displayind;
for i=1:size(edges,1)
    drawline(edges(i,1)+1,edges(i,2)+1,'r-')
end



function drawline(s,t,spec)
    global nodes
    xs=[nodes(s,1),nodes(t,1)];ys=[nodes(s,2),nodes(t,2)];
    plot(xs,ys,spec);
end

function txt=displayind(~,info)
    global nodes
    x = info.Position(1);
    y = info.Position(2);
    txt=num2str(find(ismember(nodes, [x y],'rows'))-1);
end

