globals:
	g++ -g -Wall -Wextra -o bin/globals.o -c src/data/globals.cpp
rule: globals utils
	g++ -g -Wall -Wextra -o bin/rule.o -c src/data/rule.cpp
utils:
	g++ -g -Wall -Wextra -o bin/utils.o -c src/data/utils.cpp
parse: rule utils globals
	g++ -g -Wall -Wextra -o bin/parse.o -c src/parse.cpp 
tree: rule
	g++ -g -Wall -Wextra -o bin/tree.o -c src/data/tree.cpp
threadQueue: rule tree
	g++ -g -Wall -Wextra -o bin/threadQueue.o -c src/data/threadQueue.cpp -pthread
logic: rule tree threadQueue
	g++ -g -Wall -Wextra -o bin/logic.o -c src/logic.cpp
catch:
	g++ -o bin/catch.o -c tests/catch_main.cpp

utils_test: utils catch
	g++ -g -Wall -Wextra -o bin/utils_test.o -c tests/utils_tests.cpp
parse_test: parse catch rule
	g++ -g -Wall -Wextra -o bin/parse_test.o -c tests/parse_tests.cpp
logic_test: logic tree rule parse catch
	g++ -g -Wall -Wextra -o bin/logic_test.o -c tests/logic_tests.cpp

utils_debug: utils_test utils catch
	g++ bin/catch.o bin/utils_test.o bin/utils.o -o bin/utils_debug
parse_debug: parse catch parse_test rule globals
	g++ bin/catch.o bin/parse.o bin/parse_test.o bin/rule.o bin/utils.o bin/globals.o -o bin/parse_debug 
logic_debug: logic_test catch rule logic tree utils
	g++ bin/threadQueue.o bin/catch.o bin/parse.o bin/globals.o bin/rule.o bin/logic.o bin/tree.o bin/utils.o bin/logic_test.o -o bin/logic_debug

globals_thread:
	g++ -g -Wall -Wextra -o bin/globals_thread.o -c src/data/globals.cpp -pthread
rule_thread: globals_thread utils_thread
	g++ -g -Wall -Wextra -o bin/rule_thread.o -c src/data/rule.cpp -pthread
utils_thread:
	g++ -g -Wall -Wextra -o bin/utils_thread.o -c src/data/utils.cpp -pthread
parse_thread: rule_thread utils_thread globals_thread
	g++ -g -Wall -Wextra -o bin/parse_thread.o -c src/parse.cpp -pthread
tree_thread: rule_thread
	g++ -g -Wall -Wextra -o bin/tree_thread.o -c src/data/tree.cpp -pthread
threadQueue_thread: rule_thread tree_thread
	g++ -g -Wall -Wextra -o bin/threadQueue_thread.o -c src/data/threadQueue.cpp -pthread
logic_thread: rule_thread tree_thread threadQueue_thread
	g++ -g -Wall -Wextra -o bin/logic_thread.o -c src/logic.cpp -pthread
catch_thread:
	g++ -o bin/catch_thread.o -c tests/catch_main.cpp -pthread
thread_tests: catch_thread logic_thread tree_thread rule_thread parse_thread threadQueue_thread
	g++ -g -Wall -Wextra -o bin/thread_tests.o -c tests/thread_tests.cpp -pthread
thread_debug: catch_thread logic_thread tree_thread rule_thread parse_thread threadQueue_thread thread_tests
	g++ bin/thread_tests.o bin/catch_thread.o bin/logic_thread.o bin/threadQueue_thread.o bin/tree_thread.o bin/parse_thread.o bin/utils_thread.o bin/rule_thread.o bin/globals_thread.o -o bin/thread_debug -pthread
main_thread: logic_thread tree_thread rule_thread parse_thread threadQueue_thread
	g++ -g -Wall -Wextra -o bin/main_thread.o -c production/main.cpp -pthread
main_debug: logic_thread tree_thread rule_thread parse_thread threadQueue_thread main_thread
	g++ bin/logic_thread.o bin/threadQueue_thread.o bin/tree_thread.o bin/parse_thread.o bin/utils_thread.o bin/rule_thread.o bin/globals_thread.o bin/main_thread.o -o bin/main_debug -pthread

production_globals:
	g++ -O2 -o bin/production_globals.o -c src/data/globals.cpp -pthread
production_rule: production_globals production_utils
	g++ -O2 -o bin/production_rule.o -c src/data/rule.cpp -pthread
production_utils:
	g++ -O2 -o bin/production_utils.o -c src/data/utils.cpp -pthread
production_threadQueue: production_rule production_tree
	g++ -O2 -o bin/production_threadQueue.o -c src/data/threadQueue.cpp -pthread
production_parse: production_rule production_utils production_globals
	g++ -O2 -o bin/production_parse.o -c src/parse.cpp -pthread
production_tree: production_rule
	g++ -O2 -o bin/production_tree.o -c src/data/tree.cpp -pthread
production_logic: production_rule production_tree production_threadQueue
	g++ -O2 -o bin/production_logic.o -c src/logic.cpp -pthread
main: production_parse production_rule production_logic production_threadQueue
	g++ -O2 -o bin/main.o -c production/main.cpp -pthread

production: production_rule production_utils production_globals main production_logic production_tree
	g++ bin/production_threadQueue.o bin/production_tree.o bin/production_logic.o bin/production_globals.o bin/production_rule.o bin/production_utils.o bin/production_parse.o bin/main.o -o bin/RiLab -pthread

clean:
	rm bin/*

