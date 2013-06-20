#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<map>
#include<list>
#include<set>
#include<queue>
#include<utility>
#include<vector>
#include<cmath>

using namespace std;

//置信水平取0.95时的卡方表
const double CHI[18]={0.004,0.103,0.352,0.711,1.145,1.635,2.167,2.733,3.325,3.94,4.575,5.226,5.892,6.571,7.261,7.962};
/*根据多维数组计算卡方值*/
template<typename Comparable>
double cal_chi(Comparable **arr,int row,int col){
    vector<Comparable> rowsum(row);
    vector<Comparable> colsum(col);
    Comparable totalsum=static_cast<Comparable>(0);
    //cout<<"observation"<<endl;
    for(int i=0;i<row;++i){
        for(int j=0;j<col;++j){
            //cout<<arr[i][j]<<"\t";
            totalsum+=arr[i][j];
            rowsum[i]+=arr[i][j];
            colsum[j]+=arr[i][j];
        }
        //cout<<endl;
    }
    double rect=0.0;
    //cout<<"exception"<<endl;
    for(int i=0;i<row;++i){
        for(int j=0;j<col;++j){
            double excep=1.0*rowsum[i]*colsum[j]/totalsum;
            //cout<<excep<<"\t";
            if(excep!=0)
                rect+=pow(arr[i][j]-excep,2.0)/excep;
        }
        //cout<<endl;
    }
    return rect;
}

class MyTriple{
public:
    double first;
    int second;
    int third;
    MyTriple(){
        first=0.0;
        second=0;
        third=0;
    }
    MyTriple(double f,int s,int t):first(f),second(s),third(t){}
    bool operator< (const MyTriple &obj) const{
        int cmp=this->first-obj.first;
        if(cmp>0)
            return false;
        else if(cmp<0)
            return true;
        else{
            cmp=obj.second-this->second;
            if(cmp<0)
                return true;
            else
                return false;
        }
    }
};

typedef map<string,int> MAP_REST_COUNT;
typedef map<string,MAP_REST_COUNT> MAP_ATTR_REST;
typedef vector<MAP_ATTR_REST> VEC_STATI;

const int ATTR_NUM=8;       //自变量的维度
vector<string> X(ATTR_NUM);
int rest_number;        //因变量的种类数，即类别数
vector<pair<string,int> > classes;      //把类别、对应的记录数存放在一个数组中
int total_record_number;        //总的记录数
vector<vector<string> > inputData;      //原始输入数据

class node{
public:
    node* parent;       //父节点
    node* leftchild;        //左孩子节点
    node* rightchild;       //右孩子节点
    string cond;        //分枝条件
    string decision;        //在该节点上作出的类别判定
    double precision;       //判定的正确率
    int record_number;      //该节点上涵盖的记录个数
    int size;       //子树包含的叶子节点的数目
    int index;      //层次遍历树，给节点标上序号
    double alpha;   //表面误差率的增加量
    node(){
        parent=NULL;
        leftchild=NULL;
        rightchild=NULL;
        precision=0.0;
        record_number=0;
        size=1;
        index=0;
        alpha=1.0;
    }
    node(node* p){
        parent=p;
        leftchild=NULL;
        rightchild=NULL;
        precision=0.0;
        record_number=0;
        size=1;
        index=0;
        alpha=1.0;
    }
    node(node* p,string c,string d):cond(c),decision(d){
        parent=p;
        leftchild=NULL;
        rightchild=NULL;
        precision=0.0;
        record_number=0;
        size=1;
        index=0;
        alpha=1.0;
    }
    void printInfo(){
        cout<<"index:"<<index<<"\tdecisoin:"<<decision<<"\tprecision:"<<precision<<"\tcondition:"<<cond<<"\tsize:"<<size;
        if(parent!=NULL)
            cout<<"\nparent index:"<<parent->index;
        if(leftchild!=NULL)
            cout<<"\tleftchild:"<<leftchild->index<<"\trightchild："<<rightchild->index;
        cout<<endl;
    }
    void printTree(){
        printInfo();
        if(leftchild!=NULL)
            leftchild->printTree();
        if(rightchild!=NULL)
            rightchild->printTree();
    }
};

int readInput(string filename){
    ifstream ifs(filename.c_str());
    if(!ifs){
        cerr<<"open inputfile failed!"<<endl;
        return -1;
    }
    map<string,int> catg;
    string line;
    getline(ifs,line);
    string item;
    istringstream strstm(line);
    strstm>>item;
    for(int i=0;i<X.size();++i){
        strstm>>item;
        X[i]=item;
    }
    while(getline(ifs,line)){
        vector<string> conts(ATTR_NUM+2);
        istringstream strstm(line);
        //strstm.str(line);
        for(int i=0;i<conts.size();++i){
            strstm>>item;
            conts[i]=item;
            if(i==conts.size()-1)
                catg[item]++;
        }
        inputData.push_back(conts);
    }
    total_record_number=inputData.size();
    ifs.close();
    map<string,int>::const_iterator itr=catg.begin();
    while(itr!=catg.end()){
        classes.push_back(make_pair(itr->first,itr->second));
        itr++;
    }
    rest_number=classes.size();
    return 0;
}

/*根据inputData作出一个统计stati*/
void statistic(vector<vector<string> > &inputData,VEC_STATI &stati){
    for(int i=1;i<ATTR_NUM+1;++i){
        MAP_ATTR_REST attr_rest;
        for(int j=0;j<inputData.size();++j){
            string attr_value=inputData[j][i];
            string rest=inputData[j][ATTR_NUM+1];
            MAP_ATTR_REST::iterator itr=attr_rest.find(attr_value);
            if(itr==attr_rest.end()){
                MAP_REST_COUNT rest_count;
                rest_count[rest]=1;
                attr_rest[attr_value]=rest_count;
            }
            else{
                MAP_REST_COUNT::iterator iter=(itr->second).find(rest);
                if(iter==(itr->second).end()){
                    (itr->second).insert(make_pair(rest,1));
                }
                else{
                    iter->second+=1;
                }
            }
        }
        stati.push_back(attr_rest);
    }
}

/*依据某条件作出分枝时，inputData被分成两部分*/
void splitInput(vector<vector<string> > &inputData,int fitIndex,string cond,vector<vector<string> > &LinputData,vector<vector<string> > &RinputData){
    for(int i=0;i<inputData.size();++i){
        if(inputData[i][fitIndex+1]==cond)
            LinputData.push_back(inputData[i]);
        else
            RinputData.push_back(inputData[i]);
    }
}

void printStati(VEC_STATI &stati){
    for(int i=0;i<stati.size();i++){
        MAP_ATTR_REST::const_iterator itr=stati[i].begin();
        while(itr!=stati[i].end()){
            cout<<itr->first;
            MAP_REST_COUNT::const_iterator iter=(itr->second).begin();
            while(iter!=(itr->second).end()){
                cout<<"\t"<<iter->first<<"\t"<<iter->second;
                iter++;
            }
            itr++;
            cout<<endl;
        }
        cout<<endl;
    }
}

void split(node *root,vector<vector<string> > &inputData,vector<pair<string,int> > classes){
    //root->printInfo();
    root->record_number=inputData.size();
    VEC_STATI stati;
    statistic(inputData,stati);
    //printStati(stati);
    //for(int i=0;i<rest_number;i++)
    //  cout<<classes[i].first<<"\t"<<classes[i].second<<"\t";
    //cout<<endl;
    /*找到最大化GINI指标的划分*/
    double minGain=1.0;     //最小的GINI增益
    int fitIndex=-1;
    string fitCond;
    vector<pair<string,int> > fitleftclasses;
    vector<pair<string,int> > fitrightclasses;
    int fitleftnumber;
    int fitrightnumber;
    for(int i=0;i<stati.size();++i){     //扫描每一个自变量
        MAP_ATTR_REST::const_iterator itr=stati[i].begin();
        while(itr!=stati[i].end()){         //扫描自变量上的每一个取值
            string condition=itr->first;     //判定的条件，即到达左孩子的条件
            //cout<<"cond 为"<<X[i]+condition<<"时：";
            vector<pair<string,int> > leftclasses(classes);     //左孩子节点上类别、及对应的数目
            vector<pair<string,int> > rightclasses(classes);    //右孩子节点上类别、及对应的数目
            int leftnumber=0;       //左孩子节点上包含的类别数目
            int rightnumber=0;      //右孩子节点上包含的类别数目
            for(int j=0;j<leftclasses.size();++j){       //更新类别对应的数目
                string rest=leftclasses[j].first;
                MAP_REST_COUNT::const_iterator iter2;
                iter2=(itr->second).find(rest);
                if(iter2==(itr->second).end()){      //没找到
                    leftclasses[j].second=0;
                    rightnumber+=rightclasses[j].second;
                }
                else{       //找到
                    leftclasses[j].second=iter2->second;
                    leftnumber+=leftclasses[j].second;
                    rightclasses[j].second-=(iter2->second);
                    rightnumber+=rightclasses[j].second;
                }
            }
            /**if(leftnumber==0 || rightnumber==0){
                cout<<"左右有一边为空"<<endl;

                for(int k=0;k<rest_number;k++)
                    cout<<leftclasses[k].first<<"\t"<<leftclasses[k].second<<"\t";
                cout<<endl;
                for(int k=0;k<rest_number;k++)
                    cout<<rightclasses[k].first<<"\t"<<rightclasses[k].second<<"\t";
                cout<<endl;
            }**/
            double gain1=1.0;           //计算GINI增益
            double gain2=1.0;
            if(leftnumber==0)
                gain1=0.0;
            else
                for(int j=0;j<leftclasses.size();++j)
                    gain1-=pow(1.0*leftclasses[j].second/leftnumber,2.0);
            if(rightnumber==0)
                gain2=0.0;
            else
                for(int j=0;j<rightclasses.size();++j)
                    gain2-=pow(1.0*rightclasses[j].second/rightnumber,2.0);
            double gain=1.0*leftnumber/(leftnumber+rightnumber)*gain1+1.0*rightnumber/(leftnumber+rightnumber)*gain2;
            //cout<<"GINI增益："<<gain<<endl;
            if(gain<minGain){
                //cout<<"GINI增益："<<gain<<"\t"<<i<<"\t"<<condition<<endl;
                fitIndex=i;
                fitCond=condition;
                fitleftclasses=leftclasses;
                fitrightclasses=rightclasses;
                fitleftnumber=leftnumber;
                fitrightnumber=rightnumber;
                minGain=gain;
            }
            itr++;
        }
    }

    /*计算卡方值，看有没有必要进行分裂*/
    //cout<<"按"<<X[fitIndex]+fitCond<<"划分，计算卡方"<<endl;
    int **arr=new int*[2];
    for(int i=0;i<2;i++)
        arr[i]=new int[rest_number];
    for(int i=0;i<rest_number;i++){
        arr[0][i]=fitleftclasses[i].second;
        arr[1][i]=fitrightclasses[i].second;
    }
    double chi=cal_chi(arr,2,rest_number);
    //cout<<"chi="<<chi<<" CHI="<<CHI[rest_number-2]<<endl;
    if(chi<CHI[rest_number-2]){      //独立，没必要再分裂了
        delete []arr[0];    delete []arr[1];    delete []arr;
        return;             //不需要分裂函数就返回
    }
    delete []arr[0];    delete []arr[1];    delete []arr;

    /*分裂*/
    root->cond=X[fitIndex]+"="+fitCond;      //root的分枝条件
    //cout<<"分类条件:"<<root->cond<<endl;
    node *travel=root;      //root及其祖先节点的size都要加1
    while(travel!=NULL){
        (travel->size)++;
        travel=travel->parent;
    }

    node *LChild=new node(root);        //创建左右孩子
    node *RChild=new node(root);
    root->leftchild=LChild;
    root->rightchild=RChild;
    int maxLcount=0;
    int maxRcount=0;
    string Ldicision,Rdicision;
    for(int i=0;i<rest_number;++i){      //统计哪种类别出现的最多，从而作出类别判定
        if(fitleftclasses[i].second>maxLcount){
            maxLcount=fitleftclasses[i].second;
            Ldicision=fitleftclasses[i].first;
        }
        if(fitrightclasses[i].second>maxRcount){
            maxRcount=fitrightclasses[i].second;
            Rdicision=fitrightclasses[i].first;
        }
    }
    LChild->decision=Ldicision;
    RChild->decision=Rdicision;
    LChild->precision=1.0*maxLcount/fitleftnumber;
    RChild->precision=1.0*maxRcount/fitrightnumber;

    /*递归对左右孩子进行分裂*/
    vector<vector<string> > LinputData,RinputData;
    splitInput(inputData,fitIndex,fitCond,LinputData,RinputData);
    //cout<<"左边inputData行数:"<<LinputData.size()<<endl;
    //cout<<"右边inputData行数:"<<RinputData.size()<<endl;
    split(LChild,LinputData,fitleftclasses);
    split(RChild,RinputData,fitrightclasses);
}

/*计算子树的误差代价*/
double calR2(node *root){
    if(root->leftchild==NULL)
        return (1-root->precision)*root->record_number/total_record_number;
    else
        return calR2(root->leftchild)+calR2(root->rightchild);
}

/*层次遍历树，给节点标上序号。同时计算alpha*/
void index(node *root,priority_queue<MyTriple> &pq){
    int i=1;
    queue<node*> que;
    que.push(root);
    while(!que.empty()){
        node* n=que.front();
        que.pop();
        n->index=i++;
        if(n->leftchild!=NULL){
            que.push(n->leftchild);
            que.push(n->rightchild);
            //计算表面误差率的增量
            double r1=(1-n->precision)*n->record_number/total_record_number;      //节点的误差代价
            double r2=calR2(n);
            n->alpha=(r1-r2)/(n->size-1);
            pq.push(MyTriple(n->alpha,n->size,n->index));
        }
    }
}

/*剪枝*/
void prune(node *root,priority_queue<MyTriple> &pq){
    MyTriple triple=pq.top();
    int i=triple.third;
    queue<node*> que;
    que.push(root);
    while(!que.empty()){
        node* n=que.front();
        que.pop();
        if(n->index==i){
            cout<<"将要剪掉"<<i<<"的左右子树"<<endl;
            n->leftchild=NULL;
            n->rightchild=NULL;
            int s=n->size-1;
            node *trav=n;
            while(trav!=NULL){
                trav->size-=s;
                trav=trav->parent;
            }
            break;
        }
        else if(n->leftchild!=NULL){
            que.push(n->leftchild);
            que.push(n->rightchild);
        }
    }
}

void test(string filename,node *root){
    ifstream ifs(filename.c_str());
    if(!ifs){
        cerr<<"open inputfile failed!"<<endl;
        return;
    }
    string line;
    getline(ifs,line);
    string item;
    istringstream strstm(line);     //跳过第一行
    map<string,string> independent;       //自变量，即分类的依据
    while(getline(ifs,line)){
        istringstream strstm(line);
        //strstm.str(line);
        strstm>>item;
        cout<<item<<"\t";
        for(int i=0;i<ATTR_NUM;++i){
            strstm>>item;
            independent[X[i]]=item;
        }
        node *trav=root;
        while(trav!=NULL){
            if(trav->leftchild==NULL){
                cout<<(trav->decision)<<"\t置信度:"<<(trav->precision)<<endl;;
                break;
            }
            string cond=trav->cond;
            string::size_type pos=cond.find("=");
            string pre=cond.substr(0,pos);
            string post=cond.substr(pos+1);
            if(independent[pre]==post)
                trav=trav->leftchild;
            else
                trav=trav->rightchild;
        }
    }
    ifs.close();
}

int main(){
    string inputFile="1.txt";
    readInput(inputFile);
    VEC_STATI stati;        //最原始的统计
    statistic(inputData,stati);

//  for(int i=0;i<classes.size();++i)
//      cout<<classes[i].first<<"\t"<<classes[i].second<<"\t";
//  cout<<endl;
    node *root=new node();
    split(root,inputData,classes);      //分裂根节点
    priority_queue<MyTriple> pq;
    index(root,pq);
    root->printTree();
    cout<<"剪枝前使用该决策树最多进行"<<root->size-1<<"次条件判断"<<endl;
    /**
    //检验一个是不是表面误差增量最小的被剪掉了
    while(!pq.empty()){
        MyTriple triple=pq.top();
        pq.pop();
        cout<<triple.first<<"\t"<<triple.second<<"\t"<<triple.third<<endl;
    }
    **/
    test(inputFile,root);

    prune(root,pq);
    cout<<"剪枝后使用该决策树最多进行"<<root->size-1<<"次条件判断"<<endl;
    test(inputFile,root);
    return 0;
}
