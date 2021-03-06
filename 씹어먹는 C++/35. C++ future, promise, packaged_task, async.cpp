#include <iostream>
#include <future>
#include <string>
#include <thread>

#include <condition_variable>
#include <mutex>
#include <exception>

#include <chrono>

#include <vector> 

/*1 for showing unefficiency of this code.
string txt=read("a.txt");//5ms
string resault=do_something_with_txt(txt);//5ms
do_other_computation();//5ms
//read function waits while files are read in hard disk.

//1 solution for eficiency by using thread we have learned.
void file_read(string* result){
	string txt=read("a.txt");//0ms thanks to thread
	//Now, CPU can move to main thread and execute do_other_computation
	*result=do_something_with_txt(txt);//5ms
}
int main(){
	string result;
	thread t(file_read, &result);
	do_other_computation();//5ms
	t.join();//do work by thread. CPU don't rest once.
}*/

/*2 common concept of std::promise & std::future
using std::string;

void worker(std::promise<string>* p){
	p->set_value("some data");//do work promise. it enter to future
}
int main(){
	std::promise<string> p;//get type that will be returned as template argument.
	std::future<string> data=p.get_future();//promise that will be get back string data
	std::thread t(worker, &p);
	data.wait();//wait before getting data.
	
	//return of wait() means data is ready to future.
	//if we do get() without wait, it's same to wait()
	std::cout<<"Received data : "<<data.get()<<std::endl;
	
	t.join();
}*/

/*2 implementing of promise-future pattern by condition variable & limit of this way(why we use future-promise).
std::condition_variable cv;
std::mutex m;
bool done=false;
std::string info;

void worker(){
	{//for call destructor of std::lock_guard()
		std::lock_guard<std::mutex> lk(m);
		info="some data";
		done=true;
	}
	cv.notify_all()//after work, notify to cv that is condition_variable
}

int main(){
	std::thread t(worker);
	std::unique_lock<std::mutex> lk(m);//Lock! (unique_lock can relock after unlock)
	cv.wait(lk, []{return done;});//wait until "done is true". wait untill j.join() handle worker function in critical section.
	lk.unlock();//Unlock! after returning od lk that is unique_lock
	
	std::cout<<"Received data : "<<info<<std::endl;//print. In this time, info is already returned because condition variable waited end of thread work in critical section.
	t.join();//thread!
	//but promise & future is more simple then this.
	//In addition, we can pass exception to future!
}*/

/*2 we can throw exception to future!
using std::string;
void worker(std::promise<string>* p){
	try{
		throw std::runtime_error("Some Error!");//3. throw exception!
	} catch(...){//4. and catch with putting exception value to p
		p->set_exception(std::current_exception());//pass current error by std::current_exception() to p by set_exception()
		//we have to pass ***exception_ptr*** to set_exception
		//not exception object! just only get exception_ptr. 
		//now, we get exception object's pointer at current_exception function that return information of catched exception, not exception object's pointer getting by catch.
		//we can also get exception by object passed by catch, but calling current_exception() is mush simple to use.
	}
}
int main(){
	std::promise<string> p;//set promise object
	std::future<string> data=p.get_future();//set future object getting p's content.
	std::thread t(worker, &p);//make thread to worker with p(promise object)
	
	data.wait();//1. wait untill future object get returned value.
	//4. p get value & data.wait() is returned!
	try{//we can handle exception like using try-catch after throwing exception object to future.
		data.get();//5. get data(promise object)
	} catch(const std::exception& e){//6. catch exception in data(promise object)
		std::cout<<"Exception : "<<e.what()<<std::endl;//6. print Some Error! 
	}
	t.join();//2. thread execute! 
}*/

/*2. wait_for that let future wait for a defined seconds.
void worker(std::promise<void>* p){
	std::this_thread::sleep_for(std::chrono::seconds(10));
	p->set_value();
}
int main(){
	std::promise<void> p;//In case void, it doesn't pass any object, but it can be used flag by set or not.
	std::future<void> data=p.get_future();
	std::thread t(worker, &p);
	
	while(true){//infinite loof until break
		std::future_status status=data.wait_for(std::chrono::seconds(1));//wait 1second.
		//std::future_status can get status of future object.
		
		//Let's check status of std::future_status that gets status of data(future<void> objext)
		if(status==std::future_status::timeout)//not ready
			std::cerr<<">";//erase buffer after print unlike std::cout(with std::endl)
		else if(status==std::future_status::ready)//promise gets future
			break;
	}
	t.join();
}*/

/*3 shared_future (In situation many thread need to get future object)
using std::thread;
void runner(std::shared_future<void>* start){
	start->get();//all threads wait before return. 
	std::cout<<"Start!"<<std::endl;//if p get value? print start! Let's go!
}
int main(){
	std::promise<void> p;
	std::shared_future<void> start=p.get_future();
	
	thread t1(runner, &start);
	thread t2(runner, &start);
	thread t3(runner, &start);
	thread t4(runner, &start);
	
	std::cerr<<"Ready...";
	std::this_thread::sleep_for(std::chrono::seconds(1));//it means ready
	std::cerr<<"Go!"<<std::endl;
	p.set_value();//p get value!
	
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	//make all thread and join but, not execute because of start->get().
	//By getting value to promise, we make compiler do work thread!
}*/

/*4 packaged_task
int some_task(int x){ return 10+x; }

int main(){
	std::packaged_task<int(int)> task(some_task);//make packaged_task object by some_task function. it's like promise object 
	std::future<int> start=task.get_future();//get task by int type
	std::thread t(std::move(task), 5);//make thread with perfect move by std::move function with 5 as argument. 
	//std::packaged_task can't be made by copy so we use std::move for perfect move 
	
	std::cout<<"Resault value : "<<start.get()<<std::endl;//wait or getting value
	t.join();//thread work!
}*/

/*5 std::async
int sum(const std::vector<int>& v, int start, int end){
	int total=0;
	for(int i=start; i<end; ++i)
		total+=v[i];
	return total;
}
int parallel_sum(const std::vector<int>& v){//for calculation by multi threads 
	std::future<int> lower_half_future=std::async(std::launch::async, sum, cref(v), 0, v.size()/2);//make promise object & return like promise.get() in other thread
	//std::async([&v]() { return sum(v, 0, v.size()/2);});
	
	int upper_half=sum(v, v.size()/2, v.size());//normal call of sum function in original thread
	return lower_half_future.get()+upper_half;
}
int main(){
	std::vector<int> v;
	
	v.reserve(1000);
	for(int i=0; i<1000; ++i)
		v.push_back(i+1);
		
	std::cout<<"sum of 1 to 1000 : "<<parallel_sum(v)<<std::endl;
}*/

//5 for showing necessity of std::async(multi threading)
int do_work(int x){
	std::this_thread::sleep_for(std::chrono::seconds(3));
	return x;
}
void do_work_parallel(){
	auto f1=std::async([](){do_work(3);});
	auto f2=std::async([](){do_work(3);});
	do_work(3);
	f1.get();
	f2.get();
}
void do_work_sequential(){
	do_work(3);
	do_work(3);
	do_work(3);
}
int main(){ do_work_parallel();}//why spend 9 seconds...?
//in commonly, parallel spend 3 seconds, and sequential spend 9 seconds.



//LIKE WAT3R ???? ?????? ???? ??????????? June 15?? ?????? ?????? ??????. honestly ???? . Monday Soup. Bornfire with Friends

/*
[0.	?????????? ????]
-	???? ?????????? ???? ???????? ???????? ?? ???? ?????? ?? ???? ?????? ?? ???? ???????? ????????. 

[1.	????(sunchronous)?? ??????(asynchronous) ????]
1.	?? ???? ?????? ?????????? ???????? ?????? ??????(synchronous)???? ?????????? ????. ?? ?????? ???? ?? ???? ???? ???????? ???????? ???? ????????.
	??????, read?? ???? ?????????? ???????? ???????? ???? CPU?? ?????? ?????? ???? ?????? ?? ?????? CPU???? do_other_computation?? ???? ?????? ?????????? ???? ?? ?????????? ??????????.
	 ???????? C++?? thread?? ???? do_other_computaion?? ???? ???? read?? ?????? ???? ???????? ?????? ??????????.
	?????? ?????????? ??????, ?? ?????? ?????? ???? ?????? ???????? ?????? ???????? ???? ????????(asynchronous) ?????????? ??????. 
2.	?????????????? ???? ???????? ???? ???????? ???????? ?????? ??????????, C++?? ???? ?????????? ???????? ???????? ?????? ???????? ????.
	???? ???? ???? C++11 ???? ?????????????? ???? ?????? ???????? ?????? ?? ?? ???? ?????? ???????? ???????? ????.
	
[2.	std::promise?? std::future]
1.	???????????? ????????==?????? ???????? ???? ???????? ???? ???????? ????????. ??, ??????(future) ??????T?? ?????? ???????? ???? ?????????? ????(promise)
2.	std::future<string> data=p.get_future();?? p->set_value("some data");?? ???? future?????? ???????? ?????? ???? ???????? get?????? ???????? ?????? ?????????? ???????? ???? ????.
	get?? ???????? ???????? wait?? ?? ?????? ???? ???? ?????????? get?? ???? ?????????? ??????. get?? ???? ???????? ?????? ?????? ?????? ????????. 
	 promise?? ??????-?????? ???????? ???? ??????(producer)?? ?????? ????????, future?? ??????(consumer)?? ?????? ???????? ???? ????????. 
	promise-future?????? condition_variable?? ?????? promise-future???????? ???????? ?????????? ????????.
	????, ?????? ?????? promise?????? set_exception???? ?????? ???????? future???????? .get()???? ???????? ???? ?? ????.
	catch?? ?????? ???? ?????? ?????? ??????, current_exception()?? ???????? ?????? ???????????? ?? ????????.
	???????? ???? get?? ?????????? ?????? future?? ?????????? ????????, try_catch???? ?????? ?? ???? ?????? ????????.
	 ??, ?????????? ?????? ???? ???? ???? ???????? handling?? ?? ????.
3.	 std::future_status?????? future?????? ?????? ???? ?? ????. ?????? ?????? std::future_status::timeout?? std::future_status::ready???? ?????????? ?????? ?? ????.
	wait_for?????? primise?? ?????? ?? ???? ???????? ?????? ?????? ???????? ?????????? ???? ????????????. ???????? ???? future?? ?????? ???????? std::future_status ????????.
	std::future_status?? ready, timeout, deferred?? ??????, deferred?? ?????? ???????? ????. ?????? ???????? ???????? ?????? ???? ???????? ???????? ??????.
	(std::cerr?? ?????? ?????? ?????? ??????. ?? cout?? ???? ?????? ?????????? ?????? ???????? ?????????? ???????? ???? ???????? ??????, cerr?? ???? ?????? ?????? ???????? ???????? ???????? ????????.) 
	(???????? cerr?? ???? ???????? ???? ??????_????????????,,,) 

[3.	shared_future]
1.	???? future?? ???? ?????? get?? ?? ?????? ??????, get?? ?????? ???????? future ?????? ?????? ???????? ????????. ?????? ???? ???? ???? thread???? future?? get???????? ?????? ????.
 	???? ???? shared_future?? ????????. 
 	??, ?????? ???? ?????????? ?????? ?????? ???? ???? ????.

[4.	packaged_task]
1.	C++?????? ?? promise-future?????? ???????? ????(???????? Callable-????????, Funtor ????)?? ???????? ?????? ?????? ?? ???? packaged_task?? ????????.
	packaged_task?? ?????? ?????? ?????? ??, ?? ???? promise?? set_value????, ?????? ???????? ?????? ???????? set_exception?? ????. 
2.	std::packaged_task<????????(????????)>?? ???? ?????? ?????? ?????????? ???? ?????? ??????.  
	???? task(some_task);???? ?????? ???? ?????? ???????? ?????? ??????. ???? ?????????? ???? ?????? thread?? ???? ?? std::move?? ?????? ?????? ?????? ????.
	 ???????? packaged_task?? ?????????? ???????? ???? promise?? ???????? ?????? ?? ?????? promise???? ?????????? ?????? ?????? ???????? ?????????? ???? ????????.
	??, ?????? ???????? promise?? ?????? ?? ????. ???? promise object?? ?????????? ?????? ?????????? ????????.
	
[5.	std::async]
1.	???? promise?? packaged_task?? ???????????? ???????? ???? ???????? ?????????? ???????? ???????? ????.  
	?????? std::async?? ???? ?????? ????????, ???? ???????? ???????? ???? ?????? ???????????? ????????, ?? ???????? future?? ????????. ???? ???? 
2.	std::async?????? ?????? ???? ?????? ???????????? ?????? ????, ???? ???????? ?????? future?? ??????????, ?????? ???????? ?????????? ????????.	
	std::launch::async?? ???????????? ???????? ???????? ????,
	std::launch::deferred?? future?? get?????? ?????????? ?? ????????.(?????? ???????? ???????? ????)_??, ???? ???????????? ?????? ?????? ???? ??. 
	 ???? ???????? ???? ???? ?????????? ???????????? ????. promise?? future?? ???????????? ?????? ???????? future?????? ??????. 

[6.	????]
-	???? ???????? ???????? ?????? promise-future???????? ???????? ???? ????. 
-	shared_future?? ???? ???? ???????? ?????? ???? ????
-	packaged_task?? ?????? ?????? promise?? future???? ???? ????
-	async?? ?????? ?????? ???????????? ???? ????. 
*/ 
