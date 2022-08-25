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


//�̳߳�������
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
	@brief   : �������
	@author  : jhuang.jaygee
	@input   ��none
	@output  ��none
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
	int core_threads;	//�̳߳��߳���
	int max_threads;	//����߳���
	int max_task_size;	//����������;
	std::chrono::seconds time_out; //��ʱ S
};

enum class ThreadState { kInit = 0, kWaiting = 1, kRunning = 2, kStop = 3 };

enum class ThreadFlg { kInit = 0, kCore = 1, kCache = 2 };


class ThreadPool
{
public:
	using PoolSeconds = std::chrono::seconds;	//��ʱ
	using ThreadPtr = std::shared_ptr<std::thread>;	//�߳�ָ��
	using ThreadId = std::atomic<int>;		//�̺߳�
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
	@brief   : ���������̳߳ض���
	@author  : jhuang.jaygee
	@input   ��none
	@output  ��none
	*/
	static Ptr Instance();


	/*
	@brief   : ��ʼ���߳�
	@author  : jhuang.jaygee
	@input   ���̳߳�����
	@output  ��none
	*/
	void Init(ThreadPoolConfig config);


	/*
	@brief   : ���������̳߳�����
	@author  : jhuang.jaygee
	@input   ��none
	@output  ��none
	*/
	bool Reset(ThreadPoolConfig config);


	/*
	@brief   : �����̳߳�
	@author  : jhuang.jaygee
	@input   ��none
	@output  ��none
	*/
	bool Start();


	/*
	@brief   : �ر��̳߳�
	@author  : jhuang.jaygee
	@input   ��none
	@output  ��none
	*/
	void ShutDownNow();

	inline int GetWaitingThreadSize() { return this->waiting_thread_num.load(); }
	inline int GetTotalThreadSize() { return this->work_threads.size(); }
	inline int GetRunnedFuncNum() { return this->total_function_num.load(); }

	inline bool IsAvaliable() { return this->is_available.load(); }



	/*
	@brief   : ���뺯�����̳߳ع���
	@author  : jhuang.jaygee
	@input   ��none
	@output  ��none
	*/

	template<typename F, typename... Args>
	auto Run(F &&f, Args &&... args) -> std::shared_ptr<std::future<result_of_t<F(Args...)>>>
	{
		if (this->is_shutdown.load() || this->is_shutdown_now.load() || !IsAvaliable())
		{
			return nullptr;
		}

		//�����Զ�����
		if (GetWaitingThreadSize() == 0 && GetTotalThreadSize() < _config.max_threads)
		{
			AddThread(GetNextThreadId());
		}

		//��ȡ�����̵߳ĺ�������ֵ
		using return_type = result_of_t<F(Args...)>;

		//��һ�����񣬽�����ĺ���
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
	std::list<ThreadWrapperPtr> work_threads;	//�������߳�
	std::queue<std::function<void()>> tasks;	//���е�����
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

