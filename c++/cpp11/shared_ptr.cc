#include <iostream>
#include <memory>
using  namespace std;

// #include <iostream>
// #include <memory>
// int main () {
//   std::shared_ptr<int> foo = std::make_shared<int> (10);
//   // same as:
//   std::shared_ptr<int> foo2 (new int(10));
//   auto bar = std::make_shared<int> (20);
//   auto baz = std::make_shared<std::pair<int,int>> (30,40);
//   std::cout << "*foo: " << *foo << '\n';
//   std::cout << "*bar: " << *bar << '\n';
//   std::cout << "*baz: " << baz->first << ' ' << baz->second << '\n';
//   return 0;
// }

class Example
{
public:
    Example() : e(1) { 
        cout << "Example Constructor..." << endl; 
    }
    ~Example() { 
        cout << "Example Destructor..." << endl; 
    }

    int e;
};

int main() {

    shared_ptr<Example> pInt(new Example());
    cout << (*pInt).e << endl;
    cout << "pInt引用计数: " << pInt.use_count() << endl;

    shared_ptr<Example> pInt2 = pInt;
    cout << "pInt引用计数: " << pInt.use_count() << endl;
    cout << "pInt2引用计数: " << pInt2.use_count() << endl;
}
