clc; clear;close all;
%files = dir('.\*.csv');
%final_result=zeros(length(files),10);
max_run_time=0;
m={'sparse';'dense';'super'};
a_size=[0.5,1.5,2.5,3.5,4.5];
for i =1:3
    fh=figure();
    fh.WindowState = 'maximized';
    sgtitle(m{i});
    for j = 1:5
        datas=zeros(4,500);%agent_num;instance;e0;e1
        a_num=5:14;
        for k=1:length(a_num)
            datas(1,(k-1)*50+1:k*50)=a_num(k)*ones(1,50);
        end
        temp=0:49;
        datas(2,:)=repmat(temp,1,length(a_num));
        
        srateVagent0=zeros(1,length(a_num));
        srateVagent1=zeros(1,length(a_num));
        costVagent=zeros(1,length(a_num));
        dcost=zeros(1,length(a_num));

        prefix0=[m{i},'\',num2str(a_size(j)),'\e0\'];
        prefix1=[m{i},'\',num2str(a_size(j)),'\e1\'];
        files0 = dir([prefix0,'*.csv']);
        files1 = dir([prefix1,'*.csv']);
        for k =1:length(files0)
            temp=csvread([prefix0,files0(k).name]);
            temp=sortrows(temp,1);
            invalid_ind=temp(:,4)>=180;
            temp(invalid_ind,5)=-1;
            pos=find(a_num==temp(1,3));
            datas(3,(pos-1)*50+1:pos*50)=temp(:,5)';
            srateVagent0(pos)=sum(temp(:,5)~=-1);
        end
        
        for k=1:length(files1)
            temp=csvread([prefix1,files1(k).name]);
            temp=sortrows(temp,1);
            invalid_ind=temp(:,4)>=180;
            temp(invalid_ind,5)=-1;
            pos=find(a_num==temp(1,3));
            datas(4,(pos-1)*50+1:pos*50)=temp(:,5)';
            srateVagent1(pos)=sum(temp(:,5)~=-1);
        end
        cpdatas=datas;
        subplot(3,5,j); hold on;
        title(sprintf("sactter agent size=%f",a_size(j)));
        datas(3,datas(3,:)==-1)=max(datas(3,:))+100;
        datas(4,datas(4,:)==-1)=max(datas(4,:))+100;
        plot(datas(3,:),datas(4,:),'ro');
        plot(datas(3,:),datas(3,:),'b-');

        subplot(3,5,j+5);hold on
        title(sprintf("success rate",a_size(j)));
        plot(a_num,srateVagent0);
        plot(a_num,srateVagent1);
        legend("without edge splitting","with edge splitting");
        xlabel("agent num");ylabel("solved instance")
        
        subplot(3,5,j+10);hold on
        title(sprintf("average cost improvements",a_size(j)));
        cpdatas=cpdatas(:,cpdatas(3,:)~=-1);
        cpdatas=cpdatas(:,cpdatas(4,:)~=-1);
        for k=1:length(a_num)
            temp=cpdatas(:,cpdatas(1,:)==a_num(k));
            dcost(k)=mean(temp(3,:)-temp(4,:));
        end
        plot(a_num,dcost);
        xlabel("agent num");ylabel("cost")
    end
end