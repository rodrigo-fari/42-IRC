</div>

<p align='center'>
		<img src="https://capsule-render.vercel.app/api?type=waving&color=4fa69a&height=300&section=header&text=Internet+Relay+Chat&fontSize=90&animation=fadeIn&fontAlignY=38&desc=IRC+server+built+in+C%2B%2B+98&descAlignY=51&descAlign=62"/>

<p align="center">
	<img src="https://img.shields.io/static/v1?label=C%2B%2B&message=IRC%20Server&color=4fa69a&style=for-the-badge&logo=" alt="Project Overview"/>

</p>
</div>

---

<div align="left">

<h3 align="center">Overview</h3>

This project implements a IRC (Internet Relay Chat) server in C++.
It focuses on handling concurrent clients, command parsing and dispatching, channel management, and protocol-compliant replies.

---

<h3 align="center">Repository Layout</h3>

- `inc/`
- `src/`
- `assets/`

The codebase is split into modules for networking, parser/dispatcher flow, command handlers, repositories, and core server state.

---

<h3 align="center">Build</h3>

From inside the project folder:

```bash
make
```

---

<h3 align="center">Run</h3>

After building:

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 mypassword
```

---

<h3 align="center">Notes</h3>

- Requires a C++ compiler with C++98 support.
- Includes diagrams in `assets/png/` and UML sources in `assets/uml/`.
- Useful references:
	- RFC 1459
	- RFC 2812
	- RFC 2813
	- https://modern.ircdocs.horse/
	- https://hexchat.readthedocs.io/en/latest/



</div>
