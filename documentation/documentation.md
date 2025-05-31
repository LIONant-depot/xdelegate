# **xdelegate: A Simple and Flexible Delegate System in C++**

The `xdelegate` namespace provides a lightweight and flexible delegate system in C++ for managing callbacks with variadic arguments. It supports both member functions and free functions (including lambdas) and offers two implementations: a thread-unsafe version for single-threaded use and a thread-safe version for multi-threaded environments. This system is perfect for event handling, observer patterns, or scenarios where multiple callbacks need to be triggered by a single event.

## **Table of Contents**
1. [Overview](#overview)
2. [Key Features](#key-features)
3. [Class Structure](#class-structure)
4. [Usage Guide](#usage-guide)
   - [Creating a Delegate](#creating-a-delegate)
   - [Registering Callbacks](#registering-callbacks)
     - [Member Functions](#member-functions)
     - [Free Functions and Lambdas](#free-functions-and-lambdas)
   - [Invoking Callbacks](#invoking-callbacks)
   - [Removing Callbacks](#removing-callbacks)
5. [Thread Safety](#thread-safety)
6. [Examples](#examples)
   - [Basic Member Function Usage](#basic-member-function-usage)
   - [Using Lambdas](#using-lambdas)
   - [Free Function with Handle](#free-function-with-handle)
   - [Thread-Safe Delegate in Multi-Threaded Context](#thread-safe-delegate-in-multi-threaded-context)
   - [Multiple Callbacks with Mixed Types](#multiple-callbacks-with-mixed-types)
7. [Best Practices](#best-practices)
8. [Common Pitfalls](#common-pitfalls)
9. [Extended Examples](#extended-examples)
    1. [One-to-Many Communication](#one-to-many-communication)
    2. [Parent-to-Child Communication](#parent-to-child-communication)
    3. [Bidirectional Many-to-Many Communication](#bidirectional-many-to-many-communication)


## **Overview**
A delegate is a type-safe mechanism to store and invoke multiple callbacks. The `xdelegate` system allows you to:
- Register callbacks for member functions, free functions, or lambdas.
- Invoke all registered callbacks with one call.
- Remove callbacks associated with a specific instance or handle.

Two classes are provided:
- **`thread_unsafe`**: A lightweight delegate for single-threaded applications.
- **`thread_safe`**: A thread-safe delegate with mutex synchronization for multi-threaded applications.


## **Key Features**
- **Variadic Arguments**: Supports callbacks with any number and type of arguments.
- **Type Safety**: Ensures callbacks match the delegate's signature using templates.
- **Performance**: Uses raw pointers and minimal overhead for efficiency.
- **Flexible Registration**: Works with member functions, free functions, and lambdas.
- **Thread Safety Option**: Provides a thread-safe variant for concurrent access.
- **Callback Management**: Allows removal of specific callbacks.


## **Class Structure**
The `xdelegate` namespace includes two templated classes:

### **`thread_unsafe<T_ARGS...>`**
- Manages a list of callbacks in a non-thread-safe manner.
- Ideal for single-threaded applications or where synchronization is handled externally.
- Stores callbacks in a `std::vector` of `info` structs containing a function pointer and a handle/instance pointer.

### **`thread_safe<T_ARGS...>`**
- Inherits from `thread_unsafe` and adds a `std::mutex` for synchronization.
- Safe for use in multi-threaded applications where concurrent access is possible.
- Overrides key methods to lock the mutex during operations.

Both classes use variadic templates (`T_ARGS...`) to define the argument types for callbacks.


## **Usage Guide**

### **Creating a Delegate**
To create a delegate, specify the argument types that the callbacks will accept:
- `xdelegate::thread_unsafe<int, double>` for callbacks taking an `int` and a `double`.
- `xdelegate::thread_safe<std::string>` for callbacks taking a `std::string`.

### **Registering Callbacks**
Callbacks can be registered as member functions or free functions/lambdas.

#### **Member Functions**
- Use the `Register` method with the member function pointer as a template parameter.
- Pass a reference to the class instance.

**Syntax**:
```cpp
delegate.Register<&Class::MemberFunction>(instance);
```

#### **Free Functions and Lambdas**
- Use the `Register` method with the callable as a template parameter.
- Optionally provide a `void* pHandle` to identify the callback for removal.

**Syntax**:
```cpp
delegate.Register<&FreeFunction>(handle);  // Free function
delegate.Register<[](T_ARGS... args) { /* logic */ }>();  // Lambda
```

### **Invoking Callbacks**
Use the `NotifyAll` method to call all registered callbacks with the specified arguments.

**Syntax**:
```cpp
delegate.NotifyAll(arg1, arg2, ...);
```

### **Removing Callbacks**
Use the `RemoveDelegates` method with the instance pointer or handle to remove associated callbacks.

**Syntax**:
```cpp
delegate.RemoveDelegates(&instance);  // For member functions
delegate.RemoveDelegates(handle);     // For free functions/lambdas with handle
```


## **Thread Safety**
- **`thread_unsafe`**: No synchronization. Use only in single-threaded contexts or with external locking.
- **`thread_safe`**: Uses a `std::mutex` to protect the delegate list. Safe for concurrent registration, invocation, and removal. The mutex is locked during the entire `NotifyAll` call, including callback execution.


## **Examples**

### **Basic Member Function Usage**
Register and invoke a member function callback.

```cpp
#include <iostream>
#include "xdelegate.h"

class MyClass {
public:
    void OnEvent(int x) {
        std::cout << "Event received: " << x << "\n";
    }
};

int main() {
    xdelegate::thread_unsafe<int> delegate;
    MyClass obj;
    delegate.Register<&MyClass::OnEvent>(obj);
    delegate.NotifyAll(42);
    // Output: Event received: 42
    return 0;
}
```

### **Using Lambdas**
Register and invoke a lambda callback.

```cpp
#include <iostream>
#include "xdelegate.h"

int main() {
    xdelegate::thread_unsafe<int> delegate;
    delegate.Register<[](int x) {
        std::cout << "Lambda called with: " << x << "\n";
    }>();
    delegate.NotifyAll(100);
    // Output: Lambda called with: 100
    return 0;
}
```

### **Free Function with Handle**
Register a free function with a handle and remove it.

```cpp
#include <iostream>
#include "xdelegate.h"

void PrintValue(int x) {
    std::cout << "Free function: " << x << "\n";
}

int main() {
    xdelegate::thread_unsafe<int> delegate;
    delegate.Register<&PrintValue>(&PrintValue);
    delegate.NotifyAll(5);  // Output: Free function: 5
    delegate.RemoveDelegates(&PrintValue);
    delegate.NotifyAll(10); // No output (callback removed)
    return 0;
}
```

### **Free Lambdas by using any pointer as a handle to free it!**
Unlike classes lambdas don't have natural pointers so how could we free them?

```cpp
#include <iostream>
#include "xdelegate.h"

int main() {
    xdelegate::thread_unsafe<int> delegate;

    // Note we use the delegate pointer as the key which later on we can use
    // to free our registered lambda. The pointer is never access so it is 
    // perfectly safe... the only thing we use is the actual value as the key
    // We could also use things like this as a handle for the delegate
    // void* pHandle = reinterpret_cast<void*>(0x1234);
    // delegate.Register<...>(pHandle) or even delegate.Register<...>(&pHandle)
    delegate.Register<[](int x) {
        std::cout << "Lambda called with: " << x << "\n";
    }>(&delegate);

    delegate.NotifyAll(100);
    // Output: Lambda called with: 100

    // Now we can remove the lambda delegate using the same key as we used
    // when registered it
    delegate.RemoveDelegates(&delegate);

    delegate.NotifyAll(10); // No output (callback removed)
    return 0;
}
```


### **Thread-Safe Delegate in Multi-Threaded Context**
Use a thread-safe delegate with multiple threads.

```cpp
#include <iostream>
#include <thread>
#include "xdelegate.h"

class MyClass {
public:
    void OnEvent(int x) {
        std::cout << "Event: " << x << "\n";
    }
};

int main() {
    xdelegate::thread_safe<int> delegate;
    MyClass obj;
    delegate.Register<&MyClass::OnEvent>(obj);

    std::thread t1([&delegate]() { delegate.NotifyAll(1); });
    std::thread t2([&delegate]() {
        delegate.Register<[](int x) { std::cout << "Lambda: " << x << "\n"; }>();
    });

    t1.join();
    t2.join();
    delegate.NotifyAll(2);
    // Possible output:
    // Event: 1
    // Event: 2
    // Lambda: 2
    return 0;
}
```

### **Multiple Callbacks with Mixed Types**
Combine member functions, free functions, and lambdas.

```cpp
#include <iostream>
#include "xdelegate.h"

class MyClass {
public:
    void OnEvent(int a, double b) {
        std::cout << "Member: " << a << ", " << b << "\n";
    }
};

void FreeFunc(int a, double b) {
    std::cout << "Free: " << a << ", " << b << "\n";
}

int main() {
    xdelegate::thread_unsafe<int, double> delegate;
    MyClass obj;
    delegate.Register<&MyClass::OnEvent>(obj);
    delegate.Register<&FreeFunc>();
    delegate.Register<[](int a, double b) {
        std::cout << "Lambda: " << a << ", " << b << "\n";
    }>();
    delegate.NotifyAll(10, 3.14);
    // Output:
    // Member: 10, 3.14
    // Free: 10, 3.14
    // Lambda: 10, 3.14
    return 0;
}
```

## **Best Practices**
- Use **`thread_unsafe`** in single-threaded applications for better performance.
- Use **`thread_safe`** in multi-threaded applications to avoid race conditions.
- Remove callbacks when objects are destroyed to prevent undefined behavior.
- Use unique handles for free functions/lambdas if you plan to remove them later.
- Keep callbacks short in `thread_safe` to minimize mutex contention.


## **Common Pitfalls**
- **Dangling Pointers**: Failing to remove callbacks for destroyed objects can lead to crashes.
- **Thread Safety**: Using `thread_unsafe` in multi-threaded code causes data races.
- **Signature Mismatch**: Ensure callback arguments match the delegate's template parameters.
- **Blocking Callbacks**: Long-running callbacks in `thread_safe` can block other threads due to the mutex.

# **Extended Examples**

Going beyound the basics...

## **One-to-Many Communication**
One-to-Many Communication (Publisher-Subscribers). A single publisher notifies multiple subscribers using the delegate system.
<br>
**Description**: The `Publisher` uses a delegate to notify multiple `Subscriber` instances. Each subscriber registers its `OnEvent` method, and `NotifyAll` calls all registered callbacks.

```cpp
#include "xdelegate.h"
#include <iostream>

class Subscriber {
public:
    void OnEvent(int x) {
        std::cout << "Subscriber " << id << " received: " << x << "\n";
    }
    int id;
};

class Publisher {
public:
    xdelegate::thread_unsafe<int> delegate;
    void TriggerEvent(int value) {
        delegate.NotifyAll(value);
    }
};

int main() {
    Publisher pub;
    Subscriber sub1{1}, sub2{2};

    pub.delegate.Register<&Subscriber::OnEvent>(sub1);
    pub.delegate.Register<&Subscriber::OnEvent>(sub2);

    pub.TriggerEvent(42);
    // Output:
    // Subscriber 1 received: 42
    // Subscriber 2 received: 42

    return 0;
}
```

## **Parent-to-Child Communication**
A parent class notifies its children using a delegate.
<br>
**Description**: This example demonstrates how to use `xdelegate` within a C++ class hierarchy. The `Parent` class contains a delegate, and derived classes (`Child1` and `Child2`) 
register their own event handlers with this delegate. The main function iterates through a vector of `Parent` pointers and calls `NotifyAll` on each delegate, triggering the registered callbacks.

```cpp
#include "xdelegate.h"
#include <iostream>
#include <vector>

// Base class with an xdelegate
class Parent {
public:
    xdelegate::thread_unsafe<int> delegate;
};

// Child class 1 inheriting from Parent
class Child1 : public Parent {
public:
    Child1() {
        delegate.Register<&Child1::HandleEvent>(*this);
    }
    void HandleEvent(int value) {
        std::cout << "Child1 received: " << value << "\n";
    }
};

// Child class 2 inheriting from Parent
class Child2 : public Parent {
public:
    Child2() {
        delegate.Register<&Child2::HandleEvent>(*this);
    }
    void HandleEvent(int value) {
        std::cout << "Child2 received: " << value << "\n";
    }
};

int main() {
    std::vector<Parent*> parents;
    Child1 c1;
    Child2 c2;
    parents.push_back(&c1);
    parents.push_back(&c2);

    for (auto* parent : parents) {
        parent->delegate.NotifyAll(100);
    }
    return 0;
}
```

## **Bidirectional Many-to-Many Communication**
Subscribers can also send messages back to publishers using a second delegate.
<br>
**Description**: This example demonstrates a flexible, many-to-many communication system using the xdelegate library, where publishers and subscribers exchange messages
via an EventDispatcher without needing to know each other’s types. The EventDispatcher manages two delegates—one for publishers to notify subscribers 
and another for subscribers to notify publishers—enabling loose coupling. This design enhances flexibility and maintainability by allowing
components to be added or modified independently.


```cpp
#include "xdelegate.h"
#include <iostream>

class Publisher {
public:
    void OnSubscriberMessage(int x) {
        std::cout << "Publisher received from subscriber: " << x << "\n";
    }
    void TriggerEvent(int x, xdelegate::thread_unsafe<int>& delegate) {
        delegate.NotifyAll(x);
    }
};

class Subscriber {
public:
    void OnPublisherEvent(int x) {
        std::cout << "Subscriber received from publisher: " << x << "\n";
    }
    void SendMessageToPublisher(int x, xdelegate::thread_unsafe<int>& delegate) {
        delegate.NotifyAll(x);
    }
};

class EventDispatcher {
public:
    xdelegate::thread_unsafe<int> publisherToSubscriberDelegate;
    xdelegate::thread_unsafe<int> subscriberToPublisherDelegate;
};

int main() {
    EventDispatcher dispatcher;
    Publisher pub1, pub2;
    Subscriber sub1, sub2;

    // Register publishers to receive messages from subscribers
    dispatcher.subscriberToPublisherDelegate.Register<&Publisher::OnSubscriberMessage>(pub1);
    dispatcher.subscriberToPublisherDelegate.Register<&Publisher::OnSubscriberMessage>(pub2);

    // Register subscribers to receive messages from publishers
    dispatcher.publisherToSubscriberDelegate.Register<&Subscriber::OnPublisherEvent>(sub1);
    dispatcher.publisherToSubscriberDelegate.Register<&Subscriber::OnPublisherEvent>(sub2);

    // Publisher sends a message to subscribers
    pub1.TriggerEvent(10, dispatcher.publisherToSubscriberDelegate);
    // Output:
    // Subscriber received from publisher: 10
    // Subscriber received from publisher: 10

    // Subscriber sends a message to publishers
    sub1.SendMessageToPublisher(20, dispatcher.subscriberToPublisherDelegate);
    // Output:
    // Publisher received from subscriber: 20
    // Publisher received from subscriber: 20

    return 0;
}
```

---