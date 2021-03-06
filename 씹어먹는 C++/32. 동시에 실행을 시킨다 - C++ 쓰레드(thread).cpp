#include <iostream>

/*3 example crawling website
int main(){
	map<string, string> url_and_content;
	for(auto itr=url_and_content.begin(); itr!=url_and_content.end(); ++itr){
		const string& url=itr->first;
		itr->second=download(url);//download and return of site.
	}
}*/

#include <thread>
/*4
using std::thread;
void func1(){
	for(int i=0; i<10; i++)
		std::cout<<"working thread 1!"<<std::endl;
}
void func2(){
	for(int i=0; i<10; i++)
		std::cout<<"working thread 2!"<<std::endl;
}
void func3(){
	for(int i=0; i<10; i++)
		std::cout<<"working thread 3!"<<std::endl;
}

int main(){
	thread t1(func1);//make thread object! 
	thread t2(func2);
	thread t3(func3);
	
	//t1.join();//return when thread done their work.
	//t2.join();
	//t3.join();
	
	t1.detach();
	t2.detach();
	t3.detach();
	
	std::cout<<"exit main \n";
}*/

#include <cstdio>
#include <vector> 
/*5
using std::thread;
using std::vector;
void worker(vector<int>::iterator start, vector<int>::iterator end, int* result){
	int sum=0;
	for(auto itr=start; itr<end; ++itr)
		sum+=*itr;
	*result=sum;
	
	//get id of thread
	thread::id this_id=std::this_thread::get_id();
	printf("calculated result of %d to %d of thread %x : %d \n", *start, *(end-1), sum);//not std::cout!! 
}

int main(){
	vector<int> data(10000);
	for(int i=0; i<10000; i++)
		data[i]=i;
	
	vector<int> partial_sums(4);
	//vector that saves partual sums in each thread
	
	vector<thread> workers;
	for(int i=0; i<4; i++)
		workers.push_back(thread(worker, data.begin()+i*2500, data.begin()+(i+1)*2500, &partial_sums[i]));
		
	for(int i=0; i<4; i++)
		workers[i].join();
		
	int total=0;
	for(int i=0; i<4; i++)
		total+=partial_sums[i];
	
	std::cout<<"total sum : "<<total<<std::endl;
}*/

//6
using std::thread;
using std::vector;

void worker(int& counter){
	for(int i=0; i<10000; i++)
		counter++;
}
int main(){
	int counter=0;
	
	vector<thread> workers;
	for(int i=0; i<4; i++)
		workers.push_back(thread(worker, std::ref(counter)));
	for(int i=0; i<4; i++)
		workers[i].join();
		
	std::cout<<"Final result of Counter : "<<counter<<std::endl;//diffenent value!
}



/*
[1.	???? ?????? ????????]
1.	???????????? ?????? ?????? ?????? ?????????? ?? ?? ????. ?????? ???? 1???? ?????????? ?????? ?? 1???? ?????????? ????????.
	?????? ?????????? CPU ???????? ??????????, CPU?? ?????? ?????? ???????? ???????? ???????? ??????(Context switching)?? ???????? ?????????? ?????? ???? ?????? ?????????? ???????? ????????.
	??, CPU?? ?????????? ?????? ???????? ?????? ??, ?????????? ????????, ????, ???????? ???????? ???????? ???? ?????????? ????????(scheduler)?? ???????? ????.
	 ?? CPU???????? ???????? ???????? ?????? ??????(thread)???? ??????, ?? CPU???????????? ?????? ?? ???? ???????? ?????? ???????? ????.
	?? ???? ?????????? ???? ?? ???? ???????? ????????, ???????? ???????? ?????? ?????????? ???? ??????(multithread) ?????????????? ????.
	???????? ?????????? ???? ?? ???????? ?? ???????? ???? ???????? ???????? ???? ???? ???????? ???????? ????, ???????????? ???? ???????? ???????? ???????? ??????. 

[2.	CPU?? ?????? ?? ???? ??????]
1.	?????? CPU?????????? ???? ?????? ?????????? ?????? ??????, CPU?? ?????? ???? ?????? ?????? ?????? ??????????.
	?????? ???????? CPU???? ?????? ???? ?????? ???????????? ???????? ???? ?????? ?? ???????? ?????? ?? ????????, ???????? CPU?????? ???????? ?????????? ?????????? ?????? ?? ??????????.
	(p.s SMT(Simultaneous Multithreading)?????? ???? ???????? ???????? ?? ?? ????) 

[3.	?????? ?? ???? ?????? ?????]
1.	???? ??????(Parallelizable)???????? ???????? ???????????? ??????????. 
	???? ???????? ?????????? ???? ????, Fn?? ???????? Fn-1 Fn-2?? ???????????? parallelize(??????)?? ?????? ?????? ???? ????.
	???? ???????? ?????? ???????? ?????????? ?????????????? ???? ??????. ?????? ????(A)?? ???????? ???? ???? ????(B)?? ?????? ???????? ????????, ?????? ???? A?? B?? ???????? ???? ????????.
	??, ???????? ???????? ?????? ?????????? ?????????? ?????? ???????? ??????????, ?????????? ?????? ???????? ???? ????????.
2.	?????????? ?? ???????? ?????????? ??????????.
	???????? CPU?????????? ???? ???? ??????. ping?????? ?????? ?????? ???? ???????? ?????? ??????????, ?????? CPU?? 1???? 10^9?? ?????? ?? ?? ??????, CPU?? ???????????? ???????? ???? ????.
	???? ???????? ?????? ???? ?????????? ?????? ???????? ???????? ???? ???????? ???? ???? CPU?? ?????????? ?????? ?? ???? ????.
	
[4.	C++???? ?????? ????????]
1.	C++ 11???? ?????? ???????? ???????? ?????? ?????????? ?????????? ???????????? ???? ?????? ?????? ???? ??????????.
2.	#include <thread>???? thread t1(func2) thread?????? ???????? ??????.  t1?? ?????? ???????? ?????? ?????? ?????????? ???????? ????.(???? ?? ?????? ?????? 3???? ???????? ???????? ?????? ?????? ???? ????. ???? ?????????? ????) 
	join()?? ???????? ?????? ???????? ???????? ????????. 
	 ???? t2?? t1???? ???? ?????????? t1.join()?? ?????? t2.join()???? ???? ????????. 
3.	???? join?? ???? ???? ???? ???????? ?????? ?? ?????????? ???? main?????? ???????? thread ?????? ???????? ????????. ???? ?? ?? ???? ?m 
	C++?????? ?????? join?????? detach???? ???? ?????????? ???????? ???????? ?????? ?????????? ????????.(???? ????)
4.	detach?? ???? ???????? ???? ???? ??, ?????????? ??????. ?? ?????????????? ???????? ??????. 
	?????????? ?????????? ?????? ??, ???? ???????? ???? ???? ?????????? ???? ?????? ???????? ???????? ????????. ??, main function???? exit main?? print?????? work thread ~?? ?????? ?? ???? ????.
	 ???????? detach???? ???? main?????????? ?????????? ?????? ?? ???? ???????? ??????. ???? ???????? ?????????? ???? ?????????? ?????????? ???????? ???????? ?????? ?? ????. 

[5.	???????? ???? ????????]
1.	thread::id this_id=std::this_thread::get_id();?? ???? ?????????? ???????????? ?? ?? ????.
	std::cout<<A<<B?? ???? A?? ???????? ?????? ???? ???????? ?????? ?????? ?? ???? ?????? ??????, A?? ?????? ?????? B?? ???????? ?????? ???? ???????? ?????? ???????? ?? ??????
	printf?? ????????. printf?? ""???? ???? ???????? ?????? ??, ???? ?????????? ?? ?????? ???????? ???????? ?????? ???? ???????? ???? ?????? ????????. 

[6.	???? ?????????? ???????? ???? ???????????]
1.	???????????? ???????? 

*/
