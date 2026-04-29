#!/bin/bash

set -e

echo "Building C++ engine..."
cmake -S backend -B backend/build
cmake --build backend/build

echo "Building C# renderer..."
dotnet build Frontend

echo "Done."
