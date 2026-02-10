#include "../../misc/TEST.hpp"
#include "../../misc/ConsoleLogger.hpp"

#ifdef TEST
#include "test_LLM.hpp"
#include "test_Script.hpp"
#endif // TEST

int main() {
    createLogger<ConsoleLogger>();
    tester.run();
    return 0;
}