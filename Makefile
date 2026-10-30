
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -pedantic -I include

SOURCES = src/lexer.cpp src/parser.cpp src/semantic.cpp src/tac_generator.cpp src/optimizer.cpp src/codegen.cpp src/main.cpp

LIB_SOURCES = src/lexer.cpp src/parser.cpp src/semantic.cpp src/tac_generator.cpp src/optimizer.cpp src/codegen.cpp

.PHONY: all clean test

# Windows configuration
ifeq ($(OS),Windows_NT)

TARGET = build/plaincode.exe

all: $(TARGET)

$(TARGET): $(SOURCES) $(wildcard include/*.h)
	@if not exist build mkdir build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

test:
	@powershell -NoProfile -Command "$$ErrorActionPreference='Continue'; $$failed=0; New-Item -ItemType Directory -Force build/tests | Out-Null; Get-ChildItem tests/*.cpp | ForEach-Object { $$name=$$_.BaseName; $$exe='build/tests/'+$$name+'.exe'; Write-Host ('[BUILD] '+$$name); & g++ -std=c++17 -Wall -Wextra -Werror -pedantic -I include $$_.FullName src/lexer.cpp src/parser.cpp src/semantic.cpp src/tac_generator.cpp src/optimizer.cpp src/codegen.cpp -o $$exe; if ($$LASTEXITCODE -ne 0) { $$failed++; return }; Write-Host ('[RUN] '+$$name); & $$exe; if ($$LASTEXITCODE -ne 0) { Write-Host ('[FAILED OR BLOCKED] '+$$name); $$failed++ } }; Write-Host ('Test files with failures or blocks: '+$$failed)"

clean:
	@if exist build rmdir /s /q build

# Linux / macOS configuration
else

TARGET = build/plaincode

all: $(TARGET)

$(TARGET): $(SOURCES) $(wildcard include/*.h)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

test:
	@mkdir -p build/tests
	@set -e; for source in tests/*.cpp; do \
		name=$$(basename $$source .cpp); \
		echo "[BUILD] $$name"; \
		$(CXX) $(CXXFLAGS) $$source $(LIB_SOURCES) -o build/tests/$$name; \
		echo "[RUN] $$name"; \
		./build/tests/$$name; \
	done

clean:
	rm -rf build

endif
