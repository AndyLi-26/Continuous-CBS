clc; clear; close all;
agent_size=0.5:1:4.5;
agent_num=5:24;
x=csvread("no_split.csv");
y=csvread("split.csv");
%%scatter plot
% for i =1:length(agent_size)
%     figure;hold on;
%     fh.WindowState = 'maximized';
%     sgtitle(sprintf("agent size=%.1f",agent_size(i)));
%     x_size=x(x(:,2)==agent_size(i),:);
%     y_size=y(y(:,2)==agent_size(i),:);
%     for j=1:length(agent_num)
%         x_temp=x_size(x_size(:,3)==agent_num(j),:);
%         y_temp=y_size(y_size(:,3)==agent_num(j),:);
%         x_temp(x_temp(:,4)>=300,5)=max(x_temp(:,5));
%         y_temp(y_temp(:,4)>=300,5)=max(y_temp(:,5));
%         x_temp=sortrows(x_temp);
%         y_temp=sortrows(y_temp);
%         subplot(4,5,j);
%         plot(x_temp(:,5),y_temp(:,5),'o');
%         title(sprintf("agent number: %d",agent_num(j)));
%     end
% end

%%cost difference and average cost
for i =1:length(agent_size)
    %figure;
    hold on;
    fh.WindowState = 'maximized';
    %sgtitle(sprintf("agent size=%.1f",agent_size(i)));
    x_size=x(x(:,2)==agent_size(i),:);
    y_size=y(y(:,2)==agent_size(i),:);
    avg_cost=zeros(2,length(agent_num));
    dif_cost=zeros(1,length(agent_num));
    for j=1:length(agent_num)
        x_temp=x_size(x_size(:,3)==agent_num(j),:);
        y_temp=y_size(y_size(:,3)==agent_num(j),:);
        x_temp=sortrows(x_temp);
        y_temp=sortrows(y_temp);
        idx=(x_temp(:,4)<300) & (y_temp(:,4)<300);
        x_temp=x_temp(idx,:);
        y_temp=y_temp(idx,:);
        avg_cost(1,j)=mean(x_temp(:,5));
        avg_cost(2,j)=mean(y_temp(:,5));
        dif_cost(j)=sum(x_temp(:,5))-sum(y_temp(:,5));
    end
    plot(agent_num,dif_cost);
    %plot(agent_num,avg_cost(1,:));
    %plot(agent_num,avg_cost(2,:));
    %legend("no split","split");
end
xlabel("agent number");
ylabel("cost improvements");
title("C_{no split} - C_{split} ")
legend("agent size=0.5","agent size=1.5","agent size=2.5","agent size=3.5","agent size=4.5")

%succ rate
% for i =1:length(agent_size)
%     figure;hold on;
%     fh.WindowState = 'maximized';
%     sgtitle(sprintf("agent size=%.1f",agent_size(i)));
%     x_size=x(x(:,2)==agent_size(i),:);
%     y_size=y(y(:,2)==agent_size(i),:);
%     succ_rate=zeros(2,length(agent_num));
%     
%     for j=1:length(agent_num)
%         x_temp=x_size(x_size(:,3)==agent_num(j),:);
%         y_temp=y_size(y_size(:,3)==agent_num(j),:);
%         x_temp=sortrows(x_temp);
%         y_temp=sortrows(y_temp);
%         succ_rate(1,j)=sum(x_temp(:,4)<300);
%         succ_rate(2,j)=sum(y_temp(:,4)<300);
%     end
%     plot(agent_num,succ_rate(1,:)/100);
%     plot(agent_num,succ_rate(2,:)/100);
%     legend("no split","split");
%     xlabel("agent_number");
%     ylabel("succ_rate");
% end