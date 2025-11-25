#include "../include/my_memory_resource.hpp"
#include "../include/queue.hpp"
#include <iostream>
#include <string>

struct Person {
    int identifier;
    std::string name;
    int task_count;

    Person(int id = 0, std::string nm = "", int tasks = 0) : identifier(id), name(nm), task_count(tasks) {}

    friend std::ostream &operator<<(std::ostream &os, const Person &p) {
        os << "Person #" << p.identifier << " " << p.name << " (tasks: " << p.task_count << ")";
        return os;
    }
};

int main() {
    CustomMemoryPool pool;

    auto person_queue = CustomQueue<Person, std::pmr::polymorphic_allocator<Person>>(std::pmr::polymorphic_allocator<Person>{&pool});

    person_queue.enqueue(Person(1, "Smith", 5));
    person_queue.enqueue(Person(2, "Johnson", 3));
    person_queue.enqueue(Person(3, "Williams", 7));

    std::cout << "Person queue for task verification:\n";
    std::cout << "Total in queue: " << person_queue.length() << " persons\n\n";
    std::cout << "First: " << person_queue.get_front() << std::endl;
    std::cout << "Last: " << person_queue.get_back() << std::endl;

    std::cout << "\nQueue traversal:\n";
    int pos = 1;
    for (const auto& person : person_queue) {
        std::cout << pos << ". " << person << std::endl;
        pos++;
    }
    
    while (!person_queue.empty()) {
        Person current = person_queue.get_front();
        std::cout << "Processing: " << current << std::endl;
        person_queue.remove_front();
        std::cout << "Remaining: " << person_queue.length() << " persons\n";
        
        if (!person_queue.empty()) {
            std::cout << "Next: " << person_queue.get_front() << "\n" << std::endl;
        }
    }

    std::cout << "All persons processed.\n";

    return 0;
}
