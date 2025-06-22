#pragma once

#include <cassert>
#include <initializer_list>

#include "array_ptr.h"

using namespace std;

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    SimpleVector(const SimpleVector& other) :
        massiv_ptr_(other.size_),
        size_(other.size_),
        capacity_(other.capacity_)
    {
        copy(other.begin(), other.end(), massiv_ptr_.Get());
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            if (rhs.IsEmpty()) {
                Clear();
                return *this;
            }
            SimpleVector rhs_copy(rhs);
            capacity_ = rhs.capacity_;
            size_ = rhs.size_;
            massiv_ptr_.swap(rhs_copy.massiv_ptr_);
        }
        return *this;
    }

    SimpleVector(ReserveProxyObj other) {
        Reserve(other.GetReserve());
    }

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size)
        : massiv_ptr_(size),
        size_(size),
        capacity_(size)
    {
        fill(massiv_ptr_.Get(), massiv_ptr_.Get() + size, Type{});
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value)
        : massiv_ptr_(size),
        size_(size),
        capacity_(size)
    {
        fill(massiv_ptr_.Get(), massiv_ptr_.Get() + size_, value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
        : massiv_ptr_(init.size()),
        size_(init.size()),
        capacity_(init.size())
    {
        copy(init.begin(), init.end(), massiv_ptr_.Get());
    }

    SimpleVector(SimpleVector&& other) {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        massiv_ptr_.swap(other.massiv_ptr_);
    }

    SimpleVector operator=(SimpleVector&& other) {
        if (this != &other) {
            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);
            massiv_ptr_.swap(other.massiv_ptr_);
        }
        return *this;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> new_massiv(new_capacity);
            capacity_ = new_capacity;

            copy(massiv_ptr_.Get(), massiv_ptr_.Get() + size_, new_massiv.Get());
            massiv_ptr_.swap(new_massiv);
        }
    }

    void PushBack(Type&& item) {
        Resize(size_ + 1);
        massiv_ptr_[size_ - 1] = move(item);
    }

    void PushBack(const Type& item) {
        Resize(size_ + 1);
        massiv_ptr_[size_ - 1] = item;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {

        assert(pos >= begin() && pos <= end());

        Iterator it = begin() + distance(cbegin(), pos);
        auto dist = distance(cbegin(), pos);

        if (size_ == capacity_) {
            if (capacity_ == 0) {
                capacity_ = 1;
            }
            else {
                capacity_ *= 2;
            }

            ArrayPtr<Type> new_massiv(capacity_);
            Iterator it_new = new_massiv.Get() + dist;

            move(begin(), it, new_massiv.Get());
            move_backward(it, end(), new_massiv.Get() + size_ + 1);
            massiv_ptr_.swap(new_massiv);

            *it_new = move(value);
            ++size_;

            return it_new;

        }
        else {

            ++size_;
            move_backward(it, end() - 1, end());
            *it = move(value);

            return it;
        }
    }

    Iterator Insert(ConstIterator pos, const Type& value) {

        assert(pos >= begin() && pos <= end());

        Iterator it = begin() + distance(cbegin(), pos);
        auto dist = distance(cbegin(), pos);

        if (size_ == capacity_) {
            if (capacity_ == 0) {
                capacity_ = 1;
            }
            else {
                capacity_ *= 2;
            }

            ArrayPtr<Type> new_massiv(capacity_);
            Iterator it_new = new_massiv.Get() + dist;

            copy(begin(), it, new_massiv.Get());
            copy_backward(it, end(), new_massiv.Get() + size_ + 1);
            massiv_ptr_.swap(new_massiv);

            *it_new = value;
            ++size_;

            return it_new;

        }
        else {

            ++size_;
            copy_backward(it, end() - 1, end());
            *it = value;

            return it;
        }
    }

    void PopBack() noexcept {
        if (size_ == 0) {
            return;
        }
        --size_;
    }

    Iterator Erase(ConstIterator pos) {

        assert(pos >= begin() && pos < end());

        Iterator it = begin() + distance(cbegin(), pos);
        move(it + 1, end() - 1, it);
        --size_;

        return it;

    }

    void swap(SimpleVector& other) noexcept {
        other.massiv_ptr_.swap(massiv_ptr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return !size_;
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return massiv_ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return massiv_ptr_[index];
    }

    // Возвращает ссылку на элемент с индексом index
    Type& At(size_t index) {
        if (index >= size_) {
            throw out_of_range("");
        }
        return massiv_ptr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw out_of_range("");
        }
        return massiv_ptr_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    void Resize(size_t new_size) {
        if (new_size == size_) {
            return;
        }
        else if (new_size < size_) {
            size_ = new_size;
        }
        else {
            if (new_size < capacity_) {

                generate(massiv_ptr_.Get() + size_, massiv_ptr_.Get() + new_size, []() {
                    return Type{};
                    });
                size_ = new_size;

            }
            else {

                capacity_ = max(new_size, capacity_ * 2);
                ArrayPtr<Type> new_massive(capacity_);

                move(massiv_ptr_.Get(), massiv_ptr_.Get() + size_, new_massive.Get());
                generate(new_massive.Get() + size_, new_massive.Get() + new_size, []() {
                    return Type{};
                    });
                massiv_ptr_.swap(new_massive);
                size_ = new_size;

            }
        }
    }

    Iterator begin() noexcept {
        return massiv_ptr_.Get();
    }

    Iterator end() noexcept {
        return massiv_ptr_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        ConstIterator begin_ = massiv_ptr_.Get();
        return begin_;
    }

    ConstIterator end() const noexcept {
        ConstIterator end_ = massiv_ptr_.Get() + size_;
        return end_;
    }

    ConstIterator cbegin() const noexcept {
        return begin();
    }

    ConstIterator cend() const noexcept {
        return end();
    }

private:
    ArrayPtr<Type> massiv_ptr_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}