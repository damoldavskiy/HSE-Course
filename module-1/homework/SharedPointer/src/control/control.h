#pragma once

#include <atomic>
#include <cassert>

class SharedCount {
public:
    void Add() {
        count_++;
    }

    virtual bool Remove() {
        if (Get() == 0) {
            return false;
        }
        count_--;
        return true;
    }

    std::size_t Get() const {
        return count_.load();
    }

    virtual bool Empty() {
        return Get() > 0;
    }

protected:
    std::atomic<std::size_t> count_{0};
};

class SharedWeakCount : public SharedCount {
public:
    void AddWeak() {
        count_weak_++;
    }

    bool RemoveWeak() {
        if (GetWeak() == 0) {
            return false;
        }
        count_weak_--;
        return true;
    }

    std::size_t GetWeak() const {
        return count_weak_.load();
    }

    bool Empty() override {
        return Get() == 0 && GetWeak() == 0;
    }

protected:
    std::atomic<std::size_t> count_weak_{0};
};

template <typename T, typename Deleter>
class ControlBlock : public SharedWeakCount {
public:
    explicit ControlBlock(T* value) : value_(value) {
        assert(value_ != nullptr);
        Add();
    }

    ControlBlock(ControlBlock&) = delete;
    void operator=(ControlBlock&) = delete;

    ~ControlBlock() {
        assert(value_ == nullptr);
    }

    bool Remove() override {
        if (SharedCount::Remove()) {
            if (Get() == 0) {
                Deleter deleter;
                deleter(value_);
                value_ = nullptr;
            }
            return true;
        }
        return false;
    }

private:
    T* value_;
};
