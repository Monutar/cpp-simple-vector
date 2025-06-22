#include <cassert>
#include <iostream>
#include <istream>
#include <string>
#include <algorithm>
#include <optional>
#include <numeric>
#include <vector>
#include <format>
#include <cstdlib>
#include <chrono>
#include <string_view>
#include <random>
#include <fstream>
#include <thread>
#include <cstddef>
#include <regex>
#include <filesystem>
#include <stack>
#include <map>
#include <set>
#include <utility>
#include <queue>
#include <sstream>
#include <cmath>
#include <new>
#include <iterator>
#include <deque>

using namespace std;
using namespace chrono;
using namespace std::literals;
using namespace literals;
using filesystem::path;

template <typename Type>
class ArrayPtr {
public:
    // Инициализирует ArrayPtr нулевым указателем
    ArrayPtr() = default;

    // Создаёт в куче массив из size элементов типа Type.
    // Если size == 0, поле raw_ptr_ должно быть равно nullptr
    explicit ArrayPtr(size_t size) {
        // Реализуйте конструктор самостоятельно
        if (size == 0) {
            raw_ptr_ = nullptr;
            return;
        }
        raw_ptr_ = new Type[size];
    }

    // Конструктор из сырого указателя, хранящего адрес массива в куче либо nullptr
    explicit ArrayPtr(Type* raw_ptr) noexcept {
        // Реализуйте конструктор самостоятельно
        raw_ptr_ = raw_ptr;
    }

    // Запрещаем копирование
    ArrayPtr(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&& other) {
        raw_ptr_ = exchange(other.Get(), nullptr);
    }

    ~ArrayPtr() {
        // Напишите деструктор самостоятельно
        delete[] raw_ptr_;
    }

    // Запрещаем присваивание
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    // Прекращает владением массивом в памяти, возвращает значение адреса массива
    // После вызова метода указатель на массив должен обнулиться
    [[nodiscard]] Type* Release() noexcept {
        // Заглушка. Реализуйте метод самостоятельно
        Type* buff_ptr = raw_ptr_;
        raw_ptr_ = nullptr;
        return buff_ptr;
    }

    // Возвращает ссылку на элемент массива с индексом index
    Type& operator[](size_t index) noexcept {
        // Реализуйте операцию самостоятельно
        return *(raw_ptr_ + index);
    }

    // Возвращает константную ссылку на элемент массива с индексом index
    const Type& operator[](size_t index) const noexcept {
        // Реализуйте операцию самостоятельно
        return *(const_cast<Type*>(raw_ptr_ + index));
    }

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const {
        // Заглушка. Реализуйте операцию самостоятельно
        return raw_ptr_;
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept {
        // Заглушка. Реализуйте метод самостоятельно
        //Type* const const_raw_ptr = raw_ptr_;
        return raw_ptr_;
    }

    // Обменивается значениям указателя на массив с объектом other
    void swap(ArrayPtr& other) noexcept {
        // Реализуйте метод самостоятельно
        Type* raw_ptr_buff = raw_ptr_;
        raw_ptr_ = other.raw_ptr_;
        other.raw_ptr_ = raw_ptr_buff;
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