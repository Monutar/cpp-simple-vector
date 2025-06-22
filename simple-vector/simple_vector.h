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

    SimpleVector(const SimpleVector& other) {
        // Напишите тело конструктора самостоятельно
        size_ = other.GetSize();
        capacity_ = other.GetCapacity();
        ArrayPtr<Type> massiv_ptr(size_);
        copy(other.begin(), other.end(), massiv_ptr.Get());
        massiv_ptr_.swap(massiv_ptr);
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        // Напишите тело конструктора самостоятельно
        if (this != &rhs) {
            Resize(rhs.GetSize());
            SimpleVector rhs_copy(rhs);
            massiv_ptr_.swap(rhs_copy.massiv_ptr_);
        }

        return *this;
    }

    /*SimpleVector(const SimpleVector&) = delete;
    SimpleVector& operator=(const SimpleVector&) = delete;*/

    SimpleVector(ReserveProxyObj other) {
        Reserve(other.GetReserve());
    }

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) : size_(size), capacity_(size) {
        // Напишите тело конструктора самостоятельно
        ArrayPtr<Type> massiv_ptr(size);
        massiv_ptr_.swap(massiv_ptr);
        fill(massiv_ptr_.Get(), massiv_ptr_.Get() + size, Type{});
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) : size_(size), capacity_(size) {
        // Напишите тело конструктора самостоятельно
        ArrayPtr<Type> massiv_ptr(size);
        massiv_ptr_.swap(massiv_ptr);
        fill(massiv_ptr_.Get(), massiv_ptr_.Get() + size_, value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) : size_(init.size()), capacity_(init.size()) {
        // Напишите тело конструктора самостоятельно
        ArrayPtr<Type> massiv_ptr(init.size());
        massiv_ptr_.swap(massiv_ptr);
        copy(init.begin(), init.end(), massiv_ptr_.Get());
    }

    SimpleVector(SimpleVector&& other) {
        size_ = exchange(other.size_, 0);
        capacity_ = exchange(other.capacity_, 0);
        massiv_ptr_.swap(other.massiv_ptr_);
    }

    SimpleVector operator=(SimpleVector&& other) {
        size_ = exchange(other.size_, 0);
        capacity_ = exchange(other.capacity_, 0);
        massiv_ptr_.swap(other.massiv_ptr_);
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

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(Type&& item) {
        // Напишите тело самостоятельно
        Resize(size_ + 1);
        massiv_ptr_[size_ - 1] = exchange(item, Type{});
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, Type&& value) {
        // Напишите тело самостоятельно
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

            *it_new = exchange(value, Type{});
            ++size_;

            return it_new;

        }
        else {

            ++size_;
            move_backward(it, end() - 1, end());
            *it = exchange(value, Type{});

            return it;
        }
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        // Напишите тело самостоятельно
        if (size_ == 0) {
            return;
        }
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        // Напишите тело самостоятельно
        Iterator it = begin() + distance(cbegin(), pos);
        Iterator it1 = it + 1;

        int d = distance(begin(), begin() + distance(cbegin(), pos));

        for (auto i = begin(); i != end(); ++i) {
            cout << i << endl;
        }

        move(it + 1, end() - 1, it);
        --size_;

        for (auto i = begin(); i != end(); ++i) {
            cout << i << endl;
        }

        return it;

    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        // Напишите тело самостоятельно
        auto buff(other);
        other.massiv_ptr_.swap(massiv_ptr_);
        other.size_ = size_;
        other.capacity_ = capacity_;
        size_ = buff.size_;
        capacity_ = buff.capacity_;
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        // Напишите тело самостоятельно
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        // Напишите тело самостоятельно
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return !size_;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        // Напишите тело самостоятельно
        return massiv_ptr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        // Напишите тело самостоятельно
        return massiv_ptr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        // Напишите тело самостоятельно
        if (index >= size_) {
            throw out_of_range("");
        }
        return massiv_ptr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        // Напишите тело самостоятельно
        if (index >= size_) {
            throw out_of_range("");
        }
        return massiv_ptr_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        // Напишите тело самостоятельно
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        // Напишите тело самостоятельно
        auto type = Type{};

        if (new_size == size_) {
            return;
        }
        else if (new_size < size_) {
            size_ = new_size;
        }
        else {
            if (new_size < capacity_) {

                for (auto i = massiv_ptr_.Get() + size_; i != massiv_ptr_.Get() + new_size; ++i) {
                    *i = exchange(type, Type{});
                }

                //move(massiv_ptr_.Get() + size_, massiv_ptr_.Get() + new_size, move(Type{}));
                size_ = new_size;

            }
            else {

                capacity_ = max(new_size, capacity_ * 2);
                ArrayPtr<Type> new_massive(capacity_);

                move(massiv_ptr_.Get(), massiv_ptr_.Get() + size_, new_massive.Get());

                for (auto i = new_massive.Get() + size_; i != new_massive.Get() + new_size; ++i) {
                    *i = exchange(type, Type{});
                }

                massiv_ptr_.swap(new_massive);
                size_ = new_size;

            }
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        // Напишите тело самостоятельно
        return massiv_ptr_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        // Напишите тело самостоятельно
        return massiv_ptr_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        // Напишите тело самостоятельно
        ConstIterator begin_ = massiv_ptr_.Get();
        return begin_;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        // Напишите тело самостоятельно
        ConstIterator end_ = massiv_ptr_.Get() + size_;
        return end_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        // Напишите тело самостоятельно
        return begin();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        // Напишите тело самостоятельно
        return end();
    }

private:
    ArrayPtr<Type> massiv_ptr_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return !(lhs < rhs);
}