#include <gtest/gtest.h>
#include "my_memory_resource.hpp"
#include "queue.hpp"
#include <string>

struct Person {
    int identifier;
    std::string name;
    int task_count;

    Person(int id = 0, std::string nm = "", int tasks = 0) 
        : identifier(id), name(nm), task_count(tasks) {}

    bool operator==(const Person& other) const {
        return identifier == other.identifier && name == other.name && task_count == other.task_count;
    }
};

TEST(PersonQueueTest, InitialState) {
    CustomMemoryPool pool;
    auto person_queue = CustomQueue<Person, std::pmr::polymorphic_allocator<Person>>(
        std::pmr::polymorphic_allocator<Person>{&pool});
    
    EXPECT_TRUE(person_queue.empty());
    EXPECT_EQ(person_queue.length(), 0);
    
    person_queue.enqueue(Person(1, "Smith", 5));
    person_queue.enqueue(Person(2, "Johnson", 3));
    
    EXPECT_FALSE(person_queue.empty());
    EXPECT_EQ(person_queue.length(), 2);
}

TEST(PersonQueueTest, AddRemove) {
    CustomMemoryPool pool;
    auto person_queue = CustomQueue<Person, std::pmr::polymorphic_allocator<Person>>(
        std::pmr::polymorphic_allocator<Person>{&pool});
    
    Person p1(1, "Smith", 5);
    Person p2(2, "Johnson", 3);
    
    person_queue.enqueue(p1);
    person_queue.enqueue(p2);
    
    EXPECT_EQ(person_queue.get_front(), p1);
    EXPECT_EQ(person_queue.get_back(), p2);
    
    person_queue.remove_front();
    EXPECT_EQ(person_queue.get_front(), p2);
    EXPECT_EQ(person_queue.length(), 1);
    
    person_queue.remove_front();
    EXPECT_TRUE(person_queue.empty());
}

TEST(PersonQueueTest, Accessors) {
    CustomMemoryPool pool;
    auto person_queue = CustomQueue<Person, std::pmr::polymorphic_allocator<Person>>(
        std::pmr::polymorphic_allocator<Person>{&pool});
    
    person_queue.enqueue(Person(1, "Smith", 5));
    person_queue.enqueue(Person(2, "Johnson", 3));
    person_queue.enqueue(Person(3, "Williams", 7));
    
    EXPECT_EQ(person_queue.get_front().identifier, 1);
    EXPECT_EQ(person_queue.get_front().name, "Smith");
    EXPECT_EQ(person_queue.get_front().task_count, 5);
    
    EXPECT_EQ(person_queue.get_back().identifier, 3);
    EXPECT_EQ(person_queue.get_back().name, "Williams");
    EXPECT_EQ(person_queue.get_back().task_count, 7);
}

TEST(PersonQueueTest, Iterator) {
    CustomMemoryPool pool;
    auto person_queue = CustomQueue<Person, std::pmr::polymorphic_allocator<Person>>(
        std::pmr::polymorphic_allocator<Person>{&pool});
    
    person_queue.enqueue(Person(1, "Smith", 5));
    person_queue.enqueue(Person(2, "Johnson", 3));
    person_queue.enqueue(Person(3, "Williams", 7));
    
    auto it = person_queue.start();
    auto end = person_queue.finish();
    EXPECT_EQ(it->identifier, 1);
    EXPECT_EQ(it->name, "Smith");
    ++it;
    EXPECT_EQ(it->identifier, 2);
    EXPECT_EQ(it->name, "Johnson");
    it++;
    EXPECT_EQ(it->identifier, 3);
    EXPECT_EQ(it->name, "Williams");
    ++it;
    EXPECT_TRUE(it == end);
}

TEST(PersonQueueTest, Loop) {
    CustomMemoryPool pool;
    auto person_queue = CustomQueue<Person, std::pmr::polymorphic_allocator<Person>>(
        std::pmr::polymorphic_allocator<Person>{&pool});
    
    person_queue.enqueue(Person(1, "Smith", 5));
    person_queue.enqueue(Person(2, "Johnson", 3));
    person_queue.enqueue(Person(3, "Williams", 7));
    
    EXPECT_EQ(person_queue.length(), 3);

    int counter = 0;
    for (const auto& person : person_queue) {
        EXPECT_TRUE(person.identifier >= 1 && person.identifier <= 3);
        EXPECT_FALSE(person.name.empty());
        EXPECT_GE(person.task_count, 0);
        counter++;
    }
    EXPECT_EQ(counter, 3);
}

TEST(PersonQueueTest, Clear) {
    CustomMemoryPool pool;
    auto person_queue = CustomQueue<Person, std::pmr::polymorphic_allocator<Person>>(
        std::pmr::polymorphic_allocator<Person>{&pool});
    
    person_queue.enqueue(Person(1, "Smith", 5));
    person_queue.enqueue(Person(2, "Johnson", 3));
    person_queue.enqueue(Person(3, "Williams", 7));
    
    EXPECT_EQ(person_queue.length(), 3);
    EXPECT_FALSE(person_queue.empty());
    
    person_queue.purge();
    
    EXPECT_EQ(person_queue.length(), 0);
    EXPECT_TRUE(person_queue.empty());
}

TEST(PersonQueueTest, Move) {
    CustomMemoryPool pool;
    auto person_queue = CustomQueue<Person, std::pmr::polymorphic_allocator<Person>>(
        std::pmr::polymorphic_allocator<Person>{&pool});
    
    Person temp(1, "Smith", 5);
    person_queue.enqueue(std::move(temp));
    
    EXPECT_EQ(person_queue.get_front().identifier, 1);
    EXPECT_EQ(person_queue.get_front().name, "Smith");
    EXPECT_EQ(person_queue.get_front().task_count, 5);
}

TEST(PersonQueueTest, MultipleQueues) {
    CustomMemoryPool pool;
 
    auto queue1 = CustomQueue<Person, std::pmr::polymorphic_allocator<Person>>(
        std::pmr::polymorphic_allocator<Person>{&pool});
    
    auto queue2 = CustomQueue<Person, std::pmr::polymorphic_allocator<Person>>(
        std::pmr::polymorphic_allocator<Person>{&pool});
    
    queue1.enqueue(Person(1, "Smith", 5));
    queue2.enqueue(Person(2, "Johnson", 3));
    
    EXPECT_EQ(queue1.get_front().identifier, 1);
    EXPECT_EQ(queue2.get_front().identifier, 2);
    EXPECT_EQ(queue1.length(), 1);
    EXPECT_EQ(queue2.length(), 1);
}

TEST(PersonQueueTest, EmptyBehavior) {
    CustomMemoryPool pool;
    auto person_queue = CustomQueue<Person, std::pmr::polymorphic_allocator<Person>>(
        std::pmr::polymorphic_allocator<Person>{&pool});
    
    EXPECT_TRUE(person_queue.empty());
    EXPECT_EQ(person_queue.length(), 0);
    EXPECT_EQ(person_queue.start(), person_queue.finish());
}

TEST(PersonQueueTest, Comparison) {
    Person p1(1, "Smith", 5);
    Person p2(1, "Smith", 5);
    Person p3(2, "Johnson", 3);
    
    EXPECT_EQ(p1, p2);
    EXPECT_FALSE(p1 == p3);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
