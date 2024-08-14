// std_enable_shared_from_this.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// https://en.cppreference.com/w/cpp/memory/enable_shared_from_this

#include <iostream>
#include <memory>

class Good : public std::enable_shared_from_this<Good>
{
public:
    std::shared_ptr<Good> getptr()
    {
        // the shared_from_this() method is inherited from class std::enable_shared_from_this
        // Returns a std::shared_ptr<T> that shares ownership of *this
        return shared_from_this();
    }
};

class Best : public std::enable_shared_from_this<Best>
{
    struct Private { explicit Private() = default; };
public:
    // Constructor is only usable by this class
    Best(Private) {}

    // Everyone else has to use this factory function
    // Hence all Best objects will be constained in shared_ptr
    static std::shared_ptr<Best> create()
    {
        return std::make_shared<Best>(Private());
    }

    std::shared_ptr<Best> getptr()
    {
        // the shared_from_this() method is inherited from class std::enable_shared_from_this
        // Returns a std::shared_ptr<T> that shares ownership of *this
        return shared_from_this();
    }
};

struct Bad
{
    std::shared_ptr<Bad> getptr()
    {
        return std::shared_ptr<Bad>(this);
    }
    ~Bad() { std::cout << "Bad::~Bad() called\n"; }
};

void testGood()
{
    // Good: the two shared_pts's share the same object
    std::shared_ptr<Good> good0 = std::make_shared<Good>();
    std::shared_ptr<Good> good1 = good0->getptr();
    std::cout << "good1.use_count() = " << good1.use_count() << '\n';
}

void misuseGood()
{
    // Bad: shared_from_this is called without having std::shared_ptr owning the caller
    try
    {
        Good not_so_good;
        std::shared_ptr<Good> gp1 = not_so_good.getptr();
    }
    catch (std::bad_weak_ptr& e)
    {
        // undefined behaviour (until C++17) and std::bad_weak_ptr thrown (since)
        std::cout << e.what() << '\n';
    }
}

void testBest()
{
    // Best: Same but can't stack allocate it:
    std::shared_ptr<Best> best0 = Best::create();
    std::shared_ptr<Best> best1 = best0->getptr();
    std::cout << "best1.use_count() = " << best1.use_count() << '\n';

    // Best stackBest; // Will not compile because Best::Best() is private
}

void testBad()
{
    // Bad: each shared_ptr thinks it is the only owner of the object:
    std::shared_ptr<Bad> bad0 = std::make_shared<Bad>();
    std::shared_ptr<Bad> bad1 = bad0->getptr();
    std::cout << "bad1.use_count() = " << bad1.use_count() << '\n';
}   // UB: double-delete of Bad

int main()
{
    testGood();
    misuseGood();

    testBest();

    testBad();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
