# **xdelegate**

A lightweight, flexible delegate system for C++20 with no external dependencies. Ideal for event handling, callbacks, and observer patterns in your projects.

Delegates and events represent a well-established design pattern that enables multiple classes to communicate seamlessly without needing to know each
other's types. This approach reduces dependencies among the classes, fostering a framework that is both user-friendly and easy to debug.

## **Features**
- **No Dependencies**: Pure C++20, no third-party libraries needed.
- **Type-Safe**: Template-driven for reliable callback signatures.
- **Variadic Support**: Handles any number of arguments.
- **Fast**: Minimal overhead with raw pointers.
- **Thread Options**: Use `thread_unsafe` for speed or `thread_safe` for concurrency.
- **MIT License**: As open source as it gets...

## **Quick Example**
```cpp
#include "xdelegate.h"
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
Dive into detailed guides and examples in the [full documentation](https://github.com/LIONant-depot/xdelegate/blob/main/documentation/documentation.md).

**xdelegate**: Simple, powerful, and ready for your C++20 toolkit!

---
