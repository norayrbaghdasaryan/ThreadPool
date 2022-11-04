CXX=clang++
CPPFLAGS="-std=c++17"

GTEST_DIR=build/googletest-release-1.12.0/googletest

# Where to find user code.
USER_DIR=.

# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread

# All tests produced by this Makefile.
# created to the list.
TESTS = build/test_thread_pool

# All Google Test headers.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

THREAD_POOL_SOURCES = $(USER_DIR)/pthread_mutex_lock.cpp \
 					  $(USER_DIR)/thread_pool.cpp

# Install gtest sources
gtest-install:
	if [ ! -d $(GTEST_DIR) ]; then \
		mkdir build 2> /dev/null || true; \
		wget https://github.com/google/googletest/archive/refs/tags/release-1.12.0.zip -O build/gtest.zip; \
		cd build && (echo "A" |  unzip gtest.zip) && rm gtest.zip; \
    fi

# Builds gtest.a and gtest_main.a.

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
build/gtest-all.o : gtest-install
	if [ ! -f "build/gtest-all.o" ]; then \
		$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
				$(GTEST_DIR)/src/gtest-all.cc -o build/gtest-all.o; \
	fi

build/gtest_main.o : gtest-install
	if [ ! -f "build/gtest_main.o" ]; then \
		$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
				$(GTEST_DIR)/src/gtest_main.cc -o build/gtest_main.o; \
	fi

build/gtest.a : build/gtest-all.o
	if [ ! -f "build/gtest.a" ]; then \
		$(AR) $(ARFLAGS) $@ $^; \
	fi

build/gtest_main.a : build/gtest-all.o build/gtest_main.o
	if [ ! -f "build/gtest_main.a" ]; then \
		$(AR) $(ARFLAGS) $@ $^; \
	fi


# ================================== End of gtest related actions ==================================


build/thread_pool.so: $(THREAD_POOL_SOURCES)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -shared -fPIC $^ -o build/thread_pool.so

build/test_thread_pool.o : gtest-install $(USER_DIR)/test_thread_pool.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/test_thread_pool.cpp -o build/test_thread_pool.o

build/test_thread_pool: build/gtest_main.a build/test_thread_pool.o build/thread_pool.so
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@



all : $(TESTS)
	$^

clean :
	rm -rf build

