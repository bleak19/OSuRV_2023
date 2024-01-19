#!/bin/bash

exit 0


./waf configure


./waf build

# Terminal 1
# WARNING: Must be started before test_send
./build/test_receive

# Terminal 2
./build/test_send
