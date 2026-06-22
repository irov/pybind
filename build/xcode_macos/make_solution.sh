#!/usr/bin/env bash
set -euo pipefail

CONFIGURATION="${1:-Debug}"

PROJECT="pybind"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
SOLUTION_DIR="${ROOT_DIR}/solutions/${PROJECT}_xcode_macos_${CONFIGURATION}"

mkdir -p "${SOLUTION_DIR}"

cmake -S "${ROOT_DIR}" -B "${SOLUTION_DIR}" -G Xcode \
    -DCMAKE_CONFIGURATION_TYPES:STRING="${CONFIGURATION}" \
    -DCMAKE_BUILD_TYPE:STRING="${CONFIGURATION}" \
    -DPYBIND_PYTHON_VERSION:INTEGER=278 \
    -DPYBIND_PROJECT_NAME:STRING="${PROJECT}"
