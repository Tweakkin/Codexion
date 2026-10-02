#!/bin/bash

echo "=== TEST 1: The 1-Coder Edge Case (Must die at ~800ms) ==="
./codexion 1 800 200 100 50 3 100 edf

echo -e "\n=== TEST 2: 5 Coders, Standard Survival (0 cooldown to match standard Philosophers) ==="
./codexion 5 800 200 100 50 5 0 edf
echo "Result: Exit Code $?"

echo -e "\n=== TEST 3: 4 Coders, Tight Survival (410 burnout, 200 compile) ==="
./codexion 4 410 200 100 50 5 0 edf
echo "Result: Exit Code $?"

echo -e "\n=== TEST 4: 4 Coders, Inevitable Death (310 burnout, 200 compile) ==="
./codexion 4 310 200 100 50 5 0 edf

echo -e "\n=== TEST 5: Error Handling ==="
./codexion -5 800 200 100 50 3 100 edf
