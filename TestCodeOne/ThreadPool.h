#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>


//线程池配置类
class ThreadPoolConfig
{
public:
	ThreadPoolConfig(int core, int max, int max_task, std::chrono::seconds time) :
		core_threads(core), max_threads(max), max_task_size(max_task), time_out(time) {}

	ThreadPoolConfig()
	{
		core_threads = 0;
		max_threads = 0;
		max_task_size = 0;
	}

	/*
	@brief   : 检测配置
	@author  : jhuang.jaygee
	@input   ：none
	@output  ：none
	*/
	inline bool IsVaildConfig()
	{
		if (core_threads < 1 || max_task_size < core_threads || time_out.count() < 1)
		{
			return false;
		}
		return true;
	}
public:
	int core_threads;	//线程池线程数
	int max_threads;	//最大线程数
	int max_task_size;	//最大的任务数;
	std::chrono::seconds time_out; //超时 S
};

enum class ThreadState { kInit = 0, kWaiting = 1, kRunning = 2, kStop = 3 };

enum class ThreadFlg { kInit = 0, kCore = 1, kCache = 2 };


class ThreadPool
{
public:
	using PoolSeconds = std::chrono::seconds;	//计时
	using ThreadPtr = std::shared_ptr<std::thread>;	//线程指针
	using ThreadId = std::atomic<int>;		//线程号
	using ThreadStateAtomic = std::atomic<ThreadState>;
	using ThreadFlgAtomic = std::atomic<ThreadFlg>;
	using Ptr = std::shared_ptr<ThreadPool>;
	using ThreadPoolLock = std::unique_lock<std::mutex>;

	template<typename _Tp>
	using result_of_t = typename std::result_of<_Tp>::type;
public:
	struct ThreadWrapper
	{
		ThreadPtr ptr;
		ThreadId id;
		ThreadFlgAtomic flag;
		ThreadStateAtomic state;

		ThreadWrapper()
		{
			ptr = nullptr;
			id = 0;
			state.store(ThreadState::kInit);
		}
	};
	using ThreadWrapperPtr = std::shared_ptr<ThreadWrapper>;

	ThreadPool(const ThreadPool &) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	~ThreadPool();

	/*
	@brief   : 单例构造线程池对象
	@author  : jhuang.jaygee
	@input   ：none
	@output  ：none
	*/
	static Ptr Instance();


	/*
	@brief   : 初始化线程
	@author  : jhuang.jaygee
	@input   ：线程池配置
	@output  ：none
	*/
	void Init(ThreadPoolConfig config);


	/*
	@brief   : 重新设置线程池配置
	@author  : jhuang.jaygee
	@input   ：none
	@output  ：none
	*/
	bool Reset(ThreadPoolConfig config);


	/*
	@brief   : 启动线程池
	@author  : jhuang.jaygee
	@input   ：none
	@output  ：none
	*/
	bool Start();


	/*
	@brief   : 关闭线程池
	@author  : jhuang.jaygee
	@input   ：none
	@output  ：none
	*/
	void ShutDownNow();

	inline int GetWaitingThreadSize() { return this->waiting_thread_num.load(); }
	inline int GetTotalThreadSize() { return this->work_threads.size(); }
	inline int GetRunnedFuncNum() { return this->total_function_num.load(); }

	inline bool IsAvaliable() { return this->is_available.load(); }



	/*
	@brief   : 加入函数到线程池工作
	@author  : jhuang.jaygee
	@input   ：none
	@output  ：none
	*/

	template<typename F, typename... Args>
	auto Run(F &&f, Args &&... args) -> std::shared_ptr<std::future<result_of_t<F(Args...)>>>
	{
		if (this->is_shutdown.load() || this->is_shutdown_now.load() || !IsAvaliable())
		{
			return nullptr;
		}

		//不够自动扩充
		if (GetWaitingThreadSize() == 0 && GetTotalThreadSize() < _config.max_threads)
		{
			AddThread(GetNextThreadId());
		}

		//获取加入线程的函数返回值
		using return_type = result_of_t<F(Args...)>;

		//绑定一个任务，将传入的函数
		auto task = std::make_shared<std::packaged_task<return_type()>>
			(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...)
				);
		total_function_num++;

		std::future<return_type> res = task->get_future();

		{
			ThreadPoolLock lock(this->task_mutex);
			this->tasks.emplace([task]() {(*task)(); });
		}

		this->task_cv.notify_one();
		return std::make_shared<std::future<result_of_t<F(Args...)>>>(std::move(res));
	}


private:
	ThreadPool();

	void AddThread(int id, ThreadFlg thread_flg);
	void ShutDown(bool now);

	void Resize(int thread_num);

	inline void AddThread(int id) { AddThread(id, ThreadFlg::kCore); }

	inline int GetNextThreadId() { return this->thread_id++; }

private:
	ThreadPoolConfig _config;
	std::list<ThreadWrapperPtr> work_threads;	//工作的线程
	std::queue<std::function<void()>> tasks;	//所有的任务
	std::mutex task_mutex;
	std::condition_variable task_cv;

	std::atomic<int> total_function_num;
	std::atomic<int> waiting_thread_num;
	std::atomic<int> thread_id;

	std::atomic<bool> is_shutdown_now;
	std::atomic<bool> is_shutdown;
	std::atomic<bool> is_available;

	static Ptr m_instance_ptr;
	static std::mutex m_mutex;


};

