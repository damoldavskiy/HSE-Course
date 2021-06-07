#pragma once

#include "../control/control.h"

struct DefaultDeleter {
    template <typename T>
    void operator()(T* pointer) {
        delete pointer;
    }
};

struct ArrayDeleter {
    template <typename T>
    void operator()(T* pointer) {
        delete[] pointer;
    }
};

template <typename T>
struct DeleteArrayType {
    using type = T;
    using deleter = DefaultDeleter;
};

template <typename T>
struct DeleteArrayType<T[]> {
    using type = T;
    using deleter = ArrayDeleter;
};

template <typename T, std::size_t N>
struct DeleteArrayType<T[N]> {
    using type = T;
    using deleter = ArrayDeleter;
};

// SharedPtr
template <typename T>
class WeakPtr;

template <typename T>
class SharedPtr {
public:
    using element_type = typename DeleteArrayType<T>::type;
    using deleter_type = typename DeleteArrayType<T>::deleter;

    constexpr SharedPtr() noexcept = default;
    ~SharedPtr();

    template <typename Y>
    explicit SharedPtr(Y* p);

    template <typename Y, typename Deleter>
    SharedPtr(Y* p, Deleter deleter) noexcept;

    SharedPtr(const SharedPtr& other) noexcept;
    SharedPtr(SharedPtr&& other) noexcept;

    SharedPtr& operator=(const SharedPtr& other) noexcept;

    template <typename Y>
    SharedPtr& operator=(const SharedPtr<Y>& other) noexcept;

    SharedPtr& operator=(SharedPtr&& other) noexcept;

    template <typename Y>
    SharedPtr& operator=(SharedPtr<Y>&& other) noexcept;

    // Modifiers
    void Reset() noexcept;

    template <typename Y>
    void Reset(Y* p) noexcept;

    template <typename Y, typename Deleter>
    void Reset(Y* p, Deleter deleter) noexcept;

    void Swap(SharedPtr& other) noexcept;

    // Observers
    T* Get() const noexcept;
    int64_t UseCount() const noexcept;
    T& operator*() const noexcept;
    T* operator->() const noexcept;
    element_type& operator[](std::ptrdiff_t idx) const;
    explicit operator bool() const noexcept;

    template <typename U>
    friend class WeakPtr;

private:
    element_type* value_{nullptr};
    SharedWeakCount* block_{nullptr};
};

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
    if (block_ == nullptr) {
        assert(value_ == nullptr);
        return;
    }

    block_->Remove();
    if (block_->Empty()) {
        delete block_;
    }
}

template <typename T>
template <typename Y>
SharedPtr<T>::SharedPtr(Y* p) : value_(p), block_(new ControlBlock<Y, deleter_type>(p)) {
}

template <typename T>
template <typename Y, typename Deleter>
SharedPtr<T>::SharedPtr(Y* p, Deleter) noexcept
    : value_(p), block_(new ControlBlock<Y, Deleter>(p)) {
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& other) noexcept
    : value_(other.value_), block_(other.block_) {
    block_->Add();
}

template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept : value_(other.value_), block_(other.block_) {
    other.value_ = nullptr;
    other.block_ = nullptr;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other) noexcept {
    Reset();
    this->value_ = other.value_;
    block_ = other.block_;
    block_->Add();
    return *this;
}

template <typename T>
template <typename Y>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<Y>& other) noexcept {
    Reset();
    value_ = other.value_;
    block_ = other.block_;
    block_->Add();
    return *this;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) noexcept {
    Reset();
    value_ = other.value_;
    block_ = other.block_;
    other.value_ = nullptr;
    other.block_ = nullptr;
    return *this;
}

template <typename T>
template <typename Y>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<Y>&& other) noexcept {
    Reset();
    value_ = other.value_;
    block_ = other.block_;
    other.value_ = nullptr;
    other.block_ = nullptr;
    return *this;
}

template <typename T>
void SharedPtr<T>::Reset() noexcept {
    if (block_ == nullptr) {
        return;
    }
    block_->Remove();
    value_ = nullptr;
    block_ = nullptr;
}

template <typename T>
template <typename Y>
void SharedPtr<T>::Reset(Y* p) noexcept {
    if (block_ != nullptr) {
        block_->Remove();
        if (block_->Empty()) {
            delete block_;
        }
    }

    value_ = p;
    block_ = new ControlBlock<Y, deleter_type>(p);
}

template <typename T>
template <typename Y, typename Deleter>
void SharedPtr<T>::Reset(Y* p, Deleter) noexcept {
    if (block_ != nullptr) {
        block_->Remove();
        if (block_->Empty()) {
            delete block_;
        }
    }

    value_ = p;
    block_ = new ControlBlock<Y, Deleter>(p);
}

template <typename T>
void SharedPtr<T>::Swap(SharedPtr& other) noexcept {
    std::swap(block_, other.block_);
    std::swap(value_, other.value_);
}

template <typename T>
T* SharedPtr<T>::Get() const noexcept {
    return value_;
}

template <typename T>
int64_t SharedPtr<T>::UseCount() const noexcept {
    if (block_ == nullptr) {
        return 0;
    }
    return block_->Get();
}

template <typename T>
T& SharedPtr<T>::operator*() const noexcept {
    return *value_;
}

template <typename T>
T* SharedPtr<T>::operator->() const noexcept {
    return value_;
}

template <typename T>
typename SharedPtr<T>::element_type& SharedPtr<T>::operator[](std::ptrdiff_t idx) const {
    return value_[idx];
}

template <typename T>
SharedPtr<T>::operator bool() const noexcept {
    return block_ != nullptr;
}

// WeakPtr
template <typename T>
class WeakPtr {

public:
    using element_type = typename DeleteArrayType<T>::type;
    using deleter_type = typename DeleteArrayType<T>::deleter;

    // Special-member functions
    constexpr WeakPtr() noexcept = default;
    template <typename Y>
    explicit WeakPtr(const SharedPtr<Y>& other);
    WeakPtr(const WeakPtr& other) noexcept;
    WeakPtr(WeakPtr&& other) noexcept;
    template <typename Y>
    WeakPtr& operator=(const SharedPtr<Y>& other);
    WeakPtr& operator=(const WeakPtr& other) noexcept;
    WeakPtr& operator=(WeakPtr&& other) noexcept;

    ~WeakPtr();

    // Modifiers
    void Reset() noexcept;
    void Swap(WeakPtr<T>& other) noexcept;

    // Observers
    bool Expired() noexcept;
    SharedPtr<T> Lock() const noexcept;

    template <typename U>
    friend class SharedPtr;

public:
    element_type* value_{nullptr};
    SharedWeakCount* block_{nullptr};
};

template <typename T>
template <typename Y>
WeakPtr<T>::WeakPtr(const SharedPtr<Y>& other) : value_(other.value_), block_(other.block_) {
    block_->AddWeak();
}

template <typename T>
WeakPtr<T>::WeakPtr(const WeakPtr& other) noexcept : value_(other.value_), block_(other.block_) {
    block_->AddWeak();
}

template <typename T>
WeakPtr<T>::WeakPtr(WeakPtr&& other) noexcept : value_(other.value_), block_(other.block_) {
    other.value_ = nullptr;
    other.block_ = nullptr;
}

template <typename T>
template <typename Y>
WeakPtr<T>& WeakPtr<T>::operator=(const SharedPtr<Y>& other) {
    Reset();
    value_ = other.value_;
    block_ = other.block_;
    block_->AddWeak();
    return *this;
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr& other) noexcept {
    Reset();
    value_ = other.value_;
    block_ = other.block_;
    block_->AddWeak();
    return *this;
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr&& other) noexcept {
    Reset();
    value_ = other.value_;
    block_ = other.block_;
    other.value_ = nullptr;
    other.block_ = nullptr;
    return this;
}

template <typename T>
WeakPtr<T>::~WeakPtr() {
    if (block_ == nullptr) {
        assert(value_ == nullptr);
        return;
    }

    block_->RemoveWeak();
    if (block_->Empty()) {
        delete block_;
    }
}

template <typename T>
void WeakPtr<T>::Reset() noexcept {
    if (block_ != nullptr) {
        block_->RemoveWeak();
        if (block_->Empty()) {
            delete block_;
        }
    }

    value_ = nullptr;
    block_ = nullptr;
}

template <typename T>
void WeakPtr<T>::Swap(WeakPtr<T>& other) noexcept {
    std::swap(value_, other.value_);
    std::swap(block_, other.block_);
}

template <typename T>
bool WeakPtr<T>::Expired() noexcept {
    return block_ == nullptr || block_->Get() == 0;
}

template <typename T>
SharedPtr<T> WeakPtr<T>::Lock() const noexcept {
    SharedPtr<T> shared;
    if (block_ == nullptr || block_->Get() == 0) {
        return shared;
    }

    shared.value_ = value_;
    shared.block_ = block_;
    return shared;
}
