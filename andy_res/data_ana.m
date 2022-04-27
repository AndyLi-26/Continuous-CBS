clc; clear all; close all;
maps=["sparse","dense","super-dense"];
for m=1:3
    T = readtable(strcat("opt",maps(m),".txt"));
    run_t=zeros(1,17);suc=zeros(1,17);fail=zeros(1,17);
    for i=1:length(T.Var1)
        if T.Var2(i)<300
            run_t(T.Var1(i)/2)=run_t(T.Var1(i)/2)+T.Var2(i);
            suc(T.Var1(i)/2)=suc(T.Var1(i)/2)+1;
        else
            fail(T.Var1(i)/2)=fail(T.Var1(i)/2)+1;
        end
    end
    figure;
    run_t=run_t./suc;
    plot(2:2:34,suc/25);
    xticks(2:2:34)
    title(maps(m))
    xlabel("number of agents");
    ylabel("Success rate");
end
