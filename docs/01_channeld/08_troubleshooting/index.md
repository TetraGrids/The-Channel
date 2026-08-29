---
content_title: Channeld Troubleshooting
---

### "Database dirty flag set (likely due to unclean shutdown): replay required"

`channeld` needs to be shut down cleanly. To ensure this is done, send a `SIGTERM`, `SIGQUIT` or `SIGINT` and wait for the process to shutdown. Failing to do this will result in this error. If you get this error, your only recourse is to replay by starting `channeld` with `--replay-blockchain`

### "Memory does not match data" Error at Restart

If you get an error such as `St9exception: content of memory does not match data expected by executable` when trying to start `channeld`, try restarting `channeld` with one of the following options (you can use `channeld --help` to get a full listing of these).

```
Command Line Options for eosio::chain_plugin:
    --force-all-checks                    do not skip any checks that can be
                                          skipped while replaying irreversible
                                          blocks
    --replay-blockchain                   clear chain state database and replay
                                          all blocks
    --hard-replay-blockchain              clear chain state database, recover as
                                          many blocks as possible from the block
                                          log, and then replay those blocks
    --delete-all-blocks                   clear chain state database and block
                                          log
```

### "Could not grow database file to requested size." Error

Start `channeld` with `--shared-memory-size-mb 1024`. A 1 GB shared memory file allows approximately half a million transactions.

### What version of Antelope am I running/connecting to?

If defaults can be used, then `chan get info` will output a block that contains a field called `server_version`.  If your `channeld` is not using the defaults, then you need to know the URL of the `channeld`. In that case, use the following with your `channeld` URL:

```sh
chan --url http://localhost:8888 get info
```

To focus only on the version line within the block:

```sh
chan --url http://localhost:8888 get info | grep server_version
```

### Error 3070000: WASM Exception Error

If you try to deploy the `ra.bios` contract or `ra.system` contract in an attempt to boot an Antelope-based blockchain and you get the following error or similar: `Publishing contract... Error 3070000: WASM Exception Error Details: env.set_proposed_producers_ex unresolveable`, it is because you have to activate the `PREACTIVATE_FEATURE` protocol first. 
