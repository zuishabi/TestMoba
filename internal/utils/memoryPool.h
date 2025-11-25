//
// Created by 张俏悦 on 2025/11/25.
//

#ifndef TESTMOBA_MEMORYPOOL_H
#define TESTMOBA_MEMORYPOOL_H
#include <iostream>
#include <vector>

static inline std::size_t align_up(std::size_t n,std::size_t align) {
    return (n + (align - 1)) & ~(align - 1);
}

class FixedSizePool {
public:
    explicit FixedSizePool(std::size_t block_size,std::size_t block_per_page = 1024) :
        block_size_(adjust_block_size(block_size)),blocks_per_page_(block_per_page){
        free_list_ = nullptr;
    }

    ~FixedSizePool() {
        for (void* p : pages_) {
            ::operator delete[] (p);
        }
    }

    // 分配一小块内存
    void* allocate() {
        if (!free_list_) {
            // 所有的小块都被拿走了，拓展内存池
            expand();
        }

        Node* head = free_list_;
        free_list_ = head->next;
        return head;
    }

    // 归还小块内存
    void deallocate(void* p) {
        if (!p)return;
        Node* node = static_cast<Node *>(p);
        node->next = free_list_;
        free_list_ = node;
    }

    std::size_t block_size()const {
        return block_size_;
    }
private:
    std::size_t adjust_block_size(std::size_t s) {
        std::size_t min = sizeof(void*);
        std::size_t res = align_up(s < min ? min : s,alignof(void*));
        return res;
    }

    // 每次向系统申请一页的内存，并把这一页的内存切为很多小块，并把这些小块挂到空闲的链表中
    void expand() {
        // 一整页内存的字节大小
        std::size_t page_bytes =  block_size_ * blocks_per_page_;
        char* page = static_cast<char*>(::operator new[](page_bytes));
        pages_.push_back(page);

        // 拆分成blocks个小块，串成free_list
        for (std::size_t i = 0;i < blocks_per_page_;i++) {
            char* addr = page + i * block_size_;
            Node *n = reinterpret_cast<Node *>(addr);
            n->next = free_list_;
            free_list_ = n;
        }
    }

    struct Node {
        Node* next;
    };
    std::size_t block_size_; // 每个小块的大小
    std::size_t blocks_per_page_;

    Node* free_list_; //空闲小块的单向链表头
    std::vector<void*> pages_; // 所有已经分配的’页‘，析构统一释放
};

#endif //TESTMOBA_MEMORYPOOL_H