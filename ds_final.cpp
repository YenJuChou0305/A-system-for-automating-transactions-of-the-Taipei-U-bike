#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

enum StatusType {Free, Rented};
enum ClassType {Electric, Lady, Road, Hybrid};
enum StationType {
	Danshui, Hongshulin, Beitou,
	Shilin, Zhongshan, Xinpu,
	Ximen, Liuzhangli, Muzha,
	Guting, Gongguan, Jingmei };

typedef string LicenseType;

struct BikeType{
	LicenseType License;
	StatusType Status;
	int Mileage; /* most recently rented mileage */
	int Cursor; /* cursor to the entry in heap where there is a pointer to this node */
	StationType Station;
	ClassType Class;
};

typedef struct BikeType *BikePtr;

struct HeapType{
	BikePtr Elem[256]; /* use array to implement heap, and each node in the heap is a pointer*/
	int Number;
};
struct OfficeType{
	int NetTotal; /* total income of station */
	int Net[4];
	int NumTrs[4]; /* number of bikes */
	HeapType Heap[8];
};

struct HNodeType {
	LicenseType License ;
	BikePtr Ptr; /* point to the node representing the bike */
	struct HNodeType *next_ptr; /*point to the next node in the chaining list*/
};

struct HTableType {
	HNodeType table[256];
	/*since each entry in the hash table is a pointer, it will be initialized as NULL;*/
	int BikeNum; /* the number of bikes in the hash table */

};

int graph[12][12]={0};

int getHash(string hash){
	int s[5],num;
	for(int i=0;i<5;i++){
		if(hash[i]>='0' && hash[i]<='9')
			s[i]=hash[i]-48;
		else
			s[i]=hash[i]-55;
	}
	num=(((s[0]*31+s[1])*31+s[2])*31+s[3])*31+s[4];
	num/=1024;
	num&=255;
	return num;
}

StationType getStationType(string station){
	if(station=="Danshui")
		return Danshui;
	else if(station=="Hongshulin")
		return Hongshulin;
	else if(station=="Beitou")
		return Beitou;
	else if(station=="Shilin")
		return Shilin;
	else if(station=="Zhongshan")
		return Zhongshan;
	else if(station=="Xinpu")
		return Xinpu;
	else if(station=="Ximen")
		return Ximen;
	else if(station=="Liuzhangli")
		return Liuzhangli;
	else if(station=="Muzha")
		return Muzha;
	else if(station=="Guting")
		return Guting;
	else if(station=="Gongguan")
		return Gongguan;
	else
		return Jingmei;
}

ClassType getClassType(string type){
	if(type=="Electric")
		return Electric;
	else if(type=="Lady")
		return Lady;
	else if(type=="Road")
		return Road;
	else
		return Hybrid;
}

string getStation(StationType stationType){
	switch(stationType){
		case Danshui:
			return "Danshui";
		case Hongshulin:
			return "Hongshulin";
		case Beitou:
			return "Beitou";
		case Shilin:
			return "Shilin";
		case Zhongshan:
			return "Zhongshan";
		case Xinpu:
			return "Xinpu";
		case Ximen:
			return 	"Ximen";
		case Liuzhangli:
			return "Liuzhangli";
		case Muzha:
			return "Muzha";
		case Guting:
			return "Guting";
		case Gongguan:
			return "Gongguan";
		case Jingmei:
			return "Jingmei";
	}
}

string getClass(ClassType classType){
	switch(classType){
		case Electric:
			return "Electric";
		case Lady:
			return "Lady";
		case Road:
			return "Road";
		case Hybrid:
			return "Hybrid";
	}
}

void init(OfficeType *office){
	office->NetTotal=0;
	for(int i=0;i<4;i++){
		office->Net[i]=0;
		office->NumTrs[i]=0;
	}
	for(int i=0;i<8;i++)
		office->Heap[i].Number=0;
}

void heapPush(HeapType* Heap,BikePtr ptr){
	int num=Heap->Number+1;
	ptr->Cursor=num;
	Heap->Elem[num]=ptr;
	Heap->Number++;

	while(num>1 && Heap->Elem[num/2]->Mileage < Heap->Elem[num]->Mileage){
		BikePtr temp;
		temp=Heap->Elem[num/2];
		Heap->Elem[num/2]=Heap->Elem[num];
		Heap->Elem[num]=temp;
		Heap->Elem[num/2]->Cursor=num/2;
		Heap->Elem[num]->Cursor=num;
		num/=2;
	}
}

int Max(int a,int b,int c){
	if(b>a&&b>=c)
		return 1;
	else if(c>a&&c>b)
		return 2;
	else
		return 0;
}

void heapDel(HeapType* Heap, int num){
	if(num==Heap->Number){
		Heap->Number--;
		return;
	}
	else{
		Heap->Elem[num]=Heap->Elem[Heap->Number];
		Heap->Elem[num]->Cursor=num;
		Heap->Number--;
	}
	while(num>1 && Heap->Elem[num/2]->Mileage < Heap->Elem[num]->Mileage){
		BikePtr temp;
		temp=Heap->Elem[num/2];
		Heap->Elem[num/2]=Heap->Elem[num];
		Heap->Elem[num]=temp;
		Heap->Elem[num/2]->Cursor=num/2;
		Heap->Elem[num]->Cursor=num;
		num/=2;
	}

	int maximum;
	if(num*2+1<=Heap->Number)
		maximum=Max(Heap->Elem[num]->Mileage,Heap->Elem[num*2]->Mileage,Heap->Elem[num*2+1]->Mileage);
	else if(num*2==Heap->Number)
		maximum=Max(Heap->Elem[num]->Mileage,Heap->Elem[num*2]->Mileage,-1);
	else
		return;
	while(maximum){
		if(maximum==1){
			BikePtr temp;
			temp=Heap->Elem[num];
			Heap->Elem[num]=Heap->Elem[num*2];
			Heap->Elem[num*2]=temp;
			Heap->Elem[num]->Cursor=num;
			Heap->Elem[num*2]->Cursor=num*2;
			num*=2;
		}
		else if(maximum==2){
			BikePtr temp;
			temp=Heap->Elem[num];
			Heap->Elem[num]=Heap->Elem[num*2+1];
			Heap->Elem[num*2+1]=temp;
			Heap->Elem[num]->Cursor=num;
			Heap->Elem[num*2+1]->Cursor=num*2+1;
			num=num*2+1;
		}
		if(num*2+1<=Heap->Number)
			maximum=Max(Heap->Elem[num]->Mileage,Heap->Elem[num*2]->Mileage,Heap->Elem[num*2+1]->Mileage);
		else if(num*2==Heap->Number)
			maximum=Max(Heap->Elem[num]->Mileage,Heap->Elem[num*2]->Mileage,-1);
		else
			return;

	}
}

int dij(int head,int tail){
    int dis=0;
    int d[12],u[12]={0};
    for(int i=0;i<12;i++)
        d[i]=1000000;

    int a=head;
    u[head]=1;
    d[head]=0;

    while(a!=tail){
        for(int i=0;i<12;i++){
            if(!u[i] && graph[a][i]!=0 && d[a]+graph[a][i]<d[i])
                   d[i]=d[a]+graph[a][i];
        }
        int min=1000000;
        for(int i=0;i<12;i++){
            if(!u[i] && d[i]<min){
                min=d[i];
                a=i;

            }
        }
        u[a]=1;
    }

    return d[a];
}

int calCharge(int dis,int head,int tail,int type){
    int shDis=dij(head,tail);
    if(dis<=shDis){
        switch(type){
            case 0:
                return dis*30;
            case 1:
                return dis*25;
            case 2:
                return dis*15;
            case 3:
                return dis*20;
        }
    }
    else{
        switch(type){
            case 0:
                return dis*40;
            case 1:
                return dis*30;
            case 2:
                return dis*20;
            case 3:
                return dis*25;
        }
    }
    return 0;
}

int main(int argc, char* argv[]){

	if(argc!=3){
		cout << "arguments are incorrect" << endl;
		return 0;

	}

	string a,b;
	int dis;
	ifstream file2(argv[2]);
	while(file2>>a>>b>>dis){
        graph[getStationType(a)][getStationType(b)]=dis;
        graph[getStationType(b)][getStationType(a)]=dis;
	}
	file2.close();

	HTableType HTable;
	HTable.BikeNum=0;
	for(int i=0;i<256;i++)
		HTable.table[i].next_ptr=NULL;
	OfficeType office[12];
	for(int i=0;i<12;i++)
		init(&office[i]);

	ifstream file1(argv[1]);
	ofstream output("output.txt");
	string command;
	while(file1>>command){
		if(command=="NewBike"){
			string type,license,station;
			int mile;
			BikePtr ptr=new BikeType;
			file1>>type>>license>>mile>>station;
			output<<"New bike is received by Station "<<station<<"."<<endl;

			ptr->License=license;
			ptr->Status=Free;
			ptr->Mileage=mile;
			ptr->Station=getStationType(station);
			ptr->Class=getClassType(type);
			heapPush(&(office[ptr->Station].Heap[ptr->Class]),ptr);

			office[ptr->Station].NumTrs[ptr->Class]++;

			HNodeType* nodePtr;
			int hash=getHash(license);
			nodePtr=&(HTable.table[hash]);
			while(nodePtr->next_ptr)
				nodePtr=nodePtr->next_ptr;
			nodePtr->next_ptr=new HNodeType;
			nodePtr->next_ptr->next_ptr=NULL;
			nodePtr->next_ptr->Ptr=ptr;
			nodePtr->next_ptr->License=license;
			HTable.BikeNum++;
		}
		else if(command=="JunkIt"){
			string license;
			file1>>license;

			HNodeType* nodePtr;
			int hash=getHash(license);
			nodePtr=&(HTable.table[hash]);
			while(nodePtr->next_ptr && nodePtr->next_ptr->License!=license)
				nodePtr=nodePtr->next_ptr;

			if(!nodePtr->next_ptr)
				output<<"Bike "<<license<<" does not belong to our company."<<endl;
			else if(nodePtr->next_ptr->Ptr->Status==Rented)
				output<<"Bike "<<license<<" is now being rented."<<endl;
			else{
				output<<"Bike "<<license<<" is deleted from "<<getStation(nodePtr->next_ptr->Ptr->Station)<<"."<<endl;
				BikePtr ptr=nodePtr->next_ptr->Ptr;
				heapDel(&(office[ptr->Station].Heap[ptr->Class]),ptr->Cursor);
				office[ptr->Station].NumTrs[ptr->Class]--;
				nodePtr->next_ptr=nodePtr->next_ptr->next_ptr;
				HTable.BikeNum--;
			}
		}
		else if(command=="Rent"){
			string station,type;
			file1>>station>>type;

			if(office[getStationType(station)].Heap[getClassType(type)].Number==0)
				output<<"No free bike is available."<<endl;
			else{
				output<<"A bike is rented from "<<station<<"."<<endl;
				BikePtr ptr=office[getStationType(station)].Heap[getClassType(type)].Elem[1];
				ptr->Status=Rented;
				heapDel(&(office[ptr->Station].Heap[ptr->Class]),ptr->Cursor);
				heapPush(&(office[ptr->Station].Heap[ptr->Class+4]),ptr);
			}
		}
		else if(command=="Returns"){
			string station,license;
			int mileage,charge=0;
			file1>>station>>license>>mileage;

			HNodeType *nodePtr;
			int hash=getHash(license);
			nodePtr=&(HTable.table[hash]);
			while(nodePtr->next_ptr && nodePtr->next_ptr->License!=license)
				nodePtr=nodePtr->next_ptr;
			BikePtr ptr=nodePtr->next_ptr->Ptr;
			ptr->Status=Free;
			dis=mileage-ptr->Mileage;
			ptr->Mileage=mileage;
			heapDel(&(office[ptr->Station].Heap[ptr->Class+4]),ptr->Cursor);
			heapPush(&(office[ptr->Station].Heap[ptr->Class]),ptr);

			charge=calCharge(dis,ptr->Station,getStationType(station),ptr->Class);
			office[ptr->Station].NetTotal+=charge;
			office[ptr->Station].Net[ptr->Class]+=charge;

			output<<"Rental charge for this bike is "<<charge<<"."<<endl;
		}
		else if(command=="Trans"){
			string station,license;
			file1>>station>>license;

			HNodeType *nodePtr;
			int hash=getHash(license);
			nodePtr=&(HTable.table[hash]);
			while(nodePtr->next_ptr && nodePtr->next_ptr->License!=license)
				nodePtr=nodePtr->next_ptr;

			if(!nodePtr)
				output<<"Bike "<<license<<" does not belong to our company."<<endl;
			else if(nodePtr->next_ptr->Ptr->Status==Rented)
				output<<"Bike "<<license<<" is now being rented."<<endl;
			else{
				output<<"Bike "<<license<<" is transfered to "<<station<<"."<<endl;
				BikePtr ptr=nodePtr->next_ptr->Ptr;
				heapDel(&(office[ptr->Station].Heap[ptr->Class]),ptr->Cursor);
				heapPush(&(office[getStationType(station)].Heap[ptr->Class]),ptr);
				office[ptr->Station].NumTrs[ptr->Class]--;
				office[getStationType(station)].NumTrs[ptr->Class]++;
				ptr->Station=getStationType(station);
			}
		}
		else if(command=="Inquire"){
			string license;
			file1>>license;

			HNodeType *nodePtr;
			int hash=getHash(license);
			nodePtr=&(HTable.table[hash]);
			while(nodePtr->next_ptr && nodePtr->next_ptr->License!=license)
				nodePtr=nodePtr->next_ptr;

			if(!nodePtr->next_ptr)
				output<<"Bike "<<license<<" does not belong to our company."<<endl;
			else{
				output<<setw(15)<<"License"<<setw(15)<<"Mileage"<<setw(15)<<"Class"<<setw(15)<<"Station"<<endl;
				output<<"============================================================"<<endl;
				output<<setw(15)<<nodePtr->next_ptr->Ptr->License<<setw(15)<<nodePtr->next_ptr->Ptr->Mileage<<setw(15)<<getClass(nodePtr->next_ptr->Ptr->Class)<<setw(15)<<getStation(nodePtr->next_ptr->Ptr->Station)<<endl;
                output<<endl;
			}
		}
		else if(command=="StationReport"){
			string station;
			file1>>station;
			output<<setw(30)<<station<<endl;
			output<<setw(30)<<"Free Bikes"<<endl;
			output<<setw(15)<<"License"<<setw(15)<<"Mileage"<<setw(15)<<"Class"<<setw(15)<<"Subtotal"<<endl;
			output<<"============================================================"<<endl;
			int k=0;
			for(int i=0;i<4;i++){
				for(int j=1;j<=office[getStationType(station)].Heap[i].Number;j++){
					BikePtr ptr=office[getStationType(station)].Heap[i].Elem[j];
					output<<setw(15)<<ptr->License<<setw(15)<<ptr->Mileage<<setw(15)<<getClass(ptr->Class)<<endl;
					k++;
				}
			}
			output<<"============================================================"<<endl;
			output<<setw(60)<<k<<endl;
			output<<endl;
			output<<setw(30)<<"Rented Bikes"<<endl;
			output<<setw(15)<<"License"<<setw(15)<<"Mileage"<<setw(15)<<"Class"<<setw(15)<<"Subtotal"<<endl;
			output<<"============================================================"<<endl;
			k=0;
			for(int i=4;i<8;i++){
				for(int j=1;j<=office[getStationType(station)].Heap[i].Number;j++){
					BikePtr ptr=office[getStationType(station)].Heap[i].Elem[j];
					output<<setw(15)<<ptr->License<<setw(15)<<ptr->Mileage<<setw(15)<<getClass(ptr->Class)<<endl;
					k++;
				}
			}
			output<<"============================================================"<<endl;
			output<<setw(60)<<k<<endl;
			output<<endl;
			output<<setw(12)<<"Net"<<setw(12)<<"Electric"<<setw(12)<<"Lady"<<setw(12)<<"Road"<<setw(12)<<"Hybrid"<<endl;
			output<<"============================================================"<<endl;
			output<<setw(12)<<office[getStationType(station)].NetTotal;
			for(int i=0;i<4;i++)
				output<<setw(12)<<office[getStationType(station)].NumTrs[i];
			output<<endl;
			output<<"============================================================"<<endl;
			output<<endl;
		}
		else if(command=="UbikeReport"){
			output<<setw(30)<<"Taipei U-bike"<<endl;
			output<<setw(30)<<"Free Bikes"<<endl;
			output<<setw(12)<<"License"<<setw(12)<<"Mileage"<<setw(12)<<"Class"<<setw(12)<<"Station"<<setw(12)<<"Subtotal"<<endl;
			output<<"============================================================"<<endl;
			int k=0;
			for(int I=0;I<12;I++){
				for(int i=0;i<4;i++){
					for(int j=1;j<=office[I].Heap[i].Number;j++){
						BikePtr ptr=office[I].Heap[i].Elem[j];
						output<<setw(12)<<ptr->License<<setw(12)<<ptr->Mileage<<setw(12)<<getClass(ptr->Class)<<setw(12)<<getStation(ptr->Station)<<endl;
						k++;
					}
				}
			}
			output<<"============================================================"<<endl;
			output<<setw(60)<<k<<endl;
			output<<endl;
			output<<setw(30)<<"Rented Bikes"<<endl;
			output<<setw(12)<<"License"<<setw(12)<<"Mileage"<<setw(12)<<"Class"<<setw(12)<<"Station"<<setw(12)<<"Subtotal"<<endl;
			output<<"============================================================"<<endl;
			k=0;
			for(int I=0;I<12;I++){
				for(int i=4;i<8;i++){
					for(int j=1;j<=office[I].Heap[i].Number;j++){
						BikePtr ptr=office[I].Heap[i].Elem[j];
						output<<setw(12)<<ptr->License<<setw(12)<<ptr->Mileage<<setw(12)<<getClass(ptr->Class)<<setw(12)<<getStation(ptr->Station)<<endl;
						k++;
					}
				}
			}
			output<<"============================================================"<<endl;
			output<<setw(60)<<k<<endl;
			output<<endl;
			output<<setw(12)<<"Net"<<setw(12)<<"Electric"<<setw(12)<<"Lady"<<setw(12)<<"Road"<<setw(12)<<"Hybrid"<<endl;
			output<<"============================================================"<<endl;
			int net=0;
			for(int i=0;i<12;i++)
				net+=office[i].NetTotal;
			output<<setw(12)<<net;
			for(int i=0;i<4;i++){
				net=0;
				for(int j=0;j<12;j++)
					net+=office[j].NumTrs[i];
				output<<setw(12)<<net;
			}
			output<<endl;
			output<<"============================================================"<<endl;
			output<<endl;
		}
		else if(command=="NetSearch"){
			string station;
			file1>>station;

			output<<station<<endl;
			output<<"==============="<<endl;
			int k=0;
			for(int i=0;i<4;i++){
				output<<getClass(ClassType(i))<<" "<<office[getStationType(station)].Net[i]<<endl;
				k+=office[getStationType(station)].Net[i];
			}
			output<<"==============="<<endl;
			output<<"Total "<<k<<endl;
			output<<endl;

		}
		else if(command=="HashReport"){
			output<<"Hash Table"<<endl;
			for(int i=0;i<256;i++){
				if(HTable.table[i].next_ptr){
					HNodeType* nodePtr;
					nodePtr=&HTable.table[i];
					output<<i<<" "<<nodePtr->next_ptr->License;
					nodePtr=nodePtr->next_ptr;
					while(nodePtr->next_ptr){
						output<<"->"<<nodePtr->next_ptr->License;
						nodePtr=nodePtr->next_ptr;
					}
					output<<endl;
				}
			}
		}
	}
	file1.close();
	output.close();
	return 0;
}
