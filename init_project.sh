#!/usr/bin/env bash
set -e

# Idempotent, builds the lib structure in case of future revision
PROJECT_NAME="htx"

echo "INFO: Scaffolding project structure for '${PROJECT_NAME}'..."

# 1. Top-Level Directories
echo "INFO: Creating all project directories..."
mkdir -p \
  cmake \
  "include/${PROJECT_NAME}" \
  src \
  transport/h2 \
  transport/h3 \
  tools \
  tests/unit \
  tests/integration \
  tests/data/vectors \
  fuzz \
  third_party/noise-c \
  third_party/nghttp2 \
  third_party/openssl \
  docs/spec \
  docs/dev \
  packaging/debian \
  packaging/brew/Formula \
  scripts/ci \
  .github/workflows

# 2. Top-Level Files (Build System, Config, License)
# Create essential project-wide files.
echo "INFO: Creating top-level configuration files..."
touch \
  CMakeLists.txt \
  .clang-format \
  .clang-tidy \
  .editorconfig \
  .gitignore \
  LICENSE \
  README.md

# 3. CMake Helper Directory
echo "INFO: Creating CMake helper files..."
touch \
  cmake/Toolchain-clang.cmake \
  cmake/Sanitizers.cmake \
  cmake/PkgConfig.cmake

# 4. Public Headers (The Library's API)
echo "INFO: Creating public header files in 'include/${PROJECT_NAME}/'..."
touch \
  "include/${PROJECT_NAME}/htx.h" \
  "include/${PROJECT_NAME}/frame.h" \
  "include/${PROJECT_NAME}/varint.h" \
  "include/${PROJECT_NAME}/u24.h" \
  "include/${PROJECT_NAME}/aead.h" \
  "include/${PROJECT_NAME}/nonce.h" \
  "include/${PROJECT_NAME}/noise_bind.h" \
  "include/${PROJECT_NAME}/errors.h"

# 5. Source Files (The Library's Implementation)
echo "INFO: Creating source files in 'src/'..."
touch \
  src/htx.c \
  src/frame.c \
  src/varint.c \
  src/u24.c \
  src/aead_openssl.c \
  src/nonce.c \
  src/noise_bind.c

# 6. Transport Layer Demos
echo "INFO: Creating transport demo files..."
touch \
  transport/h2/h2_server.c \
  transport/h2/h2_client.c

# 7. Tooling
echo "INFO: Creating tool source files..."
touch \
  tools/htx-dump.c \
  tools/htx-gen.c \
  tools/htx-bench.c

# 8. Test Suite
echo "INFO: Creating test source and data files..."
touch \
  tests/unit/test_varint.c \
  tests/unit/test_u24.c \
  tests/unit/test_frame_roundtrip.c \
  tests/unit/test_tamper_header.c \
  tests/unit/test_nonce.c \
  tests/integration/test_h2_echo.c \
  tests/integration/test_noise_kdf.c \
  tests/data/vectors/vector_01.json

# 9. Fuzzing Harnesses
echo "INFO: Creating fuzzing target files..."
touch \
  fuzz/fuzz_varint.cc \
  fuzz/fuzz_frame_header.cc \
  fuzz/fuzz_frame_roundtrip.cc

# 10. Third Party Placeholders
# We create .gitkeep files to ensure the directories are committed even if empty,
# signaling that this is where submodules or vendored code should go.
echo "INFO: Creating third_party placeholders..."
touch \
  third_party/noise-c/.gitkeep \
  third_party/nghttp2/.gitkeep \
  third_party/openssl/.gitkeep

# 11. Documentation
echo "INFO: Creating documentation files..."
touch \
  docs/spec/htx-framing-v0.1.md \
  docs/dev/CONTRIBUTING.md \
  docs/dev/BUILD.md \
  docs/dev/REPRODUCIBLE.md \
  docs/SECURITY.md \
  docs/CODE_OF_CONDUCT.md

# 12. Packaging Files
echo "INFO: Creating packaging files..."
touch \
  "packaging/${PROJECT_NAME}.pc" \
  "packaging/brew/Formula/${PROJECT_NAME}.rb"

# 13. Helper Scripts
echo "INFO: Creating utility scripts..."
touch \
  scripts/dev-setup.sh \
  scripts/run-tests.sh \
  scripts/gen-sbom.sh \
  scripts/sign-artifacts.sh \
  scripts/ci/gha-env.sh

# 14. GitHub Actions (CI/CD)
echo "INFO: Creating GitHub Actions workflow files..."
touch \
  .github/workflows/ci.yml \
  .github/workflows/release.yml \
  .github/workflows/codeql.yml


echo ""
echo "SUCCESS: Project structure for '${PROJECT_NAME}' created."
echo "Next steps:"
echo "  1. Fill in the contents of the placeholder files (especially CMakeLists.txt and the src/ files)."
echo "  2. Initialize a git repository with 'git init'."
echo "  3. Add dependencies to 'third_party/' (e.g., as git submodules)."
echo "  4. Start coding!"

