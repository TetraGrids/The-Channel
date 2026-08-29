## Goal

Connect to a specific `channeld` or `keyd` host to send COMMAND

`chan` and `keyd` can connect to a specific node by using the `--url` or `--wallet-url` optional arguments, respectively, followed by the http address and port number these services are listening to.

[[info | Default address:port]]
| If no optional arguments are used (i.e. `--url` or `--wallet-url`), `chan` attempts to connect to a local `channeld` or `keyd` running at localhost `127.0.0.1` and default port `8888`.

## Before you begin

* Install the currently supported version of `chan`

## Steps
### Connecting to Channeld

```sh
chan -url http://channeld-host:8888 COMMAND
```

### Connecting to Keyd

```sh
chan --wallet-url http://keyd-host:8888 COMMAND
```
