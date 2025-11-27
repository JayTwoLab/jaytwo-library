#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#include "j2_library/queue/queue.hpp"

using namespace j2::queue;

// 패킷 구조체
struct packet
{
    int id;
    int type;        // 1: normal, 2: high
    std::string msg;

    // 이 생성자를 추가
    packet() = default;

    packet(int id_, int type_, std::string msg_)
        : id(id_)
        , type(type_)
        , msg(std::move(msg_))
    {
    }
};


// 함수 선언
void example_unbounded_int_queue();
void example_bounded_packet_queue();
void example_wait_dequeue_with_thread();

// main 을 최상단에 배치
int main()
{
    example_unbounded_int_queue();
    example_bounded_packet_queue();
    example_wait_dequeue_with_thread();
    return 0;
}

// 무제한 int 큐 예제: enqueue, emplace, try_dequeue, size, empty, is_bounded, capacity
void example_unbounded_int_queue()
{
    std::cout << "=== example_unbounded_int_queue ===\n";

    // max_size = 0 → 무제한
    concurrent_queue<int> q; // 기본값: max_size = 0, overflow_policy::reject_new

    std::cout << "is_bounded = " << std::boolalpha << q.is_bounded() << "\n";
    // is_bounded = false

    std::cout << "capacity   = " << q.capacity() << "\n";
    // capacity   = 0

    std::cout << "empty      = " << q.empty() << "\n";
    // empty      = true

    int x = 10;
    q.enqueue(x);          // lvalue enqueue
    q.enqueue(20);         // rvalue enqueue
    q.emplace(30);         // emplace 로 직접 생성

    std::cout << "size after enqueue/emplace = " << q.size() << "\n";
    // size after enqueue/emplace = 3

    int value = 0;
    while (q.try_dequeue(value))
    {
        std::cout << "try_dequeue: " << value << "\n";
    }
    // try_dequeue: 10
    // try_dequeue: 20
    // try_dequeue: 30

    std::cout << "size after all dequeue = " << q.size() << "\n\n";
    // size after all dequeue = 0
}

// 크기 제한 packet 큐 예제: overflow_policy, dequeue_if, size 조건, head 조건
void example_bounded_packet_queue()
{
    std::cout << "=== example_bounded_packet_queue ===\n";

    // 크기 4, 꽉 차면 오래된 것 버리기
    concurrent_queue<packet> q_drop(4, overflow_policy::drop_oldest);
    std::cout << "drop_oldest queue - is_bounded = " << std::boolalpha << q_drop.is_bounded()
        << ", capacity = " << q_drop.capacity() << "\n";
    // drop_oldest queue - is_bounded = true, capacity = 4

    // 크기 3, 꽉 차면 새 것 거부
    concurrent_queue<packet> q_reject(3, overflow_policy::reject_new);
    std::cout << "reject_new  queue - is_bounded = " << std::boolalpha << q_reject.is_bounded()
        << ", capacity = " << q_reject.capacity() << "\n";
    // reject_new  queue - is_bounded = true, capacity = 3

    // drop_oldest 큐에 몇 개 넣기
    q_drop.emplace(1, 1, "normal-1");
    q_drop.emplace(2, 2, "high-1");
    q_drop.emplace(3, 1, "normal-2");
    q_drop.emplace(4, 2, "high-2");
    q_drop.emplace(5, 1, "normal-3"); // 오래된 것부터 밀려나감

    std::cout << "q_drop size = " << q_drop.size() << "\n";
    // q_drop size = 4

    packet pkt;

    // 1) head 조건 기반 dequeue_if: type == 2 (high 인 것만 꺼내기)
    bool ok = q_drop.dequeue_if(pkt,
        [](const packet& head, std::size_t /*sz*/) {
            return head.type == 2;
        });

    std::cout << "dequeue_if (head.type == 2) ok = " << std::boolalpha << ok << "\n";
    // dequeue_if (head.type == 2) ok = true

    if (ok)
    {
        std::cout << "  popped: id=" << pkt.id
            << ", type=" << pkt.type
            << ", msg=" << pkt.msg << "\n";
        //   popped: id=2, type=2, msg=high-1
    }

    std::cout << "q_drop size after dequeue_if(head) = " << q_drop.size() << "\n";
    // q_drop size after dequeue_if(head) = 3

    // 2) size 조건 기반 dequeue_if: 큐 크기가 2보다 클 때만 pop
    ok = q_drop.dequeue_if(pkt,
        [](const packet& /*head*/, std::size_t sz) {
            return sz > 2;
        });

    std::cout << "dequeue_if (size > 2) ok = " << std::boolalpha << ok << "\n";
    // dequeue_if (size > 2) ok = true

    if (ok)
    {
        std::cout << "  popped: id=" << pkt.id
            << ", type=" << pkt.type
            << ", msg=" << pkt.msg << "\n";
        //   popped: id=3, type=1, msg=normal-2
    }

    std::cout << "q_drop size after dequeue_if(size) = " << q_drop.size() << "\n";
    // q_drop size after dequeue_if(size) = 2

    // 남은 것들은 일반 try_dequeue 로 모두 꺼내기
    while (q_drop.try_dequeue(pkt))
    {
        std::cout << "remaining: id=" << pkt.id
            << ", type=" << pkt.type
            << ", msg=" << pkt.msg << "\n";
    }
    // remaining: id=4, type=2, msg=high-2
    // remaining: id=5, type=1, msg=normal-3

    std::cout << "q_drop empty = " << q_drop.empty() << "\n\n";
    // q_drop empty = true
}

// wait_dequeue 를 이용해서 다른 쓰레드에서 꺼내는 예제
void example_wait_dequeue_with_thread()
{
    std::cout << "=== example_wait_dequeue_with_thread ===\n";
    // [producer] enqueue 42

    concurrent_queue<int> q;

    // 소비자 스레드: wait_dequeue 로 대기
    std::thread consumer([&q] {
        int value = 0;
        q.wait_dequeue(value); // 비어 있으면 여기서 대기

        std::cout << "[consumer] got value = " << value << "\n";
        // [consumer] got value = 42
        });

    // 잠시 기다렸다가 생산
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "[producer] enqueue 42\n";
    q.enqueue(42);

    consumer.join();

    std::cout << "queue empty = " << std::boolalpha << q.empty() << "\n\n";
    // queue empty = true
}
