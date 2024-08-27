#include <string>
#include <iostream>

int main() {
    std::string src = "Hello world";
    char *current = &src[0];

    while(*current != '\0') {
        std::cout << *current;
        current++;
    }

    return 0;
}
