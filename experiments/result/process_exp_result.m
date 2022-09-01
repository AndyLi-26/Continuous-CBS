clc; clear;close all;
files = dir('.\*.csv');
final_result=zeros(length(files),10);
max_run_time=0;
for i=1:length(files)
    %fprintf("name:%s\n",files(i).name)
    temp=csvread(files(i).name);
    new_temp=temp(temp(:,4)~=-1,:);
    failed_num=150-size(new_temp,1);
    new_temp=new_temp(new_temp(:,3)<300,:);
    info=split(files(i).name,'-');
    a_num=regexp(info(2),'\d*','Match');
    a_num=str2num(a_num{1}{1});
    e=regexp(info(3),'\d*','Match'); 
    e=str2num(e{1}{1});
    a=[a_num,e,size(new_temp,1),mean(new_temp,1)];
    final_result(i,:)=a;
    final_result(i,10)=failed_num;
    tempmax=max(new_temp(:,3));
    max_run_time=max([max_run_time,tempmax]);
end
%final_result=final_result(:,1:end-1);
title("difference between two algorithm")
for i =1:5
    subplot(3,2,i);hold on;
    title(sprintf("agent size=%.1f",i-0.5))
    tempData=final_result(final_result(:,4)==(i-0.5),:);
    %plot split
    sData=tempData(tempData(:,2)==1,:);
    sData=sortrows(sData);
    %plot(sData(:,1),sData(:,7));
    %plot non-split
    nonData=tempData(tempData(:,2)==0,:);
    nonData=sortrows(nonData);
    plot(nonData(:,1),nonData(:,7)-sData(:,7));
    plot([5,14],[0,0],'r-')
    %legend("with edge spllitting","no edge splitting","Location","best");
    xlabel("agent num");    ylabel("cost");
end