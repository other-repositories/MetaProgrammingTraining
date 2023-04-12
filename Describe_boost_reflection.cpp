#include <iostream>
#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <cassert>
#include <boost/fiber/fiber.hpp>
#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <tuple>
#include <boost/json.hpp>
/*
using namespace std::chrono_literals;

using Task = std::function<void()>;

template<class T>
class MutexQueue {
public:
	void push(const T& value) {
		{
			std::unique_lock<std::mutex> lock(mt);
			m_tasks.push(value);
		}
		cv.notify_one();
	}
	T pop() {
		T copy;
		{
			std::unique_lock<std::mutex> lock(mt);
			cv.wait(lock, [&]() { return !m_tasks.empty(); });
			copy = m_tasks.front();
			m_tasks.pop();
		}
		return copy;
	}
private:
	std::queue<T> m_tasks;
	std::mutex mt;
	std::condition_variable cv;
};

class ThreadPullStatic {
public:
	ThreadPullStatic() : m_pool(std::thread::hardware_concurrency() - 1), is_joinable(false) {
		startWorking();
	}

	void push(Task task) {
		m_tasks.push(task);
	}
	void join() {
		for (int i = 0;i<m_pool.size();i++) {
			m_tasks.push({});
		}	
		for (auto& thread : m_pool) {
			thread.join();
		}
		is_joinable = true;
	}
	~ThreadPullStatic() {
		if(!is_joinable)
			join();
	}
private:

	void startWorking() {
		for (auto& thread : m_pool) {
			thread = std::thread([this]() {
				workingJob();
			});
		}
	}
	void workingJob() {
		while (true) {
			auto task = m_tasks.pop();
			if (!task)
				break;
			task();
		}
	}
	std::vector <std::thread> m_pool;
	MutexQueue<Task> m_tasks;
	bool is_joinable;
};


void test_thread_pool() {
	ThreadPullStatic pool;
	std::atomic shared_counter = 0;
	const int count = 10'000'000;
	for (size_t i = 0; i < count; ++i) {
		pool.push([&] {
			auto val = shared_counter.load();
			while (!shared_counter.compare_exchange_weak(val, val + 1));
			});
	}
	pool.join();
	assert(count == shared_counter.load());
}

void fiberTest() {
	namespace ctx = boost::context;
	int data = 0;
	ctx::fiber f1{ [&data](ctx::fiber&& f2) {
		std::cout << "f1: entered first time: " << data << std::endl;
		data += 1;
		f2 = std::move(f2).resume();
		std::cout << "f1: entered second time: " << data << std::endl;
		data += 1;
		f2 = std::move(f2).resume();
		std::cout << "f1: entered third time: " << data << std::endl;
		return std::move(f2);
	} };
	f1 = std::move(f1).resume();
	std::cout << "f1: returned first time: " << data << std::endl;
	data += 1;
	f1 = std::move(f1).resume();
	std::cout << "f1: returned second time: " << data << std::endl;
	data += 1;
	f1 = std::move(f1).resume_with([&data](ctx::fiber&& f2) {
		std::cout << "f2: entered: " << data << std::endl;
		data = -1;
		return std::move(f2);
		});
	std::cout << "f1: returned third time" << std::endl;
}
*/

using namespace boost::describe;
namespace desc = boost::describe;

template<class T, template<class...> class L, class... D>
auto struct_to_tuple_impl(T const& t, L<D...>)
{
	return std::make_tuple(t.*D::pointer...);
}

template<class T,
	class Dm = desc::describe_members<T,
	desc::mod_public | desc::mod_inherited>,
	class En = std::enable_if_t<!std::is_union<T>::value> >
auto struct_to_tuple(T const& t)
{
	return struct_to_tuple_impl(t, Dm());
}

#include <boost/core/type_name.hpp>
#include <iostream>

struct X
{
	int a = 1;
};

BOOST_DESCRIBE_STRUCT(X, (), (a))

struct Y
{
	float b = 3.14f;
};

BOOST_DESCRIBE_STRUCT(Y, (), (b))

struct Z : X, Y
{
};

BOOST_DESCRIBE_STRUCT(Z, (X, Y), ())


struct Output {
	int b;
	std::string str = "testing";
};

enum class E
{
	v1 = 11,
	v2,
	v3 = 5
};

BOOST_DESCRIBE_ENUM(E, v1, v2, v3)
BOOST_DESCRIBE_STRUCT(Output, (), (b, str))

using boost::describe::operators::operator<<;

template<class T,
	class D1 = boost::describe::describe_members<T,
	boost::describe::mod_public | boost::describe::mod_protected>,
	class D2 = boost::describe::describe_members<T, boost::describe::mod_private>,
	class En = std::enable_if_t<boost::mp11::mp_empty<D2>::value && !std::is_union<T>::value> >

void tag_invoke(boost::json::value_from_tag const&, boost::json::value& v, T const& t)
{
	auto& obj = v.emplace_object();

	boost::mp11::mp_for_each<D1>([&](auto D) {

		obj[D.name] = boost::json::value_from(t.*D.pointer);

		});
}

struct A
{
	int x;
	int y;
};

BOOST_DESCRIBE_STRUCT(A, (), (x, y))

struct B
{
	std::vector<A> v;
	std::map<std::string, A> m;
};

BOOST_DESCRIBE_STRUCT(B, (), (v, m))


void Enumiration() {
	boost::mp11::mp_for_each< boost::describe::describe_enumerators<E> >([](auto D) {
		std::cout << D.name << "\n";
		}); // get enums names
}

void Logging() {
	Output a;
	std::cout << a << "\n"; // get struct describe output
	std::cout << typeid(a).name() << "\n"; // differenes
}

void RecursiveStructToTuple() {
	Z z;
	auto tp = struct_to_tuple(z);
	std::cout << boost::core::type_name<decltype(tp)>() << ": "
		<< std::get<0>(tp) << ", " << std::get<1>(tp) << "\n";
}

void toJson() {
	B b{ { { 1, 2 }, { 3, 4 } }, { { "k1", { 5, 6 } }, { "k2", { 7, 8 } } } };
	std::cout << boost::json::value_from(b) << std::endl;
}

int main() {
	//Enumiration();
	//Logging();
	//RecursiveStructToTuple();
	toJson();
}
