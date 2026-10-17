CXX      ?= g++
CLAPINC  ?= clap/include
CXXFLAGS ?= -std=c++17 -O3 -fPIC -fvisibility=hidden -Wall -Wextra -Wno-unused-parameter -I$(CLAPINC) -Isrc
NAMES    := Sub Wobble Reese Growl Screech Kick Snare Hat
PLUGS    := $(foreach i,0 1 2 3 4 5 6 7,build/DS-$(word $(shell expr $(i) + 1),$(NAMES)).clap)

all: $(PLUGS) build/DubstepKit.clap

build/DS-%.clap: src/ds_plugin.cpp src/ds_engine.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -shared -DDS_ONLY_TYPE=$(shell echo "$(NAMES)" | tr ' ' '\n' | grep -nx "$*" | cut -d: -f1 | xargs -I{} expr {} - 1) $< -o $@

build/DubstepKit.clap: src/ds_plugin.cpp src/ds_engine.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -shared $< -o $@

test: all
	$(CXX) -std=c++17 -O2 -I$(CLAPINC) tests/harness.cpp -o build/harness -ldl
	@mkdir -p demo
	./build/harness build/DubstepKit.clap demo

install: all
	mkdir -p $(HOME)/.clap
	cp build/DS-*.clap $(HOME)/.clap/

clean:
	rm -rf build
.PHONY: all test install clean
