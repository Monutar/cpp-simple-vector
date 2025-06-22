#pragma once

#include <iostream>
#include <algorithm>
#include <optional>
#include <utility>
#include <new>
#include <iterator>

using namespace std;

template <typename Type>
class ArrayPtr {
public:

    // Инициализирует ArrayPtr нулевым указателем
    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        if (size == 0) {
            raw_ptr_ = nullptr;
            return;
        }
        raw_ptr_ = new Type[size];
    }

    // Конструктор из сырого указателя
    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    // Запрещаем копирование
    ArrayPtr(const ArrayPtr&) = delete;

    // Запрещаем присваивание
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&& other) {
        raw_ptr_ = exchange(other.Get(), nullptr);
    }

    ArrayPtr& operator=(ArrayPtr&& other) {
        if (this != &other) {
            raw_ptr_ = exchange(other.raw_ptr_, nullptr);
        }
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    [[nodiscard]] Type* Release() noexcept {
        return exchange(raw_ptr_, nullptr);
    }

    Type& operator[](size_t index) noexcept {
        return *(raw_ptr_ + index);
    }

    const Type& operator[](size_t index) const noexcept {
        return *(const_cast<Type*>(raw_ptr_ + index));
    }

    explicit operator bool() const {
        return raw_ptr_;
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};


class ReserveProxyObj {
public:

    ReserveProxyObj(size_t reserve) : reserve_(reserve) {}

    size_t GetReserve() {
        return reserve_;
    }

private:
    size_t reserve_ = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    ReserveProxyObj new_proxy(capacity_to_reserve);
    return new_proxy;
}