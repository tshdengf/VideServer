#include "ThreadPool.h"


typedef std::shared_ptr<ThreadPool> Ptr;

Ptr ThreadPool::m_instance_ptr = nullptr;
std::mutex ThreadPool::m_mutex;

ThreadPool::ThreadPool() {}

void ThreadPool::AddThread(int id, ThreadFlg thread_flg)
{
	//初始化线程单元
	ThreadWrapperPtr thread_ptr = std::make_shared<ThreadWrapper>();
	thread_ptr->id.store(id);
	thread_ptr->flag.store(thread_flg);

	//线程函数
	auto func = [this, thread_ptr]()
	{
		for (;;)
		{
			std::function<void()> task;
			{
				ThreadPoolLock lock(this->task_mutex);
				if (thread_ptr->state.load() == ThreadState::kStop) break;

				thread_ptr->state.store(ThreadState::kWaiting);
				++this->waiting_thread_num;
				bool is_timeout = false;
				if (thread_ptr->flag.load() == ThreadFlg::kCore)
				{
					this->task_cv.wait(lock, [this, thread_ptr]
					{
						//阻塞线程
						return(this->is_shutdown || this->is_shutdown_now || !this->tasks.empty() ||
							thread_ptr->state.load() == ThreadState::kStop);
					});
				}
				else
				{
					this->task_cv.wait_for(lock, this->_config.time_out, [this, thread_ptr]
					{
						return(this->is_shutdown || this->is_shutdown_now || !this->tasks.empty() ||
							thread_ptr->state.load() == ThreadState::kStop);
					});
					is_timeout = !((this->is_shutdown || this->is_shutdown_now || !this->tasks.empty() ||
						thread_ptr->state.load() == ThreadState::kStop));
				}
				--this->waiting_thread_num;

				if (is_timeout)thread_ptr->state.store(ThreadState::kStop);
				if (thread_ptr->state.load() == ThreadState::kStop) break;

				if (this->is_shutdown && this->tasks.empty())break;

				if (this->is_shutdown_now) break;

				thread_ptr->state.store(ThreadState::kRunning);
				task = std::move(this->tasks.front());
				this->tasks.pop();
			}
			task();
		}
	};

	thread_ptr->ptr = std::make_shared<std::thread>(std::move(func));
	if (thread_ptr->ptr->joinable())
	{
		thread_ptr->ptr->detach();
	}
	this->work_threads.emplace_back(std::move(thread_ptr));

}

void ThreadPool::ShutDown(bool now)
{
	if (this->IsAvaliable())
	{
		if (now)
		{
			this->is_shutdown_now.store(true);
		}
		else
		{
			this->is_shutdown_now.store(false);
		}

		this->task_cv.notify_all();
		this->is_available.store(false);
	}
}

void ThreadPool::Resize(int thread_num)
{
	if (thread_num < _config.core_threads) return;

	int old_t_num = work_threads.size();

	if (thread_num > old_t_num)
	{
		while (thread_num-- > old_t_num)
		{
			AddThread(GetNextThreadId());
		}
	}
	else
	{
		int diff = old_t_num - thread_num;
		auto iter = work_threads.begin();
		while (work_threads.end() != iter)
		{
			if (diff == 0) break;

			auto thread_ptr = *iter;
			if (thread_ptr->flag.load() == ThreadFlg::kCore && thread_ptr->state.load() == ThreadState::kWaiting)
			{
				thread_ptr->state.store(ThreadState::kStop);
				--diff;
				iter = work_threads.erase(iter);
			}
			else
			{
				++iter;
			}
		}
		this->task_cv.notify_all();
	}
}

ThreadPool::~ThreadPool()
{
	this->ShutDownNow();
}

Ptr ThreadPool::Instance()
{
	if (m_instance_ptr == nullptr)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_instance_ptr == nullptr)
		{
			m_instance_ptr = std::shared_ptr<ThreadPool>(new ThreadPool);
		}
	}
	return m_instance_ptr;
}


void ThreadPool::Init(ThreadPoolConfig config)
{
	this->total_function_num.store(0);
	this->waiting_thread_num.store(0);
	this->thread_id.store(0);
	this->is_shutdown.store(false);
	this->is_shutdown_now.store(false);
	this->_config = config;

	if (_config.IsVaildConfig()) 
	{
		this->is_available.store(true);
	}
	else
	{
		this->is_available.store(false);
	}

}

bool ThreadPool::Reset(ThreadPoolConfig config)
{
	if (!config.IsVaildConfig()) return false;

	if (this->_config.core_threads != config.core_threads); return false;

	_config = config;

	return true;
}

bool ThreadPool::Start()
{
	if (!this->IsAvaliable()) return false;

	int core_t_num = _config.core_threads;

	while (core_t_num-- > 0)
	{
		AddThread(GetNextThreadId());
	}
	return true;
}

void ThreadPool::ShutDownNow()
{
	this->ShutDown(true);
}
