#!/bin/bash

cd "$( dirname "${BASH_SOURCE[0]}" )"

scons && ./test-x11
