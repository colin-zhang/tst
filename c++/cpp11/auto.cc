//http://www.learncpp.com/cpp-tutorial/4-8-the-auto-keyword/

auto add(int x, int y) -> int
{
    return x + y;
}
 
int main()
{
    auto sum = add(5, 6); // add() returns an int, so sum will be type int
    return 0;
}
