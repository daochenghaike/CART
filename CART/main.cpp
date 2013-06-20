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

//����ˮƽȡ0.95ʱ�Ŀ�����
const double CHI[18]={0.004,0.103,0.352,0.711,1.145,1.635,2.167,2.733,3.325,3.94,4.575,5.226,5.892,6.571,7.261,7.962};
/*���ݶ�ά������㿨��ֵ*/
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

const int ATTR_NUM=8;       //�Ա�����ά��
vector<string> X(ATTR_NUM);
int rest_number;        //����������������������
vector<pair<string,int> > classes;      //����𡢶�Ӧ�ļ�¼�������һ��������
int total_record_number;        //�ܵļ�¼��
vector<vector<string> > inputData;      //ԭʼ��������

class node{
public:
    node* parent;       //���ڵ�
    node* leftchild;        //���ӽڵ�
    node* rightchild;       //�Һ��ӽڵ�
    string cond;        //��֦����
    string decision;        //�ڸýڵ�������������ж�
    double precision;       //�ж�����ȷ��
    int record_number;      //�ýڵ��Ϻ��ǵļ�¼����
    int size;       //����������Ҷ�ӽڵ����Ŀ
    int index;      //��α����������ڵ�������
    double alpha;   //��������ʵ�������
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
            cout<<"\tleftchild:"<<leftchild->index<<"\trightchild��"<<rightchild->index;
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

/*����inputData����һ��ͳ��stati*/
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

/*����ĳ����������֦ʱ��inputData���ֳ�������*/
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
    /*�ҵ����GINIָ��Ļ���*/
    double minGain=1.0;     //��С��GINI����
    int fitIndex=-1;
    string fitCond;
    vector<pair<string,int> > fitleftclasses;
    vector<pair<string,int> > fitrightclasses;
    int fitleftnumber;
    int fitrightnumber;
    for(int i=0;i<stati.size();++i){     //ɨ��ÿһ���Ա���
        MAP_ATTR_REST::const_iterator itr=stati[i].begin();
        while(itr!=stati[i].end()){         //ɨ���Ա����ϵ�ÿһ��ȡֵ
            string condition=itr->first;     //�ж������������������ӵ�����
            //cout<<"cond Ϊ"<<X[i]+condition<<"ʱ��";
            vector<pair<string,int> > leftclasses(classes);     //���ӽڵ�����𡢼���Ӧ����Ŀ
            vector<pair<string,int> > rightclasses(classes);    //�Һ��ӽڵ�����𡢼���Ӧ����Ŀ
            int leftnumber=0;       //���ӽڵ��ϰ����������Ŀ
            int rightnumber=0;      //�Һ��ӽڵ��ϰ����������Ŀ
            for(int j=0;j<leftclasses.size();++j){       //��������Ӧ����Ŀ
                string rest=leftclasses[j].first;
                MAP_REST_COUNT::const_iterator iter2;
                iter2=(itr->second).find(rest);
                if(iter2==(itr->second).end()){      //û�ҵ�
                    leftclasses[j].second=0;
                    rightnumber+=rightclasses[j].second;
                }
                else{       //�ҵ�
                    leftclasses[j].second=iter2->second;
                    leftnumber+=leftclasses[j].second;
                    rightclasses[j].second-=(iter2->second);
                    rightnumber+=rightclasses[j].second;
                }
            }
            /**if(leftnumber==0 || rightnumber==0){
                cout<<"������һ��Ϊ��"<<endl;

                for(int k=0;k<rest_number;k++)
                    cout<<leftclasses[k].first<<"\t"<<leftclasses[k].second<<"\t";
                cout<<endl;
                for(int k=0;k<rest_number;k++)
                    cout<<rightclasses[k].first<<"\t"<<rightclasses[k].second<<"\t";
                cout<<endl;
            }**/
            double gain1=1.0;           //����GINI����
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
            //cout<<"GINI���棺"<<gain<<endl;
            if(gain<minGain){
                //cout<<"GINI���棺"<<gain<<"\t"<<i<<"\t"<<condition<<endl;
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

    /*���㿨��ֵ������û�б�Ҫ���з���*/
    //cout<<"��"<<X[fitIndex]+fitCond<<"���֣����㿨��"<<endl;
    int **arr=new int*[2];
    for(int i=0;i<2;i++)
        arr[i]=new int[rest_number];
    for(int i=0;i<rest_number;i++){
        arr[0][i]=fitleftclasses[i].second;
        arr[1][i]=fitrightclasses[i].second;
    }
    double chi=cal_chi(arr,2,rest_number);
    //cout<<"chi="<<chi<<" CHI="<<CHI[rest_number-2]<<endl;
    if(chi<CHI[rest_number-2]){      //������û��Ҫ�ٷ�����
        delete []arr[0];    delete []arr[1];    delete []arr;
        return;             //����Ҫ���Ѻ����ͷ���
    }
    delete []arr[0];    delete []arr[1];    delete []arr;

    /*����*/
    root->cond=X[fitIndex]+"="+fitCond;      //root�ķ�֦����
    //cout<<"��������:"<<root->cond<<endl;
    node *travel=root;      //root�������Ƚڵ��size��Ҫ��1
    while(travel!=NULL){
        (travel->size)++;
        travel=travel->parent;
    }

    node *LChild=new node(root);        //�������Һ���
    node *RChild=new node(root);
    root->leftchild=LChild;
    root->rightchild=RChild;
    int maxLcount=0;
    int maxRcount=0;
    string Ldicision,Rdicision;
    for(int i=0;i<rest_number;++i){      //ͳ�����������ֵ���࣬�Ӷ���������ж�
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

    /*�ݹ�����Һ��ӽ��з���*/
    vector<vector<string> > LinputData,RinputData;
    splitInput(inputData,fitIndex,fitCond,LinputData,RinputData);
    //cout<<"���inputData����:"<<LinputData.size()<<endl;
    //cout<<"�ұ�inputData����:"<<RinputData.size()<<endl;
    split(LChild,LinputData,fitleftclasses);
    split(RChild,RinputData,fitrightclasses);
}

/*����������������*/
double calR2(node *root){
    if(root->leftchild==NULL)
        return (1-root->precision)*root->record_number/total_record_number;
    else
        return calR2(root->leftchild)+calR2(root->rightchild);
}

/*��α����������ڵ������š�ͬʱ����alpha*/
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
            //�����������ʵ�����
            double r1=(1-n->precision)*n->record_number/total_record_number;      //�ڵ��������
            double r2=calR2(n);
            n->alpha=(r1-r2)/(n->size-1);
            pq.push(MyTriple(n->alpha,n->size,n->index));
        }
    }
}

/*��֦*/
void prune(node *root,priority_queue<MyTriple> &pq){
    MyTriple triple=pq.top();
    int i=triple.third;
    queue<node*> que;
    que.push(root);
    while(!que.empty()){
        node* n=que.front();
        que.pop();
        if(n->index==i){
            cout<<"��Ҫ����"<<i<<"����������"<<endl;
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
    istringstream strstm(line);     //������һ��
    map<string,string> independent;       //�Ա����������������
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
                cout<<(trav->decision)<<"\t���Ŷ�:"<<(trav->precision)<<endl;;
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
    VEC_STATI stati;        //��ԭʼ��ͳ��
    statistic(inputData,stati);

//  for(int i=0;i<classes.size();++i)
//      cout<<classes[i].first<<"\t"<<classes[i].second<<"\t";
//  cout<<endl;
    node *root=new node();
    split(root,inputData,classes);      //���Ѹ��ڵ�
    priority_queue<MyTriple> pq;
    index(root,pq);
    root->printTree();
    cout<<"��֦ǰʹ�øþ�����������"<<root->size-1<<"�������ж�"<<endl;
    /**
    //����һ���ǲ��Ǳ������������С�ı�������
    while(!pq.empty()){
        MyTriple triple=pq.top();
        pq.pop();
        cout<<triple.first<<"\t"<<triple.second<<"\t"<<triple.third<<endl;
    }
    **/
    test(inputFile,root);

    prune(root,pq);
    cout<<"��֦��ʹ�øþ�����������"<<root->size-1<<"�������ж�"<<endl;
    test(inputFile,root);
    return 0;
}
