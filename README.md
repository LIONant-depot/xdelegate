# **xdelegate**

A lightweight, flexible delegate system for C++20 with no external dependencies. Ideal for event handling, callbacks, and observer patterns in your projects.

## **Features**
- **No Dependencies**: Pure C++20, no third-party libraries needed.
- **Type-Safe**: Template-driven for reliable callback signatures.
- **Variadic Support**: Handles any number of arguments.
- **Fast**: Minimal overhead with raw pointers.
- **Thread Options**: Use `thread_unsafe` for speed or `thread_safe` for concurrency.

## **Quick Example**
```cpp
#include "xdelegate.hpp"
#include <iostream>

class MyClass {
public:
    void OnEvent(int x) { std::cout << "Got: " << x << "\n"; }
};

int main() {
    xdelegate::thread_unsafe<int> delegate;
    MyClass obj;
    delegate.Register<&MyClass::OnEvent>(obj);
    delegate.NotifyAll(42);  // Prints: Got: 42
    return 0;
}
```

## **Documentation**
Dive into detailed guides and examples in the [full documentation](documentation/xdelegate.md).

---

**xdelegate**: Simple, powerful, and ready for your C++20 toolkit!