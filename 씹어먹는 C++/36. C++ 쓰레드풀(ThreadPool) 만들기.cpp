#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <condition_variable>
#include <exception>

#include <chrono>
#include <cstdio>

#include <future>
#include <mutex> 

//1
namespace ThreadPool{
class ThreadPool{
	//Num of Worker thread
	size_t num_threads_;
	//Save Worker thread by vector
	std::vector<std::thread> worker_threads_;
	//Save jobs that we have to handle
	std::queue<std::function<void()>> jobs_;
	//But queue is not safe in multi thread environment, so add mutex that will protect queue
	std::mutex m_job_q_;
	//For wait in producer-consumer model
	std::condition_variable cv_job_q_;
	//all Worker thread stop & for escape infinite loof
	bool stop_all;
	
	//Wait for add of work, do handle work.
	void WorkerThread();
	
	public:
		//Constructor
		ThreadPool(size_t num_threads);
		//Destructor
		~ThreadPool();
		//Add job to queue. privious version
		//void EnqueueJob(std::function<void()> job);
		//Developed EnqueueJob version
		template <class F, class... Args>
		std::future<typename std::result_of<F(Args...)>::type> EnqueueJob(F&& f, Args&&... args);
}; 
//Constructor
ThreadPool::ThreadPool(size_t num_threads):num_threads_(num_threads), stop_all(false){//with initializer list.
	worker_threads_.reserve(num_threads_);//reserve space for efficiency.
	for(size_t i=0; i<num_threads; ++i)
		worker_threads_.emplace_back([this](){ this->WorkerThread(); });//make threads to vector. In lambda function, we have to pass this for access to member function.
}
//wait for work, do work
void ThreadPool::WorkerThread(){
	while(true){
		std::unique_lock<std::mutex> lock(m_job_q_);//mutex lock
		cv_job_q_.wait(lock, [this](){ return !this->jobs_.empty() || stop_all; });//sleep until work add or stop_all activate
		if(stop_all&&this->jobs_.empty())//because of stop_all
			return;

	//because of work add not stop_all
	std::function<void()> job=std::move(jobs_.front());//get function
	jobs_.pop();//remove in queue
	lock.unlock();//mutex unlock
	
	job();//do job
	}//whie end
}
//add job to queue. privious version
//void ThreadPool::EnqueueJob(std::function<void()> job){
	//if(stop_all)//if stop all?
	//	throw std::runtime_error("ThreadPool is stopped");// then throw error
	//{//for calling of lock_guard's destructor
		//std::lock_guard<std::mutex> lock(m_job_q_);//lock!
		//jobs_.push(std::move(job));//add job to queue!
	//}
	//cv_job_q_.notify_one();//job is ready!
//}
//developed version. job add to queue that can handle return value.
template <class F, class... Args>
std::future<typename std::result_of<F(Args...)>::type> ThreadPool::EnqueueJob(F&& f, Args&&... args){
	if (stop_all)
		throw std::runtime_error("ThreadPool is stopped");
	
	using return_type=typename std::result_of<F(Args...)>::type;
	//previous version of job
	//std::packaged_task<return_type()> job(std::bind(f, args...));//2. job object is local variable!
	//new version of job for prevention runtime error because of local variable.
	auto job=std::make_shared<std::packaged_task<return_type()>> (std::bind(f, std::forward<Args>(args)...));//f is pointer of function. so it doesn't have nessecity of std::forward()
	
	std::future<return_type> job_result_future=job->get_future();
	{
		std::lock_guard<std::mutex> lock(m_job_q_);
		jobs_.push([job]() { (*job)(); });//3. job is local variable in lambda so it is broken in this time. how can we solve this problem?
	}//pass copy of shared_ptr to lambda function in returning
	cv_job_q_.notify_one();
	
	return job_result_future;
}
//destructor
ThreadPool::~ThreadPool(){
	stop_all=true;//set stop_all to true
	cv_job_q_.notify_all();//cv response please!
	
	for(auto& t:worker_threads_)
		t.join();//make all threads work for allowing "stop_all is true"
}
}//namespace ThreadPool

int work(int t, int id){
	printf("%d start \n", id);
	std::this_thread::sleep_for(std::chrono::seconds(t));
	printf("%d end after %ds\n",id, t);
	return t+id;//return!
}
int main(){
	ThreadPool::ThreadPool pool(3);
	
	std::vector<std::future<int>> futures;//vector for saving std::future
	for(int i=0; i<10; i++)
		futures.emplace_back(pool.EnqueueJob(work, i%3+1,i));//pool's EnqueueJob returns std::future type's object. pass return object to futures(vector)
	
	for(auto& f : futures)//range based for loop in chap.23
		printf("result : %d \n", f.get());//1. runtime error occur! Broken promise exception means future object is destoied before sel_value to promise.
	//we can solve this problem by 2 ways
	//Storing pasckaged_task to container or Storing packaged_task to shared_ptr
	//first way has problem packaged_task remain in containter when we don't use packaged_task.
	//second way is convenient because shared_ptr's destuctor remove object.
}



/*
[0. ?????????? ????]
1.	?????? ???? ?? ???? ????????, ???????? ???? ???????? ?? ?????? ???? ????????.
	?????? queue?? ???????? ??????, C++?? pop?? ???? ?????? ?????????? ???????? ?????? front?? ????????.
	???? ???????? ???? ???? ?????? ?????? ???? ?????? ??????, ???? ?????? ?????? ?????????? ?????????? ???? ???? ?????????? ???????? ????.

[1.	?????? ???? ????]
1.	?????????? ???????? ?????? ??????, ?????? ???????????? ???????? ?????????? Worker?????????? ????.
	?????? ???? ???????? ?????????? ?????? ??????, C++?????? ???????? ?????? ???? ???????? ?????? ?? ???? ?????????? ?????? ???? void?? ??????. 
2.	?????? ???? ???? ?????? ?????????? void?? ?????? ???? ?????? ?? ?????? ?? ?????? ?????? ?????? ?????? ?????? ?? ???? ???? ?????????? 
3.	????????: ?????????? ???????? ?? ?? ???? ???????? ???????? ?????? ??????(vector)?? ????. ?????? EnqueueJob???? ???? ??????(queue)?? ???????? ???????? ????. 

[2.	?????? ???? ????]
1.	?????? ThreadPool?? ?????? ?????? ?????? ???? ???? ?????? ???? ?? ???????? ??????. ?? ?????? ???????? ???????? ?????? ?????? ???????? ??????,
	?????? future???? ?? ???? ?????? ?? ???? ?????? ?? ????.
	 ???? ???? EnqueueJob?????? ?????? ?????? ?????? ????, ?? ?????? ???????? ???????? future?? ???????? ???? ??????????. 
2.	std::result_of<F(Args...)> ???? ???? F?? ???????? ?? ??????. ???? 
	std::future<f?? ????????> EnqueueJob(F f, Args... args);?? ??????.
3.	void()???? ?????? ?????? ?? ???? ?????????? ?????? ???? ???? ???? lmabda????????
	jobs_.push([f, args...]() {f(args...);});?? ???????????? Worker?????? ?????? ???? f(args...)?? ?????? ?? ????.
	 ???? ?????????? ???????? ???? ?????? ???? ???????????? ???????? ?????? ???????? ???? ?????? ?? ???? packaged_task?? ??????????.
	using return_type=typename std::result_of<F(Args...)>::type; ?????? ???? 
	std::packaged_task<return_type()> job(std::bind(f, args...)); 
	 ???? ?????? ???????????? ???? packaged_task?? ???????? ???????? ?????? ?????? ???????? ???????? ???? job(args...)?? ?????? std::bind(f, args...)?? ?????????? ????.

[3.	?????? ????]
1.	EnqueueJob?????? ?????? ????
	ThreadPool::EnqueueJob(F f, Args... args);???? ???????? ???????? ???????? ??????. ???????? ?????? ???? ???? ?????? ???? ?????? ????????.
2.	???? EnqueueJob ???????? ???????? ?????? ?????????? ?????? bind?????? forward?? ?????? ??????????. 
*/
