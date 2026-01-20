# FSUIPC LIB

An FSUIPC library to read the simulator radio frequency  
This library is designed to make python easier to call  
The definition of the data structure can be found in this file[`src/fsuipc_export.h`](src/fsuipc_export.h)

Use this lib, you can

- Connect to flight simulator like MFS2020, MFS2024. Or connect to XPlane via [`XPUIPC`](https://schiratti.com/xpuipc.html)
- Get aircraft's frequency and set them.(XPUIPC don't support modern offset so it don't support 8.33kHz frequency)

That's all, very tiny LIB

For code examples, see files under [`examples`](examples/main.py)

## License

MIT License

Copyright © 2025-2026 Half_nothing

No additional terms
