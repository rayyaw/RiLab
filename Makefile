globals:
	g++ -g -o bin/globals.o -c src/globals.cpp
rule: globals utils
	g++ -g -o bin/rule.o -c src/rule.cpp
utils:
	g++ -g -o bin/utils.o -c src/utils.cpp
parse: rule utils globals
	g++ -g -o bin/parse.o -c src/parse.cpp 
catch:
	g++ -o bin/catch.o -c tests/catch_main.cpp

utils_test: utils catch
	g++ -g -o bin/utils_test.o -c tests/utils_tests.cpp
parse_test: parse catch rule
	g++ -g -o bin/parse_test.o -c tests/parse_tests.cpp

utils_debug: utils_test utils catch
	g++ bin/catch.o bin/utils_test.o bin/utils.o -o bin/utils_debug
parse_debug: parse catch parse_test rule globals
	g++ bin/catch.o bin/parse.o bin/parse_test.o bin/rule.o bin/utils.o bin/globals.o -o bin/parse_debug 

production_globals:
	g++ -O2 -o bin/production_globals.o -c src/globals.cpp -pthread
production_rule: production_globals production_utils
	g++ -O2 -o bin/production_rule.o -c src/rule.cpp -pthread
production_utils:
	g++ -O2 -o bin/production_utils.o -c src/utils.cpp -pthread
production_parse: production_rule production_utils production_globals
	g++ -O2 -o bin/production_parse.o -c src/parse.cpp -pthread
main: production_parse production_rule
	g++ -O2 -o bin/main.o -c production/main.cpp -pthread

production: production_rule production_utils production_globals main
	g++ bin/production_globals.o bin/production_rule.o bin/production_utils.o bin/production_parse.o bin/main.o -o bin/RiLab -pthread

clean:
	rm bin/*

# FIXME - main, IO, rule truth determiner (+ tests)

