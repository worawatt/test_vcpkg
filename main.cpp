#include <boost/asio.hpp>

#include <iostream>
#include <memory>
#include <queue>

struct item_base
{
    void (*execute_)(std::shared_ptr<item_base> &);
};

template <class Func>
struct item : item_base
{
    item(Func f) : function_(std::move(f))
    {
        execute_ = [](std::shared_ptr<item_base> &p)
        {
            Func tmp(std::move(static_cast<item *>(p.get())->function_));
            p.reset();
            tmp();
        };
    }

    Func function_;
};

typedef std::queue<std::shared_ptr<item_base>> queue_t;

template <class Func>
void queue_submit(queue_t& q, Func f)
{
    q.push(std::make_shared<item<Func>>(std::move(f)));
}

struct minimal_io_executor
{
    boost::asio::execution_context *context_;
    queue_t& queue_;

    bool operator==(const minimal_io_executor &other) const noexcept
    {
        return context_ == other.context_ && queue_ == other.queue_;
    }

    bool operator!=(const minimal_io_executor &other) const noexcept
    {
        return !(*this == other);
    }

    boost::asio::execution_context &query(
        boost::asio::execution::context_t) const noexcept
    {
        return *context_;
    }

    static constexpr boost::asio::execution::blocking_t::never_t query(
        boost::asio::execution::blocking_t) noexcept
    {
        // This executor always has blocking.never semantics.
        return boost::asio::execution::blocking.never;
    }

    template <class F>
    void execute(F f) const
    {
        queue_submit(queue_, std::move(f));
    }
};

int main()
{
    boost::asio::execution_context context;
    queue_t queue;
    minimal_io_executor executor{&context, queue};

    boost::asio::co_spawn(
        executor, []() -> boost::asio::awaitable<void> {
            std::cout << "1" << std::endl;
            auto ex = co_await boost::asio::this_coro::executor;
            std::cout << "2" << std::endl;
            co_await boost::asio::post(ex, boost::asio::use_awaitable);
            std::cout << "3" << std::endl;
            co_return;
        },
        boost::asio::detached);

    while (!queue.empty()) {
        auto p(queue.front());
        queue.pop();
        std::cout << "pop" << std::endl;
        p->execute_(p);
    }
    return 0;
}
