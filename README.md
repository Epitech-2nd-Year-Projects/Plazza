### Plazza

A simulation of a pizza restaurant in C++, using multi-threading and IPC.

## Build

```bash
mkdir build && cd build

cmake ..

cmake --build .
```

The `plazza` executable will be placed in the root directory.

## Documentation

We use Doxygen to generate API documentation. A Doxyfile is provided at the project root.

```bash
doxygen Doxyfile
```

HTML docs will be available under `docs/doxygen/html/index.html`.
