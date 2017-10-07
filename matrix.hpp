#ifndef __MATRIX__H
#define __MATRIX__H
#include <cstring>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
using namespace std;


namespace matrix {
static int cnt = 0; // couter debug-use

template <typename T>
class Matrix {
public:
    Matrix():m(0), n(0), size(0)  {
        ++cnt;
        printf("%x ", this);
        std::cout << "default constructor \n";
    }

    Matrix(size_t mm, size_t nn): m(mm), n(nn), size(mm * nn){
        ++cnt;
        printf("%x ", this);
        std::cout << "constructor size\n";
        _data = new T[size];
    }
    
    Matrix(size_t mm, size_t nn, T e): m(mm), n(nn), size(mm * nn){
        ++cnt;
        printf("%x ", this);
        std::cout << "constructor \n";
        _data = new T[size];
        for (int i = 0; i < size; ++i) {
            _data[i] = e;
        }
    }

    Matrix(std::initializer_list<std::initializer_list<T>> list): m(list.size()), n((*list.begin()).size()), size(m * n) {
        printf("%x ", this);
        ++cnt;        
        std::cout << "constructor initializer_list\n";
        _data = new T[size];
        auto it1 = list.begin();
        for (int i = 0; i < m; ++i, ++it1) {
            auto it2 = it1->begin();
            for (int j = 0; j < n; ++j, ++it2) {
                operator()(i, j) = *it2;
            }
        }
    }

    Matrix(const Matrix<T>& other): m(other.m), n(other.n), size(other.size) {
        ++cnt;
        printf("%x ", this);
        std::cout << "constructor copy\n";
        _data = new T[size];
        memcpy(_data, other._data, size * sizeof(T));
    }

    // RVO vs move copy
    Matrix(Matrix<T>&& other): m(other.m), n(other.n), size(other.size) {
        ++cnt;
        printf("%x ", this);
        std::cout << "constructor move\n";
        _data = other._data;
        other._data = nullptr;
        *const_cast<size_t*>(&other.m) = 0;
        *const_cast<size_t*>(&other.n) = 0;
        *const_cast<size_t*>(&other.size) = 0;
    }

    Matrix<T>& operator=(const Matrix<T>& other) {
        std::cout << "operator= copy\n";
        delete _data;
        *const_cast<size_t*>(&m) = other.m;
        *const_cast<size_t*>(&n) = other.n;
        *const_cast<size_t*>(&size) = other.size;
        _data = new T[size];
        memcpy(_data, other._data, size * sizeof(T));        
        return *this;
    }

    bool operator==(Matrix<T>& other) const {
        return size == other.size && m == other.m && std::equal(_data, _data+size, other._data);
    }
    
    bool operator!=(Matrix<T>& other) const {
        return !operator==(other);
    }

    bool operator<(Matrix<T>& other) const {
        return size < other.size || (size == other.size 
            && std::lexicographical_compare(_data, _data + size, 
                other._data, other._data + size));
    }
    
    // arithmetic operator 
    Matrix<T> operator+ (const Matrix<T>& other) const { return arithmetic(other, plus<T>());}
    Matrix<T> operator- (const Matrix<T>& other) const { return arithmetic(other, minus<T>());}
    Matrix<T> operator* (const Matrix<T>& other) const { return arithmetic(other, multiplies<T>());}
    Matrix<T> operator/ (const Matrix<T>& other) const { return arithmetic(other, divides<T>());}
    // inplace arithmetic operator 
    void operator+=(const Matrix<T>& other) { arithmetic_inplace(other, plus<T>());}
    void operator-=(const Matrix<T>& other) { arithmetic_inplace(other, minus<T>());}
    void operator*=(const Matrix<T>& other) { arithmetic_inplace(other, multiplies<T>());}
    void operator/=(const Matrix<T>& other) { arithmetic_inplace(other, divides<T>());}
    // arithmetic operator 
    Matrix<T> operator+ (T other) const { return arithmetic(other, plus<T>());}
    Matrix<T> operator- (T other) const { return arithmetic(other, minus<T>());}
    Matrix<T> operator* (T other) const { return arithmetic(other, multiplies<T>());}
    Matrix<T> operator/ (T other) const { return arithmetic(other, divides<T>());}
    // inplace arithmetic operator 
    void operator+=(T other) { arithmetic_inplace(other, plus<T>());}
    void operator-=(T other) { arithmetic_inplace(other, minus<T>());}
    void operator*=(T other) { arithmetic_inplace(other, multiplies<T>());}
    void operator/=(T other) { arithmetic_inplace(other, divides<T>());}
    // => use auto to reduce code

    virtual Matrix<T> dot(const Matrix<T>& other) const {
        if (n != other.m) {
            std::cout << "Size not match\n";
            return null;
        }
        Matrix<T> res(m, other.n, 0);
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < other.n; ++j) {
                for (int k = 0; k < n; ++k) {
                    res(i, j) += operator()(i, k) * other(k, j);
                }
            }
        }
        return res;
    }

    Matrix<T> transpose() const {
        Matrix<T> res(n, m);
        // const Matrix<T> &self = *this;
        for (size_t i = 0; i < m; ++i) {
            for (size_t j = 0; j < n; ++j) {
                res(j, i) = operator()(i, j);
            }
        }
        return res;
    }

    void zeros() {
        for (int i = 0; i < size; ++i) _data[i] = 0;
    }

    void ones() {
        for (int i = 0; i < size; ++i) _data[i] = 1;
    }

    void eyes() {
        for (int i = 0; i < size; ++i) operator()(i, i) = 1;
    }

    virtual void print() const {
        std::cout << "Matrix ("<< m <<" x "<< n << ")" << std::endl;
        for (int i = 0; i < m; ++i) {
            std::cout << "[ ";
            for (int j = 0; j < n; ++j) {
                std::cout << "\t" << operator()(i, j);
            }
            std::cout << " ]" << std::endl;
        }
    }
    std::array<size_t, 2> shape() const {
        return {m, n};
        // return std::array<size_t>()
    }
    inline T operator()(size_t i, size_t j) const{
        return _data[i*n+j];
    }
    inline T& operator()(size_t i, size_t j) {
        return _data[i*n+j];
    }
    virtual ~Matrix() {
        printf("%x ", this);
        cout << "delete" << endl;
        --cnt;
        delete[] _data;
    }
    const size_t m, n, size;
    const static Matrix<T> null;

protected:
    // TODO private???
    T *_data = nullptr;

private:
    inline Matrix<T> arithmetic(const Matrix<T>& other, std::function<T(T, T)> f) const {
        if (size != other.size || m != other.m) {
            std::cout << "Size not match\n";
            return null;
        }
        Matrix<T> res(m, n);        
        for (int i = 0; i < size; ++i) res._data[i] = f(_data[i], other._data[i]);
        return res;
    }

    inline void arithmetic_inplace(const Matrix<T>& other, std::function<T(T, T)> f) {
        if (size != other.size || m != other.m) {
            std::cout << "Size not match\n";
            return null;
        }
        for (int i = 0; i < size; ++i) _data[i] = f(_data[i], other._data[i]);
    }

    inline Matrix<T> arithmetic(T other, std::function<T(T, T)> f) const {
        Matrix<T> res(m, n);        
        for (int i = 0; i < size; ++i) res._data[i] = f(_data[i], other);
        return res;
    }

    inline void arithmetic_inplace(T other, std::function<T(T, T)> f) {     
        for (int i = 0; i < size; ++i) _data[i] = f(_data[i], other);
    }

};

template <typename T>
const Matrix<T> Matrix<T>::null(0, 0);


template <typename T>
class Vector: public Matrix<T> {
public:
    using Matrix<T>::_data;
    using Matrix<T>::size;
    using Matrix<T>::null;

    Vector(): Matrix<T>(0, 1) {}

    Vector(size_t mm): Matrix<T>(mm, 1) {}
    
    Vector(size_t mm, T e): Matrix<T>(mm, 1, e) {}

    Vector(std::initializer_list<T> list): Matrix<T>(list.size(), 1) {
        auto it = list.begin();
        for (int i = 0; i < Matrix<T>::m; ++i, ++ it) {
            _data[i] = *it;
        }
    }

    virtual void print() const {
        std::cout << "Vector ("<< size <<" x 1)" << std::endl;
        std::cout << "[ ";
        for (int i = 0; i < size; ++i) {
            std::cout << "\t" << _data[i];
        }
        std::cout << " ]" << std::endl;
    }

    Vector(const Matrix<T>& other): Matrix<T>(other) {}
    Vector(const Vector<T>& other): Matrix<T>(other) {}
    
    virtual T dot(const Vector<T>& other) const {
        if (size != other.size) {
            std::cout << "Size not match\n";
            return -1;
        }
        T res(0);
        for (int i = 0; i < size; ++i) {
            res += _data[i] * other._data[i];
        }
        return res;
    }
    // Vector(Vector<T>&& other): {}

    // arithmetic operator 
    Vector<T> operator+ (const Vector<T>& other) const { return arithmetic(other, plus<T>());}
    Vector<T> operator- (const Vector<T>& other) const { return arithmetic(other, minus<T>());}
    Vector<T> operator* (const Vector<T>& other) const { return arithmetic(other, multiplies<T>());}
    Vector<T> operator/ (const Vector<T>& other) const { return arithmetic(other, divides<T>());}
    // inplace arithmetic operator 
    void operator+=(const Vector<T>& other) { arithmetic_inplace(other, plus<T>());}
    void operator-=(const Vector<T>& other) { arithmetic_inplace(other, minus<T>());}
    void operator*=(const Vector<T>& other) { arithmetic_inplace(other, multiplies<T>());}
    void operator/=(const Vector<T>& other) { arithmetic_inplace(other, divides<T>());}
    // arithmetic operator 
    Vector<T> operator+ (T other) const { return arithmetic(other, plus<T>());}
    Vector<T> operator- (T other) const { return arithmetic(other, minus<T>());}
    Vector<T> operator* (T other) const { return arithmetic(other, multiplies<T>());}
    Vector<T> operator/ (T other) const { return arithmetic(other, divides<T>());}
    // inplace arithmetic operator 
    void operator+=(T other) { arithmetic_inplace(other, plus<T>());}
    void operator-=(T other) { arithmetic_inplace(other, minus<T>());}
    void operator*=(T other) { arithmetic_inplace(other, multiplies<T>());}
    void operator/=(T other) { arithmetic_inplace(other, divides<T>());}
    // => use auto to reduce code

    virtual ~Vector() = default;

private:
    inline Vector<T> arithmetic(const Vector<T>& other, std::function<T(T, T)> f) const {
        if (size != other.size) {
            std::cout << "Size not match\n";
            return null;
        }
        Vector<T> res(size);      
        for (int i = 0; i < size; ++i) res._data[i] = f(_data[i], other._data[i]);
        return res;
    }

    inline Vector<T> arithmetic(T other, std::function<T(T, T)> f) const {
        if (size != other.size) {
            std::cout << "Size not match\n";
            return null;
        }
        Vector<T> res(size);
        for (int i = 0; i < size; ++i) res._data[i] = f(_data[i], other);
        return res;
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m) {
    m.print();
    return os;
}

} //end of namespace matrix

#include <cstdio>


#endif