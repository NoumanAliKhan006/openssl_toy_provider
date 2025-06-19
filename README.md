<div align="center">

  `MyProvider`

</div>

> [!CAUTION]
>
> ### For Learning Purposes Only
>
> This project is built for **educational purposes** and is not **intended for production use**.
>
> It serves as a **minimal example** to help developers understand how to **implement OpenSSL providers**.

<details open="open">
<summary>Table of Contents</summary>

- [About](#about)
- [Features](#features)
- [Requirements](#requirements)
- [Build Instructions](#build-instructions)
</details>

---

## About

`MyProvider` is a minimal OpenSSL 3.x provider designed for learning and experimentation. It demonstrates how to implement and register custom cryptographic algorithms, such as message digests, using the OpenSSL provider interface.

This project is **not** intended for production use and serves purely educational and demonstrative purposes.

## Features

- Implements a **dummy** digest algorithm called `BLARG`
- Demonstrates provider lifecycle: initialization, teardown, error handling
- Exposes provider metadata via `get_params` and `gettable_params`
- Basic logging via `stderr` for learning and debugging

## Requirements

- OpenSSL **3.0+**
- A C compiler (e.g., GCC, Clang)
- `make` (optional)

## Build Instructions

### 1. Clone the repository
```bash
git clone https://github.com/yourusername/myprovider.git
cd myprovider
```
### 2. Build the shared object using Makefile
```bash
make
```

> ⚠️ The provided Makefile assumes OpenSSL is installed system-wide. You may need to edit it if your OpenSSL headers or libraries are in a non-standard location.