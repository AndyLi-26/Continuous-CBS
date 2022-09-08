clc; clear all; close all;
obj = VideoWriter("sparse_s_1.mp4", 'MPEG-4');
obj.Quality = 100;
obj.FrameRate = 20;
open(obj);
agent_size=0.5;
%draw map
global nodes resolution
resolution=0.05;
nodes=readmatrix('nodes.csv');
mapSize=[min(nodes,[],'all'),max(nodes,[],'all')];range=mapSize(2)-mapSize(1);
mapSize=[mapSize(1)-0.01*range,mapSize(2)+0.01*range];
edges=readmatrix('edges.csv');
summary=readmatrix('summary.txt');%run-time, SoC, makespan

figure('Renderer', 'painters', 'Position', [10 10 900 900]);
hold on; grid on; xlim(mapSize);ylim(mapSize);
xlabel(sprintf("run-time=%f,    SoC=%f,    makespan=%f",summary));
plot(nodes(:,1),nodes(:,2),'ro')
dcm = datacursormode;
dcm.Enable = 'on';
dcm.UpdateFcn = @displayind;
for i=1:size(edges,1)
    drawline(edges(i,1)+1,edges(i,2)+1,'r-')
end

%plot tasks
tasks=readmatrix('tasks.csv');
%agents=size(tasks,1);
a=dir(['paths\*.csv']);
agents=length(extractfield(a,"name"));
colors=jet(agents+1);
for i=1:agents
    plot(nodes(tasks(i,1)+1,1),nodes(tasks(i,1)+1,2),'.',MarkerFaceColor=colors(i,:),MarkerSize=10);
    text(nodes(tasks(i,1)+1,1),nodes(tasks(i,1)+1,2),num2str(i),'Color','black','FontSize',9)
end
for i=1:agents
    plot(nodes(tasks(i,2)+1,1),nodes(tasks(i,2)+1,2),'p',MarkerFaceColor=colors(i,:),MarkerSize=10);
end
f = getframe(gcf);
writeVideo(obj,f);

%gen animation
paths=cell(1,agents);
for i=1:agents
    info=readmatrix(['paths\',num2str(i-1),'.csv']);
    paths(i)={genPathMatrix(info)};
end

plots =plot([],[]);agentNum =plot([],[]);
for t=1:summary(3)/resolution
    title(sprintf("t=%.1f",t*resolution))
    delete(plots);delete(agentNum);
    plots=[];agentNum=[];
    for a=1:agents
        p=paths{a};
        if size(p,1)>t
            plots=[plots;circle(p(t,1),p(t,2),agent_size,colors(a,:))];
            agentNum=[agentNum,text(p(t,1),p(t,2),num2str(a),'Color','black','FontSize',11)];
        else
            plots=[plots;circle(p(end,1),p(end,2),agent_size,colors(a,:))];
            agentNum=[agentNum,text(p(end,1),p(end,2),num2str(a),'Color','black','FontSize',11)];
        end
    end
    drawnow;
    f = getframe(gcf);
    writeVideo(obj,f);
end
f = getframe(gcf);
obj.close();

function out = circle(x0,y0,radius,color)
pos = [[x0,y0]-radius 2*radius 2*radius];
out = rectangle('Position',pos,'Curvature',[1 1], 'FaceColor', color, 'Edgecolor','none');
end
function M=genPathMatrix(info)
    global resolution
    M=zeros(ceil(info(1,5)/resolution),2);
    Y=[0;ceil(cumsum(info(2:end,5))/resolution)];
    for i=2:size(info,1)
        x=linspace(info(i,1),info(i,3),Y(i)-Y(i-1));
        y=linspace(info(i,2),info(i,4),Y(i)-Y(i-1));
        M(Y(i-1)+1:Y(i),1)=x;
        M(Y(i-1)+1:Y(i),2)=y;
    end
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

