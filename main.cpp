#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include<numeric>

using namespace std;

class Pos3{
public:
    double x;
    double y;
    double z;
    double *position;
    Pos3();
    Pos3(double a,double b,double c);
    void update(void);
    Pos3 operator+(Pos3 &p2);
    Pos3 operator-(Pos3 &p2);
    Pos3 operator*(double n);
    void operator+=(Pos3 &p2);
    void operator-=(Pos3 &p2);
    void operator*=(double n);
    void show(void);
    double* pos(void);
    void SwitchYZ(void);
};

Pos3::Pos3(){
    this->x=0;
    this->y=0;
    this->z=0;
    position=new double[3]{x,y,z};
}

Pos3::Pos3(double a, double b,double c){
    this->x=a;
    this->y=b;
    this->z=c;
    position=new double[3]{x,y,z};
}

void Pos3::update(void){
    position[0]=x;
    position[1]=y;
    position[2]=z;
}

Pos3 Pos3::operator+(Pos3 &p2){
    return Pos3(x+p2.x,y+p2.y,z+p2.z);
}

Pos3 Pos3::operator-(Pos3 &p2){
    return Pos3(x-p2.x,y-p2.y,z-p2.z);
}

Pos3 Pos3::operator*(double n){
    return Pos3(x*n,y*n,z*n);
}

void Pos3::operator+=(Pos3 &p2){
    x+=p2.x;
    y+=p2.y;
    z+=p2.z;
    update();
}

void Pos3::operator-=(Pos3 &p2){
    x-=p2.x;
    y-=p2.y;
    z-=p2.z;
    update();
}

void Pos3::operator*=(double n){
    x*=n;
    y*=n;
    z*=n;
    update();
}

void Pos3::show(){
    cout<<"x: "<<x<<"\ty: "<<y<<"\tz: "<<z<<endl;
}

double *Pos3::pos(void){
    return position;
}

void Pos3::SwitchYZ(void){
    double t;
    t=y;
    y=z;
    z=t;
    update();
}

class PCD{
public:
    long length;
    PCD();
    Pos3 center;
    Pos3 centroid;
    void SwitchYZ(void);
    void Translate(double dx,double dy,double dz);
    void update(void);
    void load(string path);
    void save(string path);
    void showall(void);
    double AABB[6];
private:
    double x_max;
    double x_min;
    double y_max;
    double y_min;
    double z_max;
    double z_min;
    double cal_max_min(int axis,int type); //axis:0,1,2; type: -1==min, 1==max;
    vector<Pos3> data;
    ifstream infile;
    ofstream outfile;
};

PCD::PCD(){
    length=0;
    x_max=0;
    x_min=0;
    y_max=0;
    y_min=0;
    z_max=0;
    z_min=0;
}

void PCD::SwitchYZ(void){
    long i;
    for(i=0;i<length;i++){
        data[i].SwitchYZ();
    }
    update();
}

void PCD::Translate(double dx,double dy,double dz){
    long i;
    for(i=0;i<length;i++){
        Pos3 d(dx,dy,dz);
        data[i]+=d;
    }
    update();
}

void PCD::update(void){
    length=data.size();
    x_max=cal_max_min(0,1);
    x_min=cal_max_min(0,-1);
    y_max=cal_max_min(1,1);
    y_min=cal_max_min(1,-1);
    z_max=cal_max_min(2,1);
    z_min=cal_max_min(2,-1);
    AABB[0]=x_max;
    AABB[1]=x_min;
    AABB[2]=y_max;
    AABB[3]=y_min;
    AABB[4]=z_max;
    AABB[5]=z_min;
    center= Pos3((x_max+x_min)/2,(y_max+y_min)/2,(z_max+z_min)/2);
    vector<double> xlist;
    vector<double> ylist;
    vector<double> zlist;
    for(long i=0;i<length;i++){
        xlist.push_back(data[i].x);
        ylist.push_back(data[i].y);
        zlist.push_back(data[i].z);
    }
    double cx=accumulate(xlist.begin(),xlist.end(),0.0)/length;
    double cy=accumulate(ylist.begin(),ylist.end(),0.0)/length;
    double cz=accumulate(zlist.begin(),zlist.end(),0.0)/length;
    centroid=Pos3(cx,cy,cz);
}

void PCD::load(string path){
    double x0,y0,z0;
    infile.open(path,ios::in);
    if(!infile.is_open()){
        cout<< "failed to open file"<<endl;
        return;
    }
    while (!infile.eof()){
        infile>>x0>>y0>>z0;
        data.push_back(Pos3(x0,y0,z0));
    }
    infile.close();
    update();
}

void PCD::save(string path){
    outfile.open(path);
    outfile.precision(15);
    long i;
    for(i=0;i<length;i++){
        outfile<<data[i].x<<" "<<data[i].y<<" "<<data[i].z;
        if(i!=length-1)
            outfile<<endl;
    }
    outfile.close();
}

void PCD::showall(void){
    long i;
    cout<<endl;
    for(i=0;i<5;i++)
        data[i].show();
    cout<<"..."<<endl;
    for(i=length-3;i<length;i++)
        data[i].show();
    cout<<endl;
    cout<<"Length: \t"<<length<<endl;
    cout<<"Center: \t";
    center.show();
    cout<<"Centroid: \t";
    centroid.show();
}

double PCD::cal_max_min(int axis,int type){
    vector<double> d_axis;
    int i;
    for(i=0;i<length;i++){
        d_axis.push_back(data[i].pos()[axis]);
    }
    if(type==-1)
        return *max_element(d_axis.begin(),d_axis.end());
    else if(type==1)
        return *min_element(d_axis.begin(),d_axis.end());
    else{
        cout<<"Failed to find max/min"<<endl;
        return -1;
    }
}



int main()
{
    cout << "Hello world!" << endl;
    PCD tree;
    tree.load("E:/SHU/PCD_Viewer/tree.PCD");
    tree.showall();
    tree.SwitchYZ();
    tree.showall();
    //tree.Translate(2,3,3);
    //tree.showall();
    tree.save("E:/SHU/PCD_Viewer/tree_zy.PCD");
    return 0;
}
