# TCP-IP Multi-Client Chat Application

A terminal-based client/server chat system written in **C** using POSIX sockets and `pthread`.  The server assigns each client a unique ID, enabling targeted private messages in addition to broadcast.

---

## Components

| File | Role |
|------|------|
| `TCP-IP Server.c` | Accepts connections, assigns IDs, relays messages to targets | 
| `TCP-IP First Client.c` | Simple client (interactive stdin) |
| `TCP-IP Second Client.c` | Alternate client variant (example of multiple binaries) |

---

## Features

* **Concurrent clients** – server handles up to `MAX_CLIENTS` connections via a thread per client.
* **ID assignment** – upon login the server responds `Your ID is: N`; clients can prepend the destination ID to any message to send privately.
* **Graceful exit** – typing `exit` closes sockets and cleans up threads.
* **Cross-platform** – compiles on Linux/macOS; uses standard BSD sockets.

---

## Build

```bash
# Compile server
gcc -pthread -o server "TCP-IP Server.c"

# Compile clients
gcc -pthread -o client1 "TCP-IP First Client.c"
gcc -pthread -o client2 "TCP-IP Second Client.c"
```

---

## Run Example

Terminal 1 (server):
```bash
./server
```

Terminal 2 (client A):
```bash
./client1
Hello everyone!
```

Terminal 3 (client B):
```bash
./client2
1 Hey client 1, private message!
```

---

## Notes

* All sockets use **localhost (127.0.0.1)** on port **8080** – adjust in source if needed.
* For learning purposes: no encryption/authentication implemented.

---

## License

Released under the MIT License.
