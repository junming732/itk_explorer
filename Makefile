# ============================================================================
# Developer Utilities Makefile (not part of CMake)
# Run these directly: `make format`, `make tidy`, `make check`
# ============================================================================

# Tools
CLANG_FORMAT ?= clang-format
CLANG_TIDY ?= clang-tidy

# Source directories
SRC_DIRS := src include
CPP_FILES := $(shell find $(SRC_DIRS) -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' \))

# Default target
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  make format     - Format all C++ source files with clang-format"
	@echo "  make tidy       - Run clang-tidy on all C++ source files"
	@echo "  make check      - Run clang-format in check mode (no changes)"
	@echo "  make setup-hook - Install pre-commit hook"

# ----------------------------------------------------------------------------
# Code Formatting
# ----------------------------------------------------------------------------

.PHONY: format
format:
	@echo "🧹 Formatting source code..."
	@$(CLANG_FORMAT) -i --style=file $(CPP_FILES)

.PHONY: check
check:
	@echo "🔍 Checking formatting..."
	@$(CLANG_FORMAT) --dry-run --Werror --style=file $(CPP_FILES)

# ----------------------------------------------------------------------------
# Static Analysis
# ----------------------------------------------------------------------------

.PHONY: tidy
tidy:
	@echo "🔎 Running clang-tidy..."
	@$(CLANG_TIDY) --config-file=.clang-tidy $(CPP_FILES) || true

# ----------------------------------------------------------------------------
# Git Hook Installation
# ----------------------------------------------------------------------------

.PHONY: setup-hook
setup-hook:
	@echo "⚙️  Setting up git pre-commit hook..."
	@mkdir -p .githooks
	@cp scripts/pre-commit .githooks/pre-commit
	@chmod +x .githooks/pre-commit
	@git config core.hooksPath .githooks
	@echo "✅ Git pre-commit hook installed."
