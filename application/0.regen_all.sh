#!/usr/bin/env bash

set -o errexit

./1.generate_protos_cpp.sh
./2.translations.sh
./3.compile_all_components.sh --clean
./4.run_all_tests.sh
./5.build_installation_package.sh
