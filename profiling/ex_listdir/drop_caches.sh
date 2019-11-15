#!/bin/sh
echo "dropping all caches"
echo 3 | sudo tee /proc/sys/vm/drop_caches
