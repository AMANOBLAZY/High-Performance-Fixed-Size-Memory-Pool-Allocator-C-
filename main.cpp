
#include <iostream>
#include <memory>

class Fixed_size_MemoryPool {
private: 
    typedef struct node {             
        struct node* next;
    } Node;
    long long estimated_no_of_objects;
    size_t size_of_one_object;     
    std::unique_ptr<char[]> memory_block_location; 
    struct node* free_list_head; 
public:
    Fixed_size_MemoryPool(long long x, size_t y) :  
    estimated_no_of_objects(x), 
    size_of_one_object(y < sizeof(Node) ? sizeof(Node) : y),   
    // FIXED: Corrected std::make_unique initialization syntax
    memory_block_location(std::make_unique<char[]>(x * (y < sizeof(Node) ? sizeof(Node) : y))), 
    free_list_head(nullptr)
    {
        char* current_byte = memory_block_location.get();  
        
        for(int i = 0; i < estimated_no_of_objects; ++i){
            Node* current_node = reinterpret_cast<Node*>(current_byte);  
            current_node->next = free_list_head;        
            free_list_head = current_node;  
            current_byte += size_of_one_object; 
        }
    }
    
    void* allocate() {        
        if(free_list_head == nullptr){
            std::cerr << "Memory Pool is out of memory!\n"; 
            return nullptr;
        }
        void* allocated_space = free_list_head;
        free_list_head = free_list_head->next;  
        return allocated_space; 
    }

    void deallocate(void* ptr){
        if(ptr == nullptr) return;
        Node* returning_node = reinterpret_cast<Node*>(ptr); 
        returning_node->next = free_list_head;  
        free_list_head = returning_node;    
    }
};

struct Player {
    int id;
    int health;
};

int main() {
    // FIXED: Added missing trailing semicolon
    Fixed_size_MemoryPool pool(5, sizeof(Player));

    void* mem1 = pool.allocate();    
    
    Player* player1 = new (mem1) Player{101, 100}; 

    std::cout << "Player ID: " << player1->id << ", Health: " << player1->health << "\n";

    player1->~Player();     
    pool.deallocate(mem1);  

    return 0;
}
